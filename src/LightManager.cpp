#include "LightManager.hpp"
#include <iostream>

LightManager& LightManager::instance()
{
    static LightManager instance;
    return instance;
}

void LightManager::addLight(lighting::Light* light)
{
    m_lights.push_back(light);
}

void LightManager::removeLight(lighting::Light *light)
{
    auto it = std::find(m_lights.begin(), m_lights.end(), light);

    if (it != m_lights.end())
        m_lights.erase(it);
}

std::vector<lighting::Light*> LightManager::getLights() const
{
    return m_lights;
}

lighting::Light* LightManager::getDirectionalLight() const
{
    for (const auto& light : m_lights)
        if (light->type == lighting::LightType::DIRECTIONAL)
            return light;

    return nullptr;
}

std::vector<lighting::Light*> LightManager::getSpotLights() const
{
    std::vector<lighting::Light*> spotLights;

    for (const auto& light : m_lights)
        if (light->type == lighting::LightType::SPOT)
            spotLights.push_back(light);

    return spotLights;
}

std::vector<lighting::Light*> LightManager::getPointLights() const
{
    std::vector<lighting::Light*> pointLights;

    for (const auto& light : m_lights)
        if (light->type == lighting::LightType::POINT)
            pointLights.push_back(light);

    return pointLights;
}

void LightManager::sendLightsIntoShader(const elix::Shader &shader) const
{
    for (size_t i = 0; i < m_lights.size(); ++i)
    {
        const lighting::Light* light = m_lights[i];
        shader.setInt("lights[" + std::to_string(i) + "].type", static_cast<int>(light->type));
        shader.setVec3("lights[" + std::to_string(i) + "].position", light->position);
        shader.setVec3("lights[" + std::to_string(i) + "].color", light->color);
        shader.setFloat("lights[" + std::to_string(i) + "].strength", light->strength);
        shader.setFloat("lights[" + std::to_string(i) + "].radius", light->radius);
        shader.setVec3("lights[" + std::to_string(i) + "].direction", light->direction);
        shader.setFloat("lights[" + std::to_string(i) + "].cutoff", light->cutoff);
        shader.setFloat("lights[" + std::to_string(i) + "].outerCutoff", light->outerCutoff);
    }
}

std::vector<glm::mat4> LightManager::getLightSpaceMatrix() const
{
    return m_lightSpaceMatrix;
}

void LightManager::setLightSpaceMatrix(const std::vector<glm::mat4> &matrix)
{
    m_lightSpaceMatrix = matrix;
}

void LightManager::setLightSpaceMatricesInShader(elix::Shader &shader) const
{
    for (size_t i = 0; i < m_lightSpaceMatrix.size() && i < MAX_LIGHTS; ++i)
    {
        shader.setMat4("lightSpaceMatrices[" + std::to_string(i) + "]", m_lightSpaceMatrix[i]);
    }
}

void LightManager::bindGlobalLighting(elix::Shader &shader)
{
    shader.setInt("shadowMap", 8);
}

void LightManager::bindSpotLighting(elix::Shader &shader)
{
    const auto& spotLights = getSpotLights();

    for (size_t i = 0; i < spotLights.size(); ++i)
        shader.setInt("spotShadowMaps[" + std::to_string(i) + "]", 10 + i);
}

void LightManager::bindPointLighting(elix::Shader &shader)
{
    const auto& pointLights = getPointLights();

    for (size_t i = 0; i < pointLights.size(); ++i)
    {
        shader.setInt("pointShadowMaps[" + std::to_string(i) + "]", 15 + i);
    }
}
