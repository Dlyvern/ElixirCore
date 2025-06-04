#include "SceneManager.hpp"

#include <fstream>
#include <json/json.hpp>
#include "AnimatorComponent.hpp"
#include "AssetsManager.hpp"
#include "Light.hpp"
#include "LightComponent.hpp"
#include "LightManager.hpp"
#include "Logger.hpp"
#include "RigidbodyComponent.hpp"
#include "ScriptsRegister.hpp"
#include "SkeletalMeshComponent.hpp"
#include "StaticMeshComponent.hpp"


class LightComponent;

SceneManager& SceneManager::instance()
{
    static SceneManager instance;
    return instance;
}

void SceneManager::setCurrentScene(Scene *scene)
{
    m_currentScene = scene;
}

Scene* SceneManager::getCurrentScene() const
{
    return m_currentScene;
}

void SceneManager::updateCurrentScene(float deltaTime)
{
    if (m_currentScene)
        m_currentScene->update(deltaTime);
}

void SceneManager::saveObjectsIntoFile(const std::vector<std::shared_ptr<GameObject>> &objects,const std::string &filePath)
{
    nlohmann::json json;

    for (const auto& object : objects)
    {
        if (object->getName() == "player")
            continue;

        nlohmann::json objectJson;

        objectJson["name"] = object->getName();

        if (object->hasComponent<LightComponent>())
        {
            nlohmann::json lightJson;

            auto* light = object->getComponent<LightComponent>()->getLight();

            lightJson["type"] = "LightComponent";
            lightJson["lightType"] = static_cast<int>(light->type);
            lightJson["direction"] = {light->direction.x, light->direction.y, light->direction.z};
            lightJson["color"] = {light->color.r, light->color.g, light->color.b};
            lightJson["strength"] = light->strength;
            lightJson["position"] = {light->position.x, light->position.y, light->position.z};
            lightJson["radius"] = light->radius;

            objectJson["components"].push_back(lightJson);
        }
        if (object->hasComponent<AnimatorComponent>())
        {
            nlohmann::json animatorJson;

            animatorJson["type"] = "AnimatorComponent";

            objectJson["components"].push_back(animatorJson);
        }
        if (object->hasComponent<ScriptComponent>())
        {
            nlohmann::json scriptArray = nlohmann::json::array();

            for (const auto& [name, _] : object->getComponent<ScriptComponent>()->getScripts()) {
                scriptArray.push_back(name);
            }

            nlohmann::json scriptJson;
            scriptJson["type"] = "ScriptsComponent";
            scriptJson["scripts"] = scriptArray;

            objectJson["components"].push_back(scriptJson);
        }

        if (object->hasComponent<StaticMeshComponent>())
        {
            auto model = object->getComponent<StaticMeshComponent>()->getModel();
            objectJson["model"] = model ? model->getName() : "";

            nlohmann::json materialJson;

            for (int index = 0; index < model->getMeshesSize(); index++)
            {
                auto mesh = model->getMesh(index);

                Material* material;

                if (object->overrideMaterials.contains(index))
                    material = object->overrideMaterials[index];
                else
                    material = mesh->getMaterial();

                if (material)
                {
                    materialJson[std::to_string(index)] = material->getName();
                }
            }

            objectJson["materials"] = materialJson;
        }
        else if (object->hasComponent<SkeletalMeshComponent>())
        {
            auto model = object->getComponent<SkeletalMeshComponent>()->getModel();
            objectJson["skinnedModel"] = model ? model->getName() : "";

            nlohmann::json materialJson;

            for (int index = 0; index < model->getMeshesSize(); index++)
            {
                auto mesh = model->getMesh(index);

                Material* material;

                if (object->overrideMaterials.contains(index))
                    material = object->overrideMaterials[index];
                else
                    material = mesh->getMaterial();

                if (material)
                {
                    materialJson[std::to_string(index)] = material->getName();
                }
            }

            objectJson["materials"] = materialJson;

        }

        objectJson["position"] = {object->getPosition().x, object->getPosition().y, object->getPosition().z};
        objectJson["scale"] = {object->getScale().x, object->getScale().y, object->getScale().z};
        objectJson["rotation"] = {object->getRotation().x, object->getRotation().y, object->getRotation().z};

        json["walls"].push_back(objectJson);
    }

    std::ofstream file(filePath);

    if (file.is_open())
    {
        file << std::setw(4) << json << std::endl;
        file.close();
    }
    else
        std::cout << "SceneManager::saveObjectsIntoFile(): Could not open file to save game objects: " << filePath << std::endl;
}

std::vector<std::shared_ptr<GameObject>> SceneManager::loadObjectsFromFile(const std::string& filePath)
{
    std::vector<std::shared_ptr<GameObject>> objects;

    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cerr << "SceneManager::loadObjectsFromFile(): Could not open file: " << filePath << std::endl;
        return objects;
    }

    nlohmann::json json;
    file >> json;

    if (!json.contains("walls"))
        return objects;

    for (const auto& objectJson : json["walls"])
    {
        const std::string& name = objectJson.contains("name") ? objectJson["name"] : "";

        const bool isSkinned = objectJson.contains("skinnedModel");

        const std::string modelName = objectJson[isSkinned ? "skinnedModel" : "model"];

        auto gameObject = std::make_shared<GameObject>(name);

        if (common::Model* model = AssetsManager::instance().getModelByName(modelName))
        {
            if (isSkinned)
                gameObject->addComponent<SkeletalMeshComponent>(model);
            else
                gameObject->addComponent<StaticMeshComponent>(model);

            auto& overrideMaterials = gameObject->overrideMaterials;

            if (objectJson.contains("materials"))
            {
                const auto& materials = objectJson["materials"];

                for (int i = 0; i < model->getMeshesSize(); ++i)
                {
                    const std::string indexStr = std::to_string(i);

                    if (materials.contains(indexStr))
                    {
                        const std::string materialName = materials[indexStr];

                        if (!materialName.empty())
                        {
                            if (auto material = AssetsManager::instance().getMaterialByName(materialName))
                                overrideMaterials[i] = material;
                            else
                                LOG_WARN("Could not find material " + materialName);
                        }
                    }
                }
            }
        }
        else
            LOG_WARN("Could not find model with name " + modelName);

        if (objectJson.contains("position"))
        {
            const auto& pos = objectJson["position"];
            gameObject->setPosition({ pos[0], pos[1], pos[2] });
        }

        if (objectJson.contains("scale"))
        {
            const auto& scale = objectJson["scale"];
            gameObject->setScale({ scale[0], scale[1], scale[2] });
        }

        if (objectJson.contains("rotation"))
        {
            const auto& rot = objectJson["rotation"];
            gameObject->setRotation({ rot[0], rot[1], rot[2] });
        }

        gameObject->addComponent<RigidbodyComponent>(gameObject);

        if (isSkinned)
            physics::PhysicsController::instance().resizeCollider({1.0f, 2.0f, 1.0f}, gameObject);

        if (objectJson.contains("components"))
        {
            for (const auto& componentJson : objectJson["components"])
            {
                if (!componentJson.contains("type"))
                    continue;

                //TODO make it more safe, Cause it sucks...
                if (componentJson["type"] == "LightComponent")
                {
                    lighting::Light light;
                    light.type = static_cast<lighting::LightType>(componentJson["lightType"]);
                    const auto& direction = componentJson["direction"];
                    light.direction = glm::vec3(direction[0], direction[1], direction[2]);
                    const auto& position = componentJson["position"];
                    light.position = glm::vec3(position[0], position[1], position[2]);
                    const auto& color = componentJson["color"];
                    light.color = glm::vec3(color[0], color[1], color[2]);
                    light.strength = componentJson["strength"];
                    light.radius = componentJson["radius"];
                    gameObject->addComponent<LightComponent>(light);

                    LightManager::instance().addLight(gameObject->getComponent<LightComponent>()->getLight());
                }
                else if (componentJson["type"] == "AnimatorComponent")
                {
                    gameObject->addComponent<AnimatorComponent>();
                }
                else if (componentJson["type"] == "ScriptComponent")
                {
                    auto* scriptComponent = gameObject->addComponent<ScriptComponent>();

                    if (componentJson.contains("scripts"))
                    {
                        for (auto& scriptEntry : componentJson["scripts"])
                            scriptComponent->addScript(scriptEntry.get<std::string>());
                    }
                }
            }
        }

        objects.push_back(gameObject);
    }

    file.close();
    return objects;
}