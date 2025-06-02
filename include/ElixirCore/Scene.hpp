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

    virtual void update(float deltaTime) = 0;

    virtual void create() = 0;

    virtual bool isOver() = 0;

    void setOnSceneEndCallback(const std::function<void(Scene* scene)>& callback);

    void setSkybox(const std::shared_ptr<elix::Skybox>& skybox);

    std::shared_ptr<elix::Skybox> getSkybox() const;

    void addGameObject(const std::shared_ptr<GameObject>& gameObject);

    void addDrawable(const std::shared_ptr<Drawable>& drawable);

    void setGameObjects(const std::vector<std::shared_ptr<GameObject>>& gameObjects);

    bool deleteGameObject(GameObject* gameObject);

    const std::vector<std::shared_ptr<GameObject>>& getGameObjects();

    const std::vector<std::shared_ptr<Drawable>>& getDrawables();

protected:
    std::function<void(Scene* scene)> onEndSceneCallback_;

private:
    std::vector<std::shared_ptr<GameObject>> m_objects;
    std::vector<std::shared_ptr<Drawable>> m_drawables;
    std::shared_ptr<elix::Skybox> m_skybox;
};

#endif //SCENE_HPP