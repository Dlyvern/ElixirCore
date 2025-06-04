#ifndef SCENE_HPP
#define SCENE_HPP

#include "Drawable.hpp"
#include "GameObject.hpp"
#include "Skybox.hpp"

class Scene
{
public:
    Scene();

    virtual ~Scene();

    void update(float deltaTime);

    void setSkybox(const std::shared_ptr<elix::Skybox>& skybox);

    std::shared_ptr<elix::Skybox> getSkybox() const;

    void addGameObject(const std::shared_ptr<GameObject>& gameObject);

    void addDrawable(const std::shared_ptr<Drawable>& drawable);

    void setGameObjects(const std::vector<std::shared_ptr<GameObject>>& gameObjects);

    bool deleteGameObject(GameObject* gameObject);

    const std::vector<std::shared_ptr<GameObject>>& getGameObjects();

    const std::vector<std::shared_ptr<Drawable>>& getDrawables();

private:
    std::vector<std::shared_ptr<GameObject>> m_objects;
    std::vector<std::shared_ptr<Drawable>> m_drawables;
    std::shared_ptr<elix::Skybox> m_skybox;
};

#endif //SCENE_HPP