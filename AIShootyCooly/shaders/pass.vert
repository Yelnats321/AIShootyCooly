#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 Color;

uniform mat4 VP;

void main()
{
    Color = (color + vec3(1)) / 2.f;
    gl_Position = VP * vec4(position, 1.0);
}
