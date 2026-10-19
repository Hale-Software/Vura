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

#pragma once

#include <QByteArray>
#include <QSize>
#include <QString>

#include <memory>

QT_BEGIN_NAMESPACE
class QOpenGLBuffer;
class QOpenGLShaderProgram;
class QOpenGLVertexArrayObject;
QT_END_NAMESPACE

namespace media {

namespace ffmpeg {
struct VideoFrame;
}

/// Draws YUV420P frames with a shader doing the colour conversion.
///
/// Every method except forgetFrame()/hasFrame() touches GL and must run with
/// the output's context current -- in practice, from inside the render
/// callback GLVideoWidget invokes from paintGL().
class GLYuvRenderer
{
public:
    GLYuvRenderer();
    ~GLYuvRenderer();

    bool initialize(QString *error);
    void destroy();

    void upload(const ffmpeg::VideoFrame &frame);
    void render(const QSize &surface, unsigned int fbo);

    /// Next render() draws black. No GL involved, safe any time.
    void forgetFrame() { m_hasFrame = false; }
    bool hasFrame() const { return m_hasFrame; }

private:
    void uploadPlane(int index, const unsigned char *data, int stride, int width, int height);

    std::unique_ptr<QOpenGLShaderProgram> m_program;
    std::unique_ptr<QOpenGLBuffer> m_vbo;
    std::unique_ptr<QOpenGLVertexArrayObject> m_vao;
    unsigned int m_textures[3] = {0, 0, 0};
    QSize m_planeSizes[3];

    unsigned int m_internalFormat = 0;
    unsigned int m_pixelFormat = 0;
    bool m_rowLengthSupported = false;
    QByteArray m_scratch;

    bool m_hasFrame = false;
    bool m_fullRange = false;
    bool m_bt709 = false;
    double m_aspect = 16.0 / 9.0;
};

}
