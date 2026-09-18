#version 330 core
in vec2 TexCoord;
out vec4 color;

// Our three separate planes from the AVFrame
uniform sampler2D texY;
uniform sampler2D texU;
uniform sampler2D texV;

void main() {
    // Read the grayscale values (using .r because they are single-channel textures)
    float y = texture(texY, TexCoord).r;
    float u = texture(texU, TexCoord).r - 0.5;
    float v = texture(texV, TexCoord).r - 0.5;

    // YUV to RGB Conversion Matrix
    float r = y + 1.402 * v;
    float g = y - 0.344 * u - 0.714 * v;
    float b = y + 1.772 * u;

    color = vec4(r, g, b, 1.0);
}