#include "Scene.hpp"

Scene::Scene() = default;

Scene::~Scene() = default;

void Scene::update(float deltaTime)
{
    for (const auto& object : m_objects)
        object->update(deltaTime);
}

void Scene::setSkybox(const std::shared_ptr<elix::Skybox> &skybox)
{
    m_skybox = skybox;
}

std::shared_ptr<elix::Skybox> Scene::getSkybox() const
{
    return m_skybox;
}

void Scene::addGameObject(const std::shared_ptr<GameObject>& gameObject)
{
    m_objects.push_back(gameObject);
}

void Scene::addDrawable(const std::shared_ptr<Drawable> &drawable)
{
    m_drawables.push_back(drawable);
}

void Scene::setGameObjects(const std::vector<std::shared_ptr<GameObject>> &gameObjects)
{
    m_objects = gameObjects;
}

bool Scene::deleteGameObject(GameObject *gameObject)
{
    if (!gameObject)
        return false;

    const auto it = std::find_if(m_objects.begin(), m_objects.end(), [&gameObject](const std::shared_ptr<GameObject>& gO)
    {
        return gO->getName() == gameObject->getName();
    });

    if (it == m_objects.end())
        return false;

    it->get()->destroy();
    m_objects.erase(it);
    
    return true;
}

const std::vector<std::shared_ptr<GameObject>>& Scene::getGameObjects()
{
    return m_objects;
}

const std::vector<std::shared_ptr<Drawable>>& Scene::getDrawables()
{
    return m_drawables;
}
