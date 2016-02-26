#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec3 Color;
out vec2 UV;

uniform mat4 VP;
uniform mat4 M;

void main()
{
    UV = uv;
    gl_Position = VP * M * vec4(position, 1.0);
}
