#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP
#include "Scene.hpp"
#include "AssetsCache.hpp"

class SceneManager
{
public:
    static SceneManager& instance();

    void setCurrentScene(const std::shared_ptr<Scene>& scene);

    [[nodiscard]] std::shared_ptr<Scene> getCurrentScene() const;

    void updateCurrentScene(float deltaTime);

    static void saveSceneToFile(Scene* scene, const std::string& filePath);
    static std::shared_ptr<Scene> loadSceneFromFile(const std::string& filePath, elix::AssetsCache& cache);

    ~SceneManager() = default;
private:

    std::shared_ptr<Scene> m_currentScene{nullptr};


    SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) = delete;
    SceneManager& operator=(SceneManager&&) = delete;
};

#endif //SCENE_MANAGER_HPP
