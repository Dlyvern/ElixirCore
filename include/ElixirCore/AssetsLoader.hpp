#ifndef ASSETS_LOADER_HPP
#define ASSETS_LOADER_HPP

#include <memory>
#include "Assets.hpp"
#include "AssetsCache.hpp"

namespace elix
{
    class AssetsLoader
    {
    public:
        template<typename T>
        static std::unique_ptr<T> loadAsset(const std::string& filePath)
        {
            return loadAnimation(filePath);
        }

        static std::unique_ptr<elix::Asset> loadAsset(const std::string& filePath, elix::AssetsCache* cache = nullptr);

    private:
        static std::unique_ptr<AssetTexture> loadTexture(const std::string& filePath);
        static std::unique_ptr<AssetModel> loadModel(const std::string& filePath);
        static std::unique_ptr<AssetMaterial> loadMaterial(const std::string& filePath, elix::AssetsCache* cache);
        static std::unique_ptr<AssetAnimation> loadAnimation(const std::string& filePath);

    };

} //namespace elix



// void AssetsManager::saveAnimationToJson(const common::Animation& animation, const std::string& path)
// {
//     nlohmann::json animationJson;
//
//     animationJson["name"] = animation.name;
//     animationJson["duration"] = animation.duration;
//     animationJson["ticksPerSecond"] = animation.ticksPerSecond;
//
//     for (const auto&[keyFrames, objectName] : animation.boneAnimations)
//     {
//         nlohmann::json boneAnimationJson;
//
//         boneAnimationJson["targetName"] = objectName;
//
//         for (const auto&[rotation, position, scale, timeStamp] : keyFrames)
//         {
//             nlohmann::json keyFramesJson;
//
//             keyFramesJson["timeStamp"] = timeStamp;
//             keyFramesJson["position"] = {position.x, position.y, position.z};
//             keyFramesJson["rotation"] = {rotation.w, rotation.x, rotation.y, rotation.z};
//             keyFramesJson["scale"] = {scale.x, scale.y, scale.z};
//
//             boneAnimationJson["keyFrames"].push_back(keyFramesJson);
//         }
//
//         animationJson["tracks"].push_back(boneAnimationJson);
//     }
//
//     if (std::ofstream file(path); file.is_open())
//     {
//         file << std::setw(4) << animationJson << std::endl;
//         file.close();
//     }
//     else
//         LOG_ERROR( "Failed to write animation: " + path);
// }
//
// common::Animation AssetsManager::loadAnimationFromJson(const std::string &path)
// {
//     std::ifstream file(path);
//     nlohmann::json json;
//     file >> json;
//
//     common::Animation animation;
//
//
//     animation.name = json.contains("name") ? json["name"].get<std::string>() : "";
//     animation.duration = json["duration"];
//     animation.ticksPerSecond = json["ticksPerSecond"];
//
//     for (const auto& boneAnimationJson : json["tracks"])
//     {
//         std::map<float, common::SQT> tempKeyFrames;
//
//         common::AnimationTrack boneAnimation;
//         boneAnimation.objectName = boneAnimationJson["targetName"];
//
//         for (const auto& sqtJson : boneAnimationJson["keyFrames"])
//         {
//             common::SQT sqt;
//
//             sqt.timeStamp = sqtJson["timeStamp"];
//             const auto& position = sqtJson["position"];
//             sqt.position = {position[0], position[1], position[2]};
//             const auto& rotation = sqtJson["rotation"];
//
//             glm::vec3 eulerDegrees = glm::vec3(rotation[0], rotation[1], rotation[2]);
//             glm::vec3 eulerRadians = glm::radians(eulerDegrees);
//
//             sqt.rotation = glm::quat_cast(glm::eulerAngleXYZ(eulerRadians.x, eulerRadians.y, eulerRadians.z));
//             const auto& scale = sqtJson["scale"];
//             sqt.scale = {scale[0], scale[1], scale[2]};
//
//             tempKeyFrames[sqt.timeStamp] = sqt;
//
//             boneAnimation.keyFrames.push_back(sqt);
//         }
//
//         // boneAnimation.keyFrames.push_back(sqt);
//
//         // boneAnimation.keyFrames.reserve(tempKeyFrames.size());
//         //
//         // for (auto& [time, sqt] : tempKeyFrames)
//         //     boneAnimation.keyFrames.push_back(sqt);
//
//         animation.boneAnimations.push_back(boneAnimation);
//     }
//
//     return animation;
// }


#endif //ASSETS_LOADER_HPP
