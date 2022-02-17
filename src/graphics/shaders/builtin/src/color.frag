#version 460 core
layout (location = 0) out vec4 FragColor;

layout (std430, binding = 1) buffer ColorBlock {
    vec3 color;
};

void main()
{
    FragColor = vec4(color, 1.0);
}
