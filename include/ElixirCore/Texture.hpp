#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>

namespace elix
{
    class Texture {
    public:
        struct TextureData
        {
        public:
            int width{0};
            int height{0};
            int numberOfChannels{0};
            unsigned char* data{nullptr};
            float* dataFloat{nullptr};
        };

        enum class TextureWrap
        {
            Repeat,
            ClampToEdge,
            ClampToBorder
        };

        enum class TextureFilter
        {
            Nearest,
            Linear,
            LinearMipmapLinear
        };

        enum class TextureFormat
        {
            RGB,
            RGBA,
            RED,
            SRGB,
            SRGBA,
            DepthComponent
        };

        enum class TextureType
        {
            Diffuse,
            Specular,
            Normal,
            Metallic,
            Roughness,
            AO,
            Emissive,
            Height,
            Glossiness,
            Opacity,
            Depth2D,
            Undefined
        };

        enum class TextureUsage
        {
            Standard2D,
            RenderTarget
        };

        enum class BakingType
        {
            Float,
            UnsignedByte
        };

        struct TextureParams
        {
            TextureWrap wrapS = TextureWrap::Repeat;
            TextureWrap wrapT = TextureWrap::Repeat;
            TextureWrap wrapR = TextureWrap::Repeat;
            TextureFilter minFilter = TextureFilter::Linear;
            TextureFilter magFilter = TextureFilter::Linear;
            BakingType bakingType = BakingType::UnsignedByte;
            bool generateMipmaps = true;
            TextureFormat format = TextureFormat::RGBA;
            TextureUsage usage = TextureUsage::Standard2D;
            bool flipVertically = true;
            int width{0};
            int height{0};
            float borderColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};


            bool useFloat{false};
        };

        explicit Texture(const std::string& filePath, TextureParams* params = nullptr);

        Texture();

        void loadEmpty(TextureParams* params = nullptr);

        void load(const std::string& filePath, TextureParams* params = nullptr);

        [[nodiscard]]const std::string& getName() const;

        [[nodiscard]]unsigned int getId() const;

        void bake();

        void bakeCubemap(int width, int height);

        [[nodiscard]] bool isBaked() const;

        void bind(unsigned int slot = 0) const;

        void unbind(unsigned int slot) const;

        ~Texture();
    private:
        bool m_isBaked{false};
        unsigned int m_id{0};
        TextureData m_textureData;
        std::string m_name;
        TextureParams m_parameters;
    };

} //namespace elix

#endif //TEXTURE_HPP
