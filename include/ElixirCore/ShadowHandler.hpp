#ifndef SHADOW_HANDLER_HPP
#define SHADOW_HANDLER_HPP

#include <array>
#include "Texture.hpp"
#include "DepthFrameBuffer.hpp"

class ShadowHandler
{
public:
    void initAllShadows();

    void initDirectionalShadows();
    void initPointShadows();
    void initSpotShadows();

    void beginDirectionalShadowPass() const;
    void beginPointShadowPass(int index) const;
    void beginSpotShadowPass(int index) const;
    void endShadowPass() const;

    void bindDirectionalShadowPass(int textureUnit) const;
    void bindPointShadowPass(int index, int textureUnit) const;
    void bindSpotShadowPass(int index, int textureUnit) const;

    [[nodiscard]] unsigned int getSpotLightDepthMap(int index) const;
    [[nodiscard]] unsigned int getPointLightDepthMap(int index) const;

private:
    static constexpr unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    static constexpr float BORDER_COLOR[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr int MAX_SPOT_LIGHTS = 4;
    static constexpr int MAX_POINT_LIGHTS = 4;

    elix::Texture m_directionalShadowMapTexture;
    std::array<elix::Texture, MAX_SPOT_LIGHTS> m_spotShadowMapTextures;
    std::array<elix::Texture, MAX_POINT_LIGHTS> m_pointShadowMapTextures;

    elix::DepthFrameBuffer m_directionalFbo;
    std::array<elix::DepthFrameBuffer, MAX_SPOT_LIGHTS> m_spotFbos;

    unsigned int m_pointLightFBOs[MAX_POINT_LIGHTS]{};

    unsigned int m_pointLightDepthCubemaps[MAX_POINT_LIGHTS]{};
};

#endif //SHADOW_HANDLER_HPP
