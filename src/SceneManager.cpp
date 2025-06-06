#include "SceneManager.hpp"

#include <fstream>
#include <json/json.hpp>
#include "AnimatorComponent.hpp"
#include "Light.hpp"
#include "LightComponent.hpp"
#include "LightManager.hpp"
#include "Logger.hpp"
#include "MeshComponent.hpp"
#include "RigidbodyComponent.hpp"
#include "ScriptsRegister.hpp"

class LightComponent;

SceneManager& SceneManager::instance()
{
    static SceneManager instance;
    return instance;
}

void SceneManager::setCurrentScene(const std::shared_ptr<Scene>& scene)
{
    m_currentScene = scene;
}

std::shared_ptr<Scene> SceneManager::getCurrentScene() const
{
    return m_currentScene;
}

void SceneManager::updateCurrentScene(float deltaTime)
{
    if (m_currentScene)
        m_currentScene->update(deltaTime);
}

void SceneManager::saveSceneToFile(Scene* scene, const std::string &filePath)
{
    if (!scene)
        return;

    nlohmann::json json;

    json["name"] = std::filesystem::path(filePath).filename().string();

    if (scene->getSkybox())
        json["skybox"] = scene->getSkybox()->getAssetPath();

    const auto& objects = scene->getGameObjects();

    for (const auto& object : objects)
    {
        if (object->getName() == "player")
            continue;

        nlohmann::json objectJson;

        objectJson["name"] = object->getName();
        objectJson["position"] = {object->getPosition().x, object->getPosition().y, object->getPosition().z};
        objectJson["scale"] = {object->getScale().x, object->getScale().y, object->getScale().z};
        objectJson["rotation"] = {object->getRotation().x, object->getRotation().y, object->getRotation().z};

        if (object->hasComponent<MeshComponent>())
        {
            if (auto model = object->getComponent<MeshComponent>()->getModel())
            {
                objectJson["model"] = model->getName();

                nlohmann::json materialJson;

                for (int index = 0; index < model->getNumMeshes(); index++)
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
        }

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

        json["game_objects"].push_back(objectJson);
    }

    std::ofstream file(filePath);

    if (file.is_open())
    {
        file << std::setw(4) << json << std::endl;
        file.close();
    }
    else
        LOG_ERROR("SceneManager::saveObjectsIntoFile(): Could not open file to save game objects: %s", filePath);
}

std::shared_ptr<Scene> SceneManager::loadSceneFromFile(const std::string &filePath, elix::AssetsCache& cache)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        LOG_ERROR("SceneManager::loadObjectsFromFile(): Could not open file: %s", filePath);
        return nullptr;
    }

    nlohmann::json json;

    try
    {
        file >> json;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        LOG_ERROR("Failed to parse scene file %s", e.what());
        return nullptr;
    }

    auto scene = std::make_shared<Scene>();

    if (json.contains("skybox"))
    {
        auto skybox = std::make_shared<elix::Skybox>();

        skybox->init({});

        skybox->loadFromHDR(json["skybox"]);

        scene->setSkybox(skybox);
    }

    if (!json.contains("game_objects"))
        return scene;

    std::vector<std::shared_ptr<GameObject>> objects;

    for (const auto& objectJson : json["game_objects"])
    {
        const std::string& name = objectJson.value("name", "undefined");

        auto gameObject = std::make_shared<GameObject>(name);

        if (objectJson.contains("model"))
        {
            const std::string modelName = objectJson["model"];

            if (auto modelAsset = cache.getAsset<elix::AssetModel>(modelName))
            {
                auto model = modelAsset->getModel();

                gameObject->addComponent<MeshComponent>(model);

                auto& overrideMaterials = gameObject->overrideMaterials;

                if (objectJson.contains("materials"))
                {
                    const auto& materials = objectJson["materials"];

                    for (int i = 0; i < model->getNumMeshes(); ++i)
                    {
                        const std::string indexStr = std::to_string(i);

                        if (!materials.contains(indexStr))
                            continue;

                        const std::string materialName = materials.value(indexStr, "");

                        if (materialName.empty())
                        {
                            LOG_WARN("Could not find material in json with given %s", indexStr.c_str());
                            continue;
                        }

                        if (auto material = cache.getAsset<elix::AssetMaterial>(materialName))
                            overrideMaterials[i] = material->getMaterial();
                        else
                            LOG_WARN("Could not find material %s", materialName.c_str());
                    }
                }
            }
            else
                LOG_ERROR("Could not attach mesh component because missing the model %s", modelName.c_str());
        }
        else
            LOG_WARN("Could not find model in .json. Is this okay?....");

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

        // if (isSkinned)
            // physics::PhysicsController::instance().resizeCollider({1.0f, 2.0f, 1.0f}, gameObject);

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

    scene->setGameObjects(objects);

    return scene;
}
