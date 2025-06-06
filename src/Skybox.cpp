#include <glad/glad.h>

#include "Skybox.hpp"

#include <iostream>
#include <stb/stb_image.h>
#define STB_IMAGE_IMPLEMENTATION

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "DrawCall.hpp"
#include "FrameBuffer.hpp"
#include "MainWindow.hpp"
#include "ShaderManager.hpp"

elix::Skybox::Skybox() = default;

elix::Skybox::Skybox(const std::vector<std::string> &faces)
{
    init(faces);
}

void elix::Skybox::init(const std::vector<std::string> &faces)
{
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    const auto shader = ShaderManager::instance().getShader(ShaderManager::ShaderType::SKYBOX);
    shader->bind();
    shader->setInt("skybox", 0);

    m_vertexArray.create();

    m_vbo.create();

    m_vertexArray.bind();
    m_vbo.bind();

    m_vbo.uploadRaw(&skyboxVertices, sizeof(skyboxVertices));

    m_vertexArray.setAttribute(0, 3, elix::VertexArray::Type::Float, false, 3 * sizeof(float), (void*)0);

    m_cubeMapTextureId = loadCubemap(faces);
}

void elix::Skybox::render(const glm::mat4& view, const glm::mat4& projection) const
{
    window::MainWindow::setDepthFunc(true);

    const auto shader = ShaderManager::instance().getShader(ShaderManager::ShaderType::SKYBOX);

    shader->bind();

    shader->setMat4("view", glm::mat4(glm::mat3(view)));
    shader->setMat4("projection", projection);

    m_vertexArray.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMapTextureId);
    elix::DrawCall::drawArrays(elix::DrawCall::DrawMode::TRIANGLES, 0, 36);
    m_vertexArray.unbind();
    window::MainWindow::setDepthFunc(false);
}

void elix::Skybox::loadFromHDR(const std::string &path)
{
    m_assetPath = path;
    // elix::Texture hdrTexture;
    //
    // elix::Texture::TextureParams hdrTextureParams;
    //
    // hdrTextureParams.generateMipmaps = false;
    // hdrTextureParams.flipVertically = true;
    // hdrTextureParams.wrapS = elix::Texture::TextureWrap::ClampToEdge;
    // hdrTextureParams.wrapT = elix::Texture::TextureWrap::ClampToEdge;
    // hdrTextureParams.useFloat = true;
    // hdrTextureParams.bakingType = elix::Texture::BakingType::Float;
    // hdrTextureParams.format = elix::Texture::TextureFormat::RGB;
    //
    // hdrTexture.load(path);
    //
    // hdrTexture.bake();

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
    if (!data) {
        std::cerr << "Failed to load HDR image: " << path << std::endl;
        return;
    }

    GLuint hdrTexture;
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
    stbi_image_free(data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    elix::FrameBuffer frameBuffer;
    frameBuffer.create(CUBE_MAP_SIZE, CUBE_MAP_SIZE, elix::FrameBuffer::InternalFormat::DEPTH24);
    frameBuffer.addAttachment(elix::FrameBuffer::Attachment::DEPTH);



    glGenTextures(1, &m_cubeMapTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMapTextureId);

    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, CUBE_MAP_SIZE, CUBE_MAP_SIZE, 0, GL_RGB, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0,0,0), glm::vec3( 1, 0, 0), glm::vec3(0,-1, 0)),
        glm::lookAt(glm::vec3(0,0,0), glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)),
        glm::lookAt(glm::vec3(0,0,0), glm::vec3( 0, 1, 0), glm::vec3(0, 0, 1)),
        glm::lookAt(glm::vec3(0,0,0), glm::vec3( 0,-1, 0), glm::vec3(0, 0,-1)),
        glm::lookAt(glm::vec3(0,0,0), glm::vec3( 0, 0, 1), glm::vec3(0,-1, 0)),
        glm::lookAt(glm::vec3(0,0,0), glm::vec3( 0, 0,-1), glm::vec3(0,-1, 0)),
    };

    // hdrTexture.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);

    const auto convertShader = ShaderManager::instance().getShader(ShaderManager::ShaderType::EQUIRECTANGULAR_TO_CUBEMAP);
    convertShader->bind();
    convertShader->setInt("equirectangularMap", 0);
    convertShader->setMat4("projection", captureProjection);


    window::MainWindow::setViewport(0, 0, CUBE_MAP_SIZE, CUBE_MAP_SIZE);

    frameBuffer.bind();

    for (unsigned int i = 0; i < 6; ++i)
    {
        convertShader->setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cubeMapTextureId, 0);

        window::MainWindow::clear(window::ClearFlag::COLOR_BUFFER_BIT | window::ClearFlag::DEPTH_BUFFER_BIT);

        m_vertexArray.bind();
        elix::DrawCall::drawArrays(elix::DrawCall::DrawMode::TRIANGLES, 0, 36);
    }

    elix::FrameBuffer::unbind();

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMapTextureId);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

std::string elix::Skybox::getAssetPath() const
{
    return m_assetPath;
}

unsigned int elix::Skybox::loadCubemap(const std::vector<std::string>& faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);

        if (data)
        {
            stbi_set_flip_vertically_on_load(false);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Skybox::loadCubemap(): Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

