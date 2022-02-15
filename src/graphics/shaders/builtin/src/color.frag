#version 460 core
layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform ColorBlock {
    vec3 color;
};

void main()
{
    FragColor = vec4(color, 1.0);
}
