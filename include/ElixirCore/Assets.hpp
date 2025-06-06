#ifndef ASSETS_HPP
#define ASSETS_HPP

#include "Texture.hpp"
#include "Model.hpp"
#include <memory>

namespace elix
{
    class Asset
    {
    public:
        virtual ~Asset() = default;
    };

    class AssetModel final : public Asset
    {
    public:
        explicit AssetModel(std::unique_ptr<elix::Model> model) : m_model(std::move(model)) {}
        [[nodiscard]] elix::Model* getModel() const { return m_model.get(); }
    private:
        std::unique_ptr<elix::Model> m_model{nullptr};
    };

    class AssetMaterial final : public Asset
    {
    public:
        explicit AssetMaterial(std::unique_ptr<Material> material) : m_material(std::move(material)) {}
        [[nodiscard]] Material* getMaterial() const { return m_material.get(); }
    private:
        std::unique_ptr<Material> m_material{nullptr};
    };

    class AssetTexture final : public Asset
    {
    public:
        explicit AssetTexture(std::unique_ptr<elix::Texture> texture) : m_texture(std::move(texture)) {}

        [[nodiscard]] elix::Texture* getTexture() const { return m_texture.get(); }
    private:
        std::unique_ptr<elix::Texture> m_texture{nullptr};
    };

    class AssetAnimation final : public Asset
    {
    public:
        explicit AssetAnimation(std::unique_ptr<common::Animation> animation) : m_animation(std::move(animation)) {}

        [[nodiscard]] common::Animation* getAnimation() const { return m_animation.get(); }

    private:
        std::unique_ptr<common::Animation> m_animation{nullptr};
    };

} //namespace elix

#endif //ASSETS_HPP
