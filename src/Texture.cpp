#include "Texture.hpp"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <filesystem>
#include <iostream>

namespace
{
    elix::Texture::TextureData getTextureDataFromFile(const std::string& filePath, bool flipVertically, bool useFloat = false)
    {
        stbi_set_flip_vertically_on_load(flipVertically);

        elix::Texture::TextureData textureData{};

        if (!useFloat)
        {
            textureData.data = stbi_load(filePath.data(), &textureData.width, &textureData.height, &textureData.numberOfChannels, 0);

            if(!textureData.data)
                std::cout << "getTextureDataFromFile(): Failed to load a texture" << filePath << std::endl;
        }
        else
        {
            textureData.dataFloat = stbi_loadf(filePath.data(), &textureData.width, &textureData.height, &textureData.numberOfChannels, 0);
            if(!textureData.dataFloat)
                std::cout << "getTextureDataFromFile(): Failed to load a texture" << filePath << std::endl;
        }

        return textureData;
    }

    GLenum toGL(elix::Texture::TextureWrap wrap) {
        switch (wrap) {
            case elix::Texture::TextureWrap::Repeat: return GL_REPEAT;
            case elix::Texture::TextureWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
            case elix::Texture::TextureWrap::ClampToBorder: return GL_CLAMP_TO_BORDER;
        }
        return GL_REPEAT;
    }

    GLenum toGL(elix::Texture::TextureFilter filter) {
        switch (filter) {
            case elix::Texture::TextureFilter::Nearest: return GL_NEAREST;
            case elix::Texture::TextureFilter::Linear: return GL_LINEAR;
            case elix::Texture::TextureFilter::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
        }
        return GL_LINEAR;
    }

    GLenum toGL(elix::Texture::BakingType bakingType)
    {
        switch (bakingType) {
            case elix::Texture::BakingType::Float: return GL_FLOAT;
            case elix::Texture::BakingType::UnsignedByte: return GL_UNSIGNED_BYTE;
        }

        return GL_UNSIGNED_BYTE;
    }

    GLenum toGL(elix::Texture::TextureFormat format)
    {
        switch (format) {
            case elix::Texture::TextureFormat::RGB: return GL_RGB;
            case elix::Texture::TextureFormat::RGBA: return GL_RGBA;
            case elix::Texture::TextureFormat::RED: return GL_RED;
            case elix::Texture::TextureFormat::DepthComponent: return GL_DEPTH_COMPONENT;
        }
        return GL_RGB;
    }
}

elix::Texture::Texture() = default;

void elix::Texture::loadEmpty(TextureParams *params)
{
    if (params)
        m_parameters = *params;
}

elix::Texture::Texture(const std::string &filePath, TextureParams* params)
{
    load(filePath, params);
}

void elix::Texture::load(const std::string &filePath, TextureParams* params)
{
    if (params)
        m_parameters = *params;

    m_textureData = getTextureDataFromFile(filePath, params ? params->flipVertically : false, params ? params->useFloat : false);

    if (m_textureData.numberOfChannels == 4)
        m_parameters.format = elix::Texture::TextureFormat::RGBA;
    else if (m_textureData.numberOfChannels == 3)
        m_parameters.format = elix::Texture::TextureFormat::RGB;
    else if (m_textureData.numberOfChannels == 1)
        m_parameters.format = elix::Texture::TextureFormat::RED;

    m_parameters.width = m_textureData.width;
    m_parameters.height = m_textureData.height;

    const std::filesystem::path file(filePath);
    m_name = file.filename().string();
}

const std::string& elix::Texture::getName() const
{
    return m_name;
}

unsigned int elix::Texture::getId() const
{
    return m_id;
}

void elix::Texture::bake()
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    if (m_parameters.usage == elix::Texture::TextureUsage::Standard2D)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGL(m_parameters.minFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGL(m_parameters.magFilter));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGL(m_parameters.wrapS));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGL(m_parameters.wrapT));

        if (m_parameters.wrapS == TextureWrap::ClampToBorder || m_parameters.wrapT == TextureWrap::ClampToBorder)
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, m_parameters.borderColor);
    }
    else if (m_parameters.usage == elix::Texture::TextureUsage::RenderTarget)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGL(m_parameters.minFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGL(m_parameters.magFilter));
    }

    glTexImage2D(GL_TEXTURE_2D, 0, toGL(m_parameters.format), m_parameters.width, m_parameters.height, 0, toGL(m_parameters.format), toGL(m_parameters.bakingType), m_parameters.useFloat ? (void*)m_textureData.dataFloat : (void*)m_textureData.data);

    if (m_parameters.generateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    if (m_textureData.data)
        stbi_image_free(m_textureData.data);

    if (m_textureData.dataFloat)
        stbi_image_free(m_textureData.dataFloat);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_textureData.data = nullptr;
    m_isBaked = true;
}

void elix::Texture::bakeCubemap(int width, int height)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);


    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

}

bool elix::Texture::isBaked() const
{
    return m_isBaked;
}

void elix::Texture::unbind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void elix::Texture::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

elix::Texture::~Texture()
{
    if (m_id != 0)
        glDeleteTextures(1, &m_id);
}