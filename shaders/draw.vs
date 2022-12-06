#version 330 core

layout (location = 0) in int index;

out vec4 Color;

uniform sampler2D positions;
uniform sampler2D velocities;

void main() {
    vec2 pos = texelFetch(positions,  ivec2(index, 0), 0).xy;
    gl_Position = vec4(pos, 0.0f, 1.0f);
    Color = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}
