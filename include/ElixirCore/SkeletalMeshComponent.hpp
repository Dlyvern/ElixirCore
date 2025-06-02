#ifndef SKELETAL_MESH_COMPONENT_HPP
#define SKELETAL_MESH_COMPONENT_HPP

#include "Component.hpp"
#include "SkinnedModel.hpp"

class SkeletalMeshComponent final : public Component
{
public:
    explicit SkeletalMeshComponent(common::Model* model);

    void update(float deltaTime) override {}

    void render(const elix::Shader &shader, std::unordered_map<int, Material*> *overrideMaterials = nullptr) const;

    [[nodiscard]] SkinnedModel* getModel() const;

    void setModel(SkinnedModel* model);
private:
    SkinnedModel* m_model{nullptr};
};

#endif //SKELETAL_MESH_COMPONENT_HPP
