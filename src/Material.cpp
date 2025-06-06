#include "Material.hpp"
#include <iostream>

Material::Material() = default;

Material::Material(const std::string &name) : m_name(name){}

std::string Material::getName() const
{
    return m_name;
}

void Material::setName(const std::string &name)
{
    m_name = name;
}

void Material::setBaseColor(const glm::vec3 &color)
{
    m_baseColor = color;
}

void Material::addTexture(const elix::Texture::TextureType &type, elix::Texture *texture)
{
    m_textures[type] = texture;

    if (type == elix::Texture::TextureType::Diffuse)
        m_useBaseColor = texture == nullptr;
}

elix::Texture* Material::getTexture(const elix::Texture::TextureType &type)
{
    return m_textures[type];
}

const std::unordered_map<elix::Texture::TextureType, elix::Texture *>& Material::getTextures() const
{
    return m_textures;
}

const glm::vec3& Material::getBaseColor() const
{
    return m_baseColor;
}

void Material::bind(elix::Shader &shader)
{
    int textureUnit = 0;

    auto bindTex = [&shader, &textureUnit, this](elix::Texture::TextureType type, const std::string& uniformName, const std::string& enabledFlag)
    {
        auto texture = getTexture(type);

        shader.setInt(enabledFlag, texture != nullptr);

        if (texture)
        {
            if (!texture->isBaked())
                texture->bake();

            texture->bind(textureUnit);
            shader.setInt(uniformName, textureUnit);
            textureUnit++;
        }
    };

    bindTex(elix::Texture::TextureType::Diffuse,   "u_Diffuse",   "use_Diffuse");
    bindTex(elix::Texture::TextureType::Normal,    "u_Normal",    "use_Normal");
    bindTex(elix::Texture::TextureType::Metallic,  "u_Metallic",  "use_Metallic");
    bindTex(elix::Texture::TextureType::Roughness, "u_Roughness", "use_Roughness");
    bindTex(elix::Texture::TextureType::AO,        "u_AO",        "use_AO");

    shader.setVec3("baseColor", m_baseColor);
}

std::shared_ptr<Material> Material::getDefaultMaterial()
{
    if (!m_defaultMaterial)
    {
        m_defaultMaterial = std::make_shared<Material>();
        m_defaultMaterial->setName("default_material");
        m_defaultMaterial->setBaseColor({128, 128, 128});
    }
    
    return m_defaultMaterial;
}
