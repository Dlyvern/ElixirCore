#include "AssetsLoader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Mesh.hpp"
#include "stb/stb_image.h"

#include <filesystem>
#include <json/json.hpp>

#include "Logger.hpp"
#include "Utilities.hpp"

std::unique_ptr<elix::Asset> elix::AssetsLoader::loadAsset(const std::string &filePath, elix::AssetsCache* cache)
{
    if (auto texture = loadTexture(filePath))
        return texture;

    if (auto model = loadModel(filePath))
        return model;

    if (cache)
        if (auto material = loadMaterial(filePath, cache))
            return material;

    LOG_WARN("Failed to load asset from %s", filePath.c_str());

    return nullptr;
}

std::unique_ptr<elix::AssetTexture> elix::AssetsLoader::loadTexture(const std::string &filePath)
{
    int width, height, channels;

    if (stbi_load(filePath.c_str(), &width, &height, &channels, 0))
    {
        auto texture = std::make_unique<elix::Texture>(filePath);
        LOG_INFO("Loaded texture ", filePath.c_str());
        return std::make_unique<elix::AssetTexture>(std::move(texture));
    }

    return nullptr;
}


void generateBoneHierarchy(const int parentId, const aiNode* src, Skeleton* skeleton, const glm::mat4& parentTransform)
{
    assert(src);

    std::string nodeName = src->mName.C_Str();
    glm::mat4 localTransform = utilities::convertMatrixToGLMFormat(src->mTransformation);
    glm::mat4 globalTransform = parentTransform * localTransform;

    // Clean up child name to avoid duplicates due to naming conventions
    size_t underscorePos = nodeName.find_first_of('_');

    if (underscorePos != std::string::npos)
        nodeName = nodeName.substr(0, underscorePos);

    int boneID = skeleton->getBoneId(nodeName);

    if (boneID == -1)
    {
        // If the bone isn't in the skeleton, create a new one to preserve hierarchy
        boneID = skeleton->getBonesCount();

        common::BoneInfo newBone(nodeName, boneID, utilities::convertMatrixToGLMFormat(src->mTransformation), glm::mat4(1.0f));

        boneID = skeleton->addBone(newBone);
    }

    common::BoneInfo* currentBone = skeleton->getBone(boneID);

    currentBone->globalBindTransform = globalTransform;

    if (auto parent = skeleton->getBone(parentId); parent && parent->id != boneID)
    {
        currentBone->parentId = parent->id;
        parent->children.push_back(boneID);
        parent->childrenInfo.push_back(currentBone);
    }

    for (unsigned int i = 0; i < src->mNumChildren; i++)
        generateBoneHierarchy(currentBone ? currentBone->id : -1, src->mChildren[i], skeleton, globalTransform);
}

void assignLocalBindTransforms(aiNode* node, Skeleton* skeleton)
{
    if (auto bone = skeleton->getBone(node->mName.C_Str()))
        bone->localBindTransform = utilities::convertMatrixToGLMFormat(node->mTransformation);

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        assignLocalBindTransforms(node->mChildren[i], skeleton);
}

elix::Mesh processMesh(aiMesh* mesh, const aiScene* const scene, Skeleton* skeleton)
{
    std::vector<common::Vertex> vertices;
    std::vector<unsigned int> indices;

    for(unsigned int j = 0; j < mesh->mNumVertices; j++)
    {
        common::Vertex vertex;

        for (int i = 0; i < 4; i++)
        {
            vertex.boneID[i] = -1;
            vertex.weight[i] = 0.0f;
        }

        vertex.position = {mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};
        vertex.normal = {mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z};

        if(mesh->HasTangentsAndBitangents())
        {
            vertex.tangent = {mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z};
            vertex.bitangent = {mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z};
        }

        if(mesh->HasTextureCoords(0))
            vertex.textureCoordinates = {mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y};

        vertices.push_back(vertex);
    }

    for(unsigned int k = 0; k < mesh->mNumFaces; k++)
    {
        aiFace face = mesh->mFaces[k];

        for(unsigned int s = 0; s < face.mNumIndices; s++)
            indices.push_back(face.mIndices[s]);
    }

    bool hasBones = false;

    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
    {
        hasBones = true;

        aiBone* bone = mesh->mBones[boneIndex];

        skeleton->addBone(bone);

        std::string boneName = bone->mName.C_Str();
        const unsigned int boneId = skeleton->getBoneId(boneName);

        for (unsigned int bonesWeightIndex = 0; bonesWeightIndex < bone->mNumWeights; bonesWeightIndex++)
        {
            const auto boneWeight = bone->mWeights[bonesWeightIndex];

            common::Vertex& vertexBoneData = vertices[boneWeight.mVertexId];

            for(int f = 0; f < 4; ++f)
            {
                if(vertexBoneData.boneID[f] < 0)
                {
                    vertexBoneData.weight[f] = boneWeight.mWeight;
                    vertexBoneData.boneID[f] = boneId;
                    break;
                }
            }
        }
    }

    if (hasBones)
    {
        skeleton->globalInverseTransform = glm::inverse(utilities::convertMatrixToGLMFormat(scene->mRootNode->mTransformation));
        generateBoneHierarchy(-1, scene->mRootNode, skeleton, glm::mat4(1.0f));
        assignLocalBindTransforms(scene->mRootNode, skeleton);
    }

    return {vertices, indices};
}

void processMeshes(const aiNode* const node, const aiScene* const scene, std::vector<elix::Mesh>& meshes, Skeleton* skeleton)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(processMesh(mesh, scene, skeleton));
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
        processMeshes(node->mChildren[i], scene, meshes, skeleton);
}

std::unique_ptr<elix::AssetModel> elix::AssetsLoader::loadModel(const std::string &filePath)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        return nullptr;

    std::vector<elix::Mesh> meshes;
    auto skeleton = std::make_unique<Skeleton>();

    const std::string name = std::filesystem::path(filePath).filename().string();

    processMeshes(scene->mRootNode, scene, meshes, skeleton.get());

    std::unique_ptr<elix::Model> model{nullptr};

    //TODO rename with actual path to the model
    const std::string modelName = std::filesystem::path(filePath).filename().string();

    if (skeleton->getBonesCount() <= 0)
    {
        LOG_INFO("No bones has been loaded");
        model = std::make_unique<elix::Model>(modelName, std::move(meshes));
    }
    else
    {
        LOG_INFO("Loaded boneHierarchy %d", skeleton->getBonesCount());
        model = std::make_unique<elix::Model>(modelName, std::move(meshes), std::move(skeleton));
    }

    LOG_INFO("Loaded model ", filePath.c_str());

    return std::make_unique<elix::AssetModel>(std::move(model));
}

std::unique_ptr<elix::AssetMaterial> elix::AssetsLoader::loadMaterial(const std::string &filePath, elix::AssetsCache* cache)
{
    std::ifstream file(filePath);
    nlohmann::json json;

    try
    {
        file >> json;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        return nullptr;
    }

    auto material = std::make_unique<Material>();

    if (json.contains("name"))
        material->setName(json["name"].get<std::string>());

    if (json.contains("color"))
    {
        auto color = json["color"];
        material->setBaseColor(glm::vec3(color[0].get<int>(), color[1].get<int>(), color[2].get<int>()));
    }

    if (json.contains("textures"))
        for (const auto& [key, value] : json["textures"].items())
        {
            if (!value.is_string() || value.get<std::string>().empty())
                continue;

            if (const auto textureType = utilities::fromStringToTextureType(key); textureType != elix::Texture::TextureType::Undefined)
                if (auto asset = cache->getAsset<elix::AssetTexture>(value.get<std::string>()))
                    material->addTexture(textureType, asset->getTexture());
        }


    LOG_INFO("Loaded material ", filePath.c_str());

    return std::make_unique<elix::AssetMaterial>(std::move(material));
}

std::unique_ptr<elix::AssetAnimation> elix::AssetsLoader::loadAnimation(const std::string &filePath)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        return nullptr;

    if (scene->mNumAnimations <= 0)
        return nullptr;

     aiAnimation* anim = scene->mAnimations[0];

     auto animation = std::make_unique<common::Animation>();

     animation->name = anim->mName.C_Str();
     animation->duration = anim->mDuration;
     animation->ticksPerSecond = anim->mTicksPerSecond;
     std::vector<common::AnimationTrack> boneAnimations;

     for(unsigned int animChannelIndex = 0; animChannelIndex < anim->mNumChannels; ++animChannelIndex)
     {
         aiNodeAnim* channel = anim->mChannels[animChannelIndex];
         common::AnimationTrack boneAnimation;

         boneAnimation.objectName = channel->mNodeName.C_Str();

         std::map<float, common::SQT> tempKeyFrames;

         for (unsigned int j = 0; j < channel->mNumPositionKeys; j++)
         {
             float time = channel->mPositionKeys[j].mTime;
             tempKeyFrames[time].position = glm::vec3(channel->mPositionKeys[j].mValue.x,
                                                      channel->mPositionKeys[j].mValue.y,
                                                      channel->mPositionKeys[j].mValue.z);
             tempKeyFrames[time].timeStamp = time;
         }

         for (unsigned int j = 0; j < channel->mNumRotationKeys; j++)
         {
             float time = channel->mRotationKeys[j].mTime;
             tempKeyFrames[time].rotation = glm::quat(channel->mRotationKeys[j].mValue.w,
                                                      channel->mRotationKeys[j].mValue.x,
                                                      channel->mRotationKeys[j].mValue.y,
                                                      channel->mRotationKeys[j].mValue.z);
             tempKeyFrames[time].timeStamp = time;
         }

         for (unsigned int j = 0; j < channel->mNumScalingKeys; j++)
         {
             float time = channel->mScalingKeys[j].mTime;
             tempKeyFrames[time].scale = glm::vec3(channel->mScalingKeys[j].mValue.x,
                                                   channel->mScalingKeys[j].mValue.y,
                                                   channel->mScalingKeys[j].mValue.z);
             tempKeyFrames[time].timeStamp = time;
         }

         boneAnimation.keyFrames.reserve(tempKeyFrames.size());

         for (auto& [time, sqt] : tempKeyFrames)
             boneAnimation.keyFrames.push_back(sqt);

         boneAnimations.push_back(boneAnimation);
     }

     animation->boneAnimations = (boneAnimations);

    LOG_INFO("Loaded animation ", filePath.c_str());

    return std::make_unique<elix::AssetAnimation>(std::move(animation));
}
