//
// Created by Tobias on 10/21/2020.
//

#ifndef SD3D_BUILTIN_SHADERS_H
#define SD3D_BUILTIN_SHADERS_H

namespace sd3d::shaders {

[[maybe_unused]] constexpr const char *error_vertex_src() {
	constexpr const char *src = {
		R"(
#version 460 core
layout (location = 0) in vec3 aPos;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)"
	};
	return src;
}
[[maybe_unused]] constexpr const char *error_fragment_src() {
	constexpr const char *src = {
		R"(
#version 460 core
out vec4 FragColor;

in vec2 texCoord;

void main()
{
    FragColor = vec4(1.0, 0.0, 1.0, 0.0);
}
)"
	};
	return src;
}

[[maybe_unused]] constexpr const char *color_vertex_src() {
	constexpr const char *src = {
		R"(
#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)"
	};
	return src;
}
[[maybe_unused]] constexpr const char *color_fragment_src() {
	constexpr const char *src = {
		R"frag(
#version 460 core
out vec4 FragColor;

uniform vec3 color;

void main()
{
    FragColor = vec4(color, 1.0);
}
)frag"
	};
	return src;
}

[[maybe_unused]] constexpr const char *lit_vertex_src() {
	constexpr const char *src = {
		R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragPos = vec3(model * vec4(aPos, 1.0));
    texCoord = aTexCoord;
    // TODO understand normal matrix -> http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/
    normal = normalMatrix * aNormal;
}
)"
	};
	return src;
}

[[maybe_unused]] constexpr const char *lit_fragment_src() {
	constexpr const char *src = {
		R"(
#version 460 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    float shininess;
};

struct PointLight {
    vec3 position;

    float diffuse;
    float specular;
    vec3 color;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;

    vec3 color;
    float ambient;
    float diffuse;
    float specular;
};

struct ColorInfo {
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 color;

uniform Material material;

uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 camPos;

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 viewDir, ColorInfo colors);
vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, ColorInfo colors);

ColorInfo sample_colors();

void main()
{
    vec3 unitNormal = normalize(normal);
    vec3 viewDir = normalize(camPos - fragPos);

    ColorInfo colors = sample_colors();

    vec3 result = calc_dir_light(dirLight, unitNormal, viewDir, colors);
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += calc_point_light(pointLights[i], unitNormal, fragPos, viewDir, colors);
    }
    FragColor = vec4(result, 1.0);
}

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 viewDir, ColorInfo colors) {
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * colors.diffuse;
    vec3 diffuse = light.diffuse * diff * colors.diffuse;
    vec3 specular = light.specular * spec * colors.specular;
    return (ambient + diffuse + specular) * light.color;
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, ColorInfo colors) {
    if(light.constant == 0.0 && light.linear == 0.0 && light.quadratic == 0.0) {
        return vec3(0.0);
    }
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // combine results
    vec3 diffuse  = light.diffuse  * diff * colors.diffuse;
    vec3 specular = light.specular * spec * colors.specular;
    return (diffuse + specular) * light.color * attenuation;
}

// TODO support spot lights

ColorInfo sample_colors() {
    vec3 diff = vec3(texture(material.texture_diffuse1, texCoord));
    diff += vec3(texture(material.texture_diffuse2, texCoord));
    diff += vec3(texture(material.texture_diffuse3, texCoord));
    diff *= color;

    vec3 spec = vec3(texture(material.texture_specular1, texCoord));
    spec += vec3(texture(material.texture_specular2, texCoord));
    return ColorInfo(diff, spec);
}
)"
	};
	return src;
}

} // namespace sd3d::shaders

#undef SHADER

#endif // SD3D_BUILTIN_SHADERS_H
