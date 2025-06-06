#ifndef COMMON_HPP
#define COMMON_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

#include "Material.hpp"
class Skeleton;
class GameObject;

namespace common
{
    //Maybe 32 is way too big
    enum LayerMask : uint32_t
    {
        DEFAULT = 1 << 0,
        GROUND = 1 << 1,
        PLAYER = 1 << 2,
        INTERACTABLE = 1 << 3,
    };

    struct Vertex
    {   
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textureCoordinates{0.0f, 0.0f};
        glm::vec3 tangent{0.0f, 0.0f, 0.0f};
        glm::vec3 bitangent{0.0f, 0.0f, 0.0f};
        glm::ivec4 boneID = glm::ivec4(-1);
        glm::vec4 weight = glm::vec4(0);
    };

    struct BoneInfo
    {
        std::string name;
        int id;
        glm::mat4 offsetMatrix;
        glm::mat4 finalTransformation;
        glm::mat4 localBindTransform{1.0f};
        glm::mat4 globalBindTransform{1.0f};
        std::vector<int> children;
        std::vector<BoneInfo*> childrenInfo;
        int parentId{-1};
    };

    struct SQT
    {
        glm::quat rotation{1, 0, 0, 0};
        glm::vec3 position{0, 0, 0};
        glm::vec3 scale{0, 0, 0};
        float timeStamp{0.0f};
    };

    struct AnimationTrack
    {
        std::vector<SQT> keyFrames;
        std::string objectName;
    };

    struct Animation
    {
        std::string name;
        double ticksPerSecond;
        double duration;
        std::vector<AnimationTrack> boneAnimations;
        Skeleton* skeletonForAnimation{nullptr};
        GameObject* gameObject{nullptr};

        common::AnimationTrack* getAnimationTrack(const std::string& name)
        {
            const auto it = std::find_if(boneAnimations.begin(), boneAnimations.end(), [&name](const auto& bone) {return bone.objectName == name;});
            return it == boneAnimations.end() ? nullptr : &(*it);
        }
    };
} //namespace common


#endif //COMMON_HPP