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
