#include "SkeletalMeshComponent.hpp"

SkeletalMeshComponent::SkeletalMeshComponent(common::Model* model)
{
    auto skinnedModel = dynamic_cast<SkinnedModel*>(model);

    m_model = skinnedModel;

    m_model->getSkeleton().calculateBindPoseTransforms();
}

void SkeletalMeshComponent::render(const elix::Shader &shader, std::unordered_map<int, Material*> *overrideMaterials) const
{
    const std::vector<glm::mat4>& boneMatrices = m_model->getSkeleton().getFinalMatrices();

    for (int i = 0; i < boneMatrices.size(); ++i)
        shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", boneMatrices[i]);

    overrideMaterials ? m_model->drawWithMaterials(*overrideMaterials) : m_model->draw();
}

SkinnedModel* SkeletalMeshComponent::getModel() const
{
    return m_model;
}

void SkeletalMeshComponent::setModel(SkinnedModel *model)
{
    m_model = model;
    m_model->getSkeleton().calculateBindPoseTransforms();
}
