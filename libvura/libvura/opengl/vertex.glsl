#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    // Flip the Y coordinate because OpenGL expects the origin at the bottom-left,
    // but video frames store the origin at the top-left.
    TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
}