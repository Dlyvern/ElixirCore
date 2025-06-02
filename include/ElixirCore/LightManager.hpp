#ifndef LIGHT_MANAGER_HPP
#define LIGHT_MANAGER_HPP

#include "Light.hpp"
#include "Shader.hpp"
#include <vector>

class LightManager
{
public:
    static LightManager& instance();

    void addLight(lighting::Light* light);
    void removeLight(lighting::Light* light);

    void sendLightsIntoShader(const elix::Shader& shader) const;

    [[nodiscard]] std::vector<glm::mat4> getLightSpaceMatrix() const;

    void setLightSpaceMatrix(const std::vector<glm::mat4>& matrix);

    void setLightSpaceMatricesInShader(elix::Shader& shader) const;

    void bindGlobalLighting(elix::Shader& shader);
    void bindSpotLighting(elix::Shader& shader);
    void bindPointLighting(elix::Shader& shader);



    [[nodiscard]] std::vector<lighting::Light*> getLights() const;
    [[nodiscard]] lighting::Light* getDirectionalLight() const;
    [[nodiscard]] std::vector<lighting::Light*> getSpotLights() const;
    [[nodiscard]] std::vector<lighting::Light*> getPointLights() const;
private:
    static constexpr int MAX_LIGHTS = 4;

    std::vector<lighting::Light*> m_lights;

    std::vector<glm::mat4> m_lightSpaceMatrix;

    LightManager() = default;
    LightManager(const LightManager&) = delete;
    LightManager& operator=(const LightManager&) = delete;
};

#endif //LIGHT_MANAGER_HPP
