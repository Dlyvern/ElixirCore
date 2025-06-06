#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <memory>

#include "Texture.hpp"
#include "Shader.hpp"
#include <unordered_map>

class Material
{
public:
    Material();

    explicit Material(const std::string& name);

    std::string getName() const;

    void setName(const std::string& name);

    void setBaseColor(const glm::vec3& color);

    void addTexture(const elix::Texture::TextureType& type, elix::Texture* texture);

    elix::Texture* getTexture(const elix::Texture::TextureType& type);
    const std::unordered_map<elix::Texture::TextureType, elix::Texture*>& getTextures() const;

    const glm::vec3& getBaseColor() const;

    void bind(elix::Shader& shader);

    static std::shared_ptr<Material> getDefaultMaterial();
private:
    static inline std::shared_ptr<Material> m_defaultMaterial{nullptr};

    std::string m_name{"Undefined"};
    std::unordered_map<elix::Texture::TextureType, elix::Texture*> m_textures;
    glm::vec3 m_baseColor = glm::vec3(128, 128, 128);
    bool m_useBaseColor = true;
};

#endif //MATERIAL_HPP
