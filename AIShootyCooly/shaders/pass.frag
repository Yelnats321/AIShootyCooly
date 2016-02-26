#version 330 core
in vec2 UV;

layout (location = 0) out vec4 outColor;

uniform sampler2D diffuseTex;

void main() {
    outColor = vec4(texture(diffuseTex, UV).rgb, 1.0);
}
