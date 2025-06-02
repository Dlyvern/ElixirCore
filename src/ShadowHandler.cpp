#include "ShadowHandler.hpp"
#include <glad/glad.h>

#include "FrameBuffer.hpp"
#include "Texture.hpp"
#include "WindowsManager.hpp"

namespace {
    elix::Texture::TextureParams getTextureParametersForShadows(int width, int height)
    {
        elix::Texture::TextureParams params;

        params.width = width;
        params.height = height;
        params.format = elix::Texture::TextureFormat::DepthComponent;
        params.magFilter = elix::Texture::TextureFilter::Nearest;
        params.minFilter = elix::Texture::TextureFilter::Nearest;
        params.wrapS = elix::Texture::TextureWrap::ClampToBorder;
        params.wrapT = elix::Texture::TextureWrap::ClampToBorder;
        params.generateMipmaps = false;
        params.bakingType = elix::Texture::BakingType::Float;

        return params;
    }
}

void ShadowHandler::initAllShadows()
{
    initDirectionalShadows();
    initPointShadows();
    initSpotShadows();
}

void ShadowHandler::initDirectionalShadows()
{
    auto params = getTextureParametersForShadows(SHADOW_WIDTH, SHADOW_HEIGHT);

    m_directionalShadowMapTexture.loadEmpty(&params);
    m_directionalShadowMapTexture.bake();

    m_directionalFbo.init(m_directionalShadowMapTexture.getId());
}

void ShadowHandler::initSpotShadows()
{
    auto params = getTextureParametersForShadows(SHADOW_WIDTH, SHADOW_HEIGHT);

    for (int i = 0; i < MAX_SPOT_LIGHTS; ++i)
    {
        m_spotShadowMapTextures[i].loadEmpty(&params);
        m_spotShadowMapTextures[i].bake();

        m_spotFbos[i].init(m_spotShadowMapTextures[i].getId());
    }
}

void ShadowHandler::initPointShadows()
{
    glGenFramebuffers(MAX_POINT_LIGHTS, m_pointLightFBOs);
    glGenTextures(MAX_POINT_LIGHTS, m_pointLightDepthCubemaps);

    auto params = getTextureParametersForShadows(SHADOW_WIDTH, SHADOW_HEIGHT);

    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_pointLightDepthCubemaps[i]);

        for (unsigned int face = 0; face < 6; ++face) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT,
                         SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, m_pointLightFBOs[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_pointLightDepthCubemaps[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowHandler::beginDirectionalShadowPass() const
{
    m_directionalFbo.bind();
    window::MainWindow::setViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    window::MainWindow::clear(window::ClearFlag::DEPTH_BUFFER_BIT);
    window::MainWindow::setCullMode(window::CullMode::FRONT);
}

void ShadowHandler::beginPointShadowPass(int index) const
{
    if (index < 0 || index >= MAX_POINT_LIGHTS) return;

    glBindFramebuffer(GL_FRAMEBUFFER, m_pointLightFBOs[index]);

    window::MainWindow::setViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    window::MainWindow::clear(window::ClearFlag::DEPTH_BUFFER_BIT);
    window::MainWindow::setCullMode(window::CullMode::FRONT);
}

void ShadowHandler::beginSpotShadowPass(int index) const
{
    if (index < 0 || index >= MAX_SPOT_LIGHTS) return;

    m_spotFbos[index].bind();
    window::MainWindow::setViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    window::MainWindow::clear(window::ClearFlag::DEPTH_BUFFER_BIT);
    window::MainWindow::setCullMode(window::CullMode::FRONT);
}

void ShadowHandler::endShadowPass() const
{
    window::MainWindow::setCullMode(window::CullMode::BACK);

    elix::FrameBuffer::unbind();

    if (const auto* win = window::WindowsManager::instance().getCurrentWindow())
        win->viewport();

    window::MainWindow::clear(window::ClearFlag::COLOR_BUFFER_BIT | window::ClearFlag::DEPTH_BUFFER_BIT);
}

void ShadowHandler::bindDirectionalShadowPass(int textureUnit) const
{
    m_directionalShadowMapTexture.bind(textureUnit);
}

void ShadowHandler::bindPointShadowPass(int index, int textureUnit) const
{
    if (index < 0 || index >= MAX_POINT_LIGHTS) return;

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_pointLightDepthCubemaps[index]);
}

void ShadowHandler::bindSpotShadowPass(int index, int textureUnit) const
{
    if (index < 0 || index >= MAX_SPOT_LIGHTS) return;

    m_spotShadowMapTextures[index].bind(textureUnit);
}

unsigned int ShadowHandler::getSpotLightDepthMap(int index) const
{
    if (index < 0 || index >= MAX_SPOT_LIGHTS) return 0;
    return m_spotShadowMapTextures[index].getId();
}

unsigned int ShadowHandler::getPointLightDepthMap(int index) const
{
    if (index < 0 || index >= MAX_POINT_LIGHTS) return 0;
    return m_pointLightDepthCubemaps[index];
}