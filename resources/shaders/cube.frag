#version 420 core

out vec4 FragColor;
#define MAX_LIGHTS 4

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace[MAX_LIGHTS];
} fs_in;


const int LIGHT_TYPE_DIRECTIONAL = 0;
const int LIGHT_TYPE_POINT       = 1;
const int LIGHT_TYPE_SPOT        = 2;

struct Light
{
    vec3 position;
    vec3 color;
    float strength;
    float radius;

    vec3 direction;
    float cutoff;
    float outerCutoff;

    int type;

    bool castShadows;

    float farPlane;
};

uniform sampler2D u_Diffuse;
uniform sampler2D u_Normal;
uniform sampler2D u_Metallic;
uniform sampler2D u_Roughness;
uniform sampler2D u_AO;

uniform bool use_Diffuse;
uniform bool use_Normal;
uniform bool use_Metallic;
uniform bool use_Roughness;
uniform bool use_AO;

uniform vec3 baseColor;

uniform vec3 viewPos;

uniform Light lights[MAX_LIGHTS];

uniform sampler2D shadowMap;

uniform sampler2D spotShadowMaps[MAX_LIGHTS];
uniform samplerCube pointShadowMaps[MAX_LIGHTS];


float PointLightShadow(vec3 fragPos, Light light, samplerCube depthMap)
{
    vec3 fragToLight = fragPos - light.position;
    float currentDepth = length(fragToLight);
    float closestDepth = texture(depthMap, fragToLight).r * light.farPlane;

    // Bias to avoid shadow acne (based on angle and distance)
    float bias = 0.15; // You can also make this dynamic
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir, sampler2D shadowMapToUse)
{
    //    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //    projCoords = projCoords * 0.5 + 0.5;
    //
    //    if (projCoords.z > 1.0)
    //    return 0.0;
    //
    //    float currentDepth = projCoords.z;
    //    vec3 normal = normalize(fs_in.Normal);
    //    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    //
    //    float shadow = 0.0;
    //    float samples = 4.0; // 4x4 kernel (16 samples)
    //    float range = 1.5;   // softer shadows = bigger range
    //    vec2 texelSize = 1.0 / textureSize(shadowMapToUse, 0);
    //
    //    for (float x = -samples; x <= samples; ++x)
    //    {
    //        for (float y = -samples; y <= samples; ++y)
    //        {
    //            float pcfDepth = texture(shadowMapToUse, projCoords.xy + vec2(x, y) * texelSize * range / samples).r;
    //            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    //        }
    //    }
    //
    //    shadow /= pow((2.0 * samples + 1.0), 2.0); // normalize
    //
    //    return shadow;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;

    float closestDepth = texture(shadowMapToUse, projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 normal = normalize(fs_in.Normal);

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMapToUse, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMapToUse, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    if(projCoords.z > 1.0)
    shadow = 0.0;

    return shadow;
}

vec3 getAlbedo()
{
    return use_Diffuse ? texture(u_Diffuse, fs_in.TexCoords).rgb : baseColor;
}

float getMetallic()
{
    return use_Metallic ? texture(u_Metallic, fs_in.TexCoords).r : 0.0;
}

float getRoughness()
{
    return use_Roughness ? texture(u_Roughness, fs_in.TexCoords).r : 1.0;
}

float getAO()
{
    return use_AO ? texture(u_AO, fs_in.TexCoords).r : 1.0;
}

vec3 getNormal()
{
    return normalize(fs_in.Normal);
}

vec3 getViewDir(vec3 fragPos)
{
    return normalize(viewPos - fragPos);
}

float getSpecular(vec3 normal, vec3 lightDir, vec3 viewDir, float roughness)
{
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float shininess = mix(8.0, 128.0, 1.0 - roughness);
    return pow(max(dot(normal, halfwayDir), 0.0), shininess);
}

vec3 calculateDirectionalLight(Light light, vec3 albedo, float roughness, float metallic, float ao, int index)
{
    vec3 normal = getNormal();
    vec3 lightDir = normalize(-light.direction);
    vec3 viewDir = getViewDir(fs_in.FragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = getSpecular(normal, lightDir, viewDir, roughness);

    vec3 ambient = 0.03 * albedo * ao;
    vec3 diffuse = diff * albedo * light.color * light.strength;
    vec3 specular = spec * mix(vec3(0.04), albedo, metallic) * light.strength;

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace[index], lightDir, shadowMap);
    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);

    return ambient + diffuse + specular;
}

vec3 calculateSpotLight(Light light, vec3 albedo, float roughness, float metallic, float ao, int index)
{
    vec3 normal = getNormal();
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    vec3 viewDir = getViewDir(fs_in.FragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = getSpecular(normal, lightDir, viewDir, roughness);

    vec3 ambient = 0.03 * albedo * ao;
    vec3 diffuse = diff * albedo * light.color * light.strength;
    vec3 specular = spec * mix(vec3(0.04), albedo, metallic) * light.strength;

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    diffuse *= intensity;
    specular *= intensity;

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace[index], lightDir, spotShadowMaps[index]);
    diffuse *= (1.0 - shadow);
    specular *= (1.0 - shadow);

    return ambient + diffuse + specular;
}

vec3 calculatePointLight(Light light, vec3 albedo, float roughness, float metallic, float ao, int index)
{
    vec3 normal = getNormal();
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    vec3 viewDir = getViewDir(fs_in.FragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = getSpecular(normal, lightDir, viewDir, roughness);

    vec3 ambient = 0.03 * albedo * ao;
    vec3 diffuse = diff * albedo * light.color * light.strength;
    vec3 specular = spec * mix(vec3(0.04), albedo, metallic) * light.strength;

    float distance = length(light.position - fs_in.FragPos);
    float attenuation = clamp(1.0 - (distance / light.radius), 0.0, 1.0);

    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = PointLightShadow(fs_in.FragPos, light, pointShadowMaps[index]);
    //    diffuse *= (1.0 - shadow);
    //    specular *= (1.0 - shadow);

    return ambient + diffuse + specular;
}

void main()
{
    vec3 albedo     = getAlbedo();
    float metallic  = getMetallic();
    float roughness = getRoughness();
    float ao        = getAO();

    vec3 result = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        Light lightV = lights[i];

        if (lightV.type == LIGHT_TYPE_DIRECTIONAL)
        result += calculateDirectionalLight(lightV, albedo, roughness, metallic, ao, i);
        else if (lightV.type == LIGHT_TYPE_SPOT)
        result += calculateSpotLight(lightV, albedo, roughness, metallic, ao, i);
        else if (lightV.type == LIGHT_TYPE_POINT)
        result += calculatePointLight(lightV, albedo, roughness, metallic, ao, i);
    }

    FragColor = vec4(result, 1.0);
}