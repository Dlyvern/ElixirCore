#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

const int LIGHT_TYPE_DIRECTIONAL = 0;
const int LIGHT_TYPE_POINT       = 1;
const int LIGHT_TYPE_SPOT        = 2;

struct Light {
    vec3 position;
    vec3 color;
    float strength;
    float radius;

    vec3 direction;
    float cutoff;
    float outerCutoff;

    int type;
};

uniform vec3 viewPos;
uniform Light light;

uniform sampler2D u_Diffuse;
uniform sampler2D u_Normal;
uniform sampler2D u_Metallic;
uniform sampler2D u_Roughness;
uniform sampler2D u_AO;
uniform sampler2D shadowMap;

uniform bool use_Diffuse;
uniform bool use_Normal;
uniform bool use_Metallic;
uniform bool use_Roughness;
uniform bool use_AO;

uniform vec3 baseColor;

#define MAX_LIGHTS 2
uniform Light lights[MAX_LIGHTS];

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

void main()
{
    vec3 albedo     = use_Diffuse   ? texture(u_Diffuse, fs_in.TexCoords).rgb   : baseColor;
    float metallic  = use_Metallic  ? texture(u_Metallic, fs_in.TexCoords).r    : 0.0;
    float roughness = use_Roughness ? texture(u_Roughness, fs_in.TexCoords).r   : 1.0;
    float ao        = use_AO        ? texture(u_AO, fs_in.TexCoords).r          : 1.0;

    vec3 tangentNormal = use_Normal ?  texture(u_Normal, fs_in.TexCoords).xyz * 2.0 - 1.0 : baseColor;

    vec3 normal = normalize(fs_in.Normal);
    vec3 result = vec3(0.0);

    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        Light lightV = lights[i];

        vec3 lightDir = lightV.type == LIGHT_TYPE_DIRECTIONAL ? normalize(-lightV.direction) : normalize(lightV.position - fs_in.FragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 viewDir = normalize(viewPos - fs_in.FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float shininess = mix(8.0, 128.0, 1.0 - roughness);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

        vec3 ambient  = 0.03 * albedo * ao;
        vec3 diffuse  = diff * albedo * lightV.color * lightV.strength;
        vec3 specular = spec * mix(vec3(0.04), albedo, metallic) * lightV.strength;

        vec3 lightResult = ambient + diffuse + specular;

        if(lightV.type == LIGHT_TYPE_DIRECTIONAL)
        {
            float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, lightDir);
            lightResult *= (1.0 - shadow);
        }

        result += lightResult;
    }

    FragColor = vec4(result, 1.0);
}