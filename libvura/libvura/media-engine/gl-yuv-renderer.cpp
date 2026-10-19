/*******************************************************************************
     Copyright (c) 2026 by Andrew Hale <halea2196@gmail.com>

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#include "gl-yuv-renderer.h"

#include "ffmpeg-pipeline.h"

extern "C" {
#include <libavutil/frame.h>
}

#include <QGenericMatrix>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QVector3D>

#include <cstring>

// Not every GL header set defines these; the values are fixed by the spec.
#ifndef GL_RED
#define GL_RED 0x1903
#endif
#ifndef GL_R8
#define GL_R8 0x8229
#endif
#ifndef GL_LUMINANCE
#define GL_LUMINANCE 0x1909
#endif
#ifndef GL_UNPACK_ROW_LENGTH
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#endif

namespace media {
namespace {

// Legacy GLSL covers desktop compatibility profiles (QOpenGLWidget's
// default) and GLES 2/3. Core profiles need the 150 variant.
const char *kVertexLegacy = R"(
attribute vec2 a_pos;
attribute vec2 a_tex;
varying vec2 v_tex;
void main() {
    v_tex = a_tex;
    gl_Position = vec4(a_pos, 0.0, 1.0);
}
)";

const char *kFragmentLegacy = R"(
#ifdef GL_ES
precision mediump float;
#endif
varying vec2 v_tex;
uniform sampler2D u_y;
uniform sampler2D u_u;
uniform sampler2D u_v;
uniform mat3 u_matrix;
uniform vec3 u_offset;
void main() {
    vec3 yuv = vec3(texture2D(u_y, v_tex).r, texture2D(u_u, v_tex).r, texture2D(u_v, v_tex).r) - u_offset;
    gl_FragColor = vec4(clamp(u_matrix * yuv, 0.0, 1.0), 1.0);
}
)";

const char *kVertexCore = R"(#version 150
in vec2 a_pos;
in vec2 a_tex;
out vec2 v_tex;
void main() {
    v_tex = a_tex;
    gl_Position = vec4(a_pos, 0.0, 1.0);
}
)";

const char *kFragmentCore = R"(#version 150
in vec2 v_tex;
out vec4 fragColor;
uniform sampler2D u_y;
uniform sampler2D u_u;
uniform sampler2D u_v;
uniform mat3 u_matrix;
uniform vec3 u_offset;
void main() {
    vec3 yuv = vec3(texture(u_y, v_tex).r, texture(u_u, v_tex).r, texture(u_v, v_tex).r) - u_offset;
    fragColor = vec4(clamp(u_matrix * yuv, 0.0, 1.0), 1.0);
}
)";

// x, y, u, v -- a full-screen strip. Row 0 of the image is uploaded first
// and sits at t = 0, so the top of the screen maps to t = 0.
const float kQuad[] = {
    -1.f, -1.f, 0.f, 1.f,
     1.f, -1.f, 1.f, 1.f,
    -1.f,  1.f, 0.f, 0.f,
     1.f,  1.f, 1.f, 0.f,
};

QMatrix3x3 yuvToRgb(bool bt709, bool fullRange)
{
    // Rows are R, G, B; columns are Y, U, V (after the offset is removed).
    // Limited range folds the 219/224 scale into the coefficients.
    const float y = fullRange ? 1.0f : 1.164384f;
    float m[9];
    if (bt709) {
        const float c = fullRange ? 1.0f : 1.138393f; // 255/224
        m[0] = y; m[1] = 0.0f;           m[2] = 1.5748f * c;
        m[3] = y; m[4] = -0.187324f * c; m[5] = -0.468124f * c;
        m[6] = y; m[7] = 1.8556f * c;    m[8] = 0.0f;
    } else {
        const float c = fullRange ? 1.0f : 1.138393f;
        m[0] = y; m[1] = 0.0f;           m[2] = 1.402f * c;
        m[3] = y; m[4] = -0.344136f * c; m[5] = -0.714136f * c;
        m[6] = y; m[7] = 1.772f * c;     m[8] = 0.0f;
    }
    return QMatrix3x3(m);
}

} // namespace

GLYuvRenderer::GLYuvRenderer() = default;

GLYuvRenderer::~GLYuvRenderer() = default;

bool GLYuvRenderer::initialize(QString *error)
{
    QOpenGLContext *ctx = QOpenGLContext::currentContext();
    if (!ctx) {
        if (error)
            *error = QStringLiteral("No current OpenGL context.");
        return false;
    }
    QOpenGLFunctions *f = ctx->functions();

    const QSurfaceFormat format = ctx->format();
    const bool es = ctx->isOpenGLES();
    const bool modern = format.majorVersion() >= 3;
    const bool core = !es && format.profile() == QSurfaceFormat::CoreProfile
                      && format.version() >= qMakePair(3, 2);

    // GL_LUMINANCE is gone from core profiles; GL_RED is missing from GLES2.
    m_internalFormat = modern ? GL_R8 : GL_LUMINANCE;
    m_pixelFormat = modern ? GL_RED : GL_LUMINANCE;
    m_rowLengthSupported = !es || modern;

    m_program = std::make_unique<QOpenGLShaderProgram>();
    const bool ok = m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, core ? kVertexCore : kVertexLegacy)
                    && m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, core ? kFragmentCore : kFragmentLegacy);
    m_program->bindAttributeLocation("a_pos", 0);
    m_program->bindAttributeLocation("a_tex", 1);
    if (!ok || !m_program->link()) {
        if (error)
            *error = QStringLiteral("Video shader failed to build: %1").arg(m_program->log());
        m_program.reset();
        return false;
    }

    m_vao = std::make_unique<QOpenGLVertexArrayObject>();
    m_vao->create(); // fails harmlessly where VAOs are unsupported; Binder copes

    m_vbo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(kQuad, sizeof kQuad);
    m_vbo->release();

    f->glGenTextures(3, m_textures);
    for (unsigned int tex : m_textures) {
        f->glBindTexture(GL_TEXTURE_2D, tex);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    f->glBindTexture(GL_TEXTURE_2D, 0);

    m_program->bind();
    m_program->setUniformValue("u_y", 0);
    m_program->setUniformValue("u_u", 1);
    m_program->setUniformValue("u_v", 2);
    m_program->release();
    return true;
}

void GLYuvRenderer::destroy()
{
    if (QOpenGLContext *ctx = QOpenGLContext::currentContext()) {
        if (m_textures[0])
            ctx->functions()->glDeleteTextures(3, m_textures);
    }
    for (unsigned int &tex : m_textures)
        tex = 0;
    for (QSize &s : m_planeSizes)
        s = QSize();
    m_vbo.reset();
    m_vao.reset();
    m_program.reset();
    m_hasFrame = false;
}

void GLYuvRenderer::uploadPlane(int index, const unsigned char *data, int stride, int width, int height)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glBindTexture(GL_TEXTURE_2D, m_textures[index]);

    if (m_planeSizes[index] != QSize(width, height)) {
        f->glTexImage2D(GL_TEXTURE_2D, 0, GLint(m_internalFormat), width, height, 0, m_pixelFormat,
                        GL_UNSIGNED_BYTE, nullptr);
        m_planeSizes[index] = QSize(width, height);
    }

    f->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (stride == width) {
        f->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, m_pixelFormat, GL_UNSIGNED_BYTE, data);
    } else if (m_rowLengthSupported) {
        f->glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
        f->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, m_pixelFormat, GL_UNSIGNED_BYTE, data);
        f->glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    } else {
        // GLES2: no row length, so strip the padding by hand.
        m_scratch.resize(qsizetype(width) * height);
        for (int row = 0; row < height; ++row)
            std::memcpy(m_scratch.data() + qsizetype(row) * width, data + qsizetype(row) * stride, size_t(width));
        f->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, m_pixelFormat, GL_UNSIGNED_BYTE,
                           m_scratch.constData());
    }
}

void GLYuvRenderer::upload(const ffmpeg::VideoFrame &frame)
{
    const AVFrame *av = frame.frame;
    if (!av || !m_program || av->width <= 0 || av->height <= 0)
        return;
    if (av->linesize[0] < 0 || av->linesize[1] < 0 || av->linesize[2] < 0)
        return; // bottom-up frames don't occur for YUV420P decoders in practice

    const int cw = (av->width + 1) / 2;
    const int ch = (av->height + 1) / 2;
    uploadPlane(0, av->data[0], av->linesize[0], av->width, av->height);
    uploadPlane(1, av->data[1], av->linesize[1], cw, ch);
    uploadPlane(2, av->data[2], av->linesize[2], cw, ch);
    QOpenGLContext::currentContext()->functions()->glBindTexture(GL_TEXTURE_2D, 0);

    m_fullRange = frame.fullRange;
    m_bt709 = frame.bt709;
    m_aspect = frame.aspect > 0.0 ? frame.aspect : double(av->width) / av->height;
    m_hasFrame = true;
}

void GLYuvRenderer::render(const QSize &surface, unsigned int fbo)
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    const int w = surface.width();
    const int h = surface.height();
    if (w <= 0 || h <= 0)
        return;

    f->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    f->glViewport(0, 0, w, h);
    f->glClearColor(0.f, 0.f, 0.f, 1.f);
    f->glClear(GL_COLOR_BUFFER_BIT);

    if (!m_hasFrame || !m_program)
        return;

    // Letterbox / pillarbox to the display aspect ratio.
    int vw = w;
    int vh = h;
    if (m_aspect > double(w) / h)
        vh = int(w / m_aspect + 0.5);
    else
        vw = int(h * m_aspect + 0.5);
    f->glViewport((w - vw) / 2, (h - vh) / 2, vw, vh);

    f->glDisable(GL_BLEND);
    f->glDisable(GL_DEPTH_TEST);

    m_program->bind();
    m_program->setUniformValue("u_matrix", yuvToRgb(m_bt709, m_fullRange));
    m_program->setUniformValue("u_offset", QVector3D(m_fullRange ? 0.f : 16.f / 255.f, 0.5f, 0.5f));

    for (int i = 0; i < 3; ++i) {
        f->glActiveTexture(GL_TEXTURE0 + i);
        f->glBindTexture(GL_TEXTURE_2D, m_textures[i]);
    }

    {
        QOpenGLVertexArrayObject::Binder vaoBinder(m_vao.get());
        m_vbo->bind();
        m_program->enableAttributeArray(0);
        m_program->enableAttributeArray(1);
        m_program->setAttributeBuffer(0, GL_FLOAT, 0, 2, 4 * sizeof(float));
        m_program->setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(float), 2, 4 * sizeof(float));
        f->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        m_program->disableAttributeArray(0);
        m_program->disableAttributeArray(1);
        m_vbo->release();
    }

    for (int i = 2; i >= 0; --i) {
        f->glActiveTexture(GL_TEXTURE0 + i);
        f->glBindTexture(GL_TEXTURE_2D, 0);
    }
    m_program->release();
}

}
