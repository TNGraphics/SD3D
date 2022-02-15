#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec3 fragPos;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec2 texCoord;

layout (binding = 0) uniform ColorBlock {
    mat4 model;
    mat3 normalMatrix;
    mat4 view;
    mat4 projection;
};

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragPos = vec3(model * vec4(aPos, 1.0));
    texCoord = aTexCoord;
    // TODO understand normal matrix -> http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/
    normal = normalMatrix * aNormal;
}
