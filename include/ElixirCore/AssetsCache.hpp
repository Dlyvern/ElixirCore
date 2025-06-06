#ifndef ASSETS_CACHE_HPP
#define ASSETS_CACHE_HPP

#include "Assets.hpp"
#include <filesystem>
#include "Logger.hpp"

namespace elix
{
    class AssetsCache
    {
    public:
        elix::Asset* addAsset(const std::string& path, std::unique_ptr<elix::Asset> asset);

        template<typename T>
        std::vector<T*> getAllAssets()
        {
            std::vector<T*> result;

            for (const auto& [_, asset] : m_assets)
                if (auto dynamicAsset = dynamic_cast<T*>(asset.get()))
                    result.push_back(dynamicAsset);

            return result;
        }

        template<typename T>
        T* getAsset(const std::string& path)
        {
            if (const auto it = m_assets.find(path); it != m_assets.end())
                return dynamic_cast<T*>(it->second.get());

            for (const auto& [pathToAsset, asset] : m_assets)
                if (std::string justAName = std::filesystem::path(pathToAsset).filename().string(); justAName == path)
                    return dynamic_cast<T*>(asset.get());

            return nullptr;
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<elix::Asset>> m_assets;
    };
} //namespace elix
#endif //ASSETS_CACHE_HPP
