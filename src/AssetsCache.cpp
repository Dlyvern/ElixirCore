#include "AssetsCache.hpp"

elix::Asset* elix::AssetsCache::addAsset(const std::string &path, std::unique_ptr<elix::Asset> asset)
{
    m_assets[path] = std::move(asset);
    return m_assets[path].get();
}
