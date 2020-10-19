#version 460 core
struct Material {
    sampler2D diffuse;
// TODO maybe this needs to be white to enable white highlights
    sampler2D specular;
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

out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform Material material;

uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 camPos;

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 viewDir);
vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 unitNormal = normalize(normal);
    vec3 viewDir = normalize(camPos - fragPos);

    vec3 result = calc_dir_light(dirLight, unitNormal, viewDir);
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += calc_point_light(pointLights[i], unitNormal, fragPos, viewDir);
    }
    FragColor = vec4(result, 1.0);
}

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
    return (ambient + diffuse + specular) * light.color;
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
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
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, texCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
    return (diffuse + specular) * light.color * attenuation;
}

// TODO support spot lights
