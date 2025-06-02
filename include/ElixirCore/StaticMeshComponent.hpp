#ifndef STATIC_MESH_COMPONENT_HPP
#define STATIC_MESH_COMPONENT_HPP

#include "Component.hpp"
#include "StaticModel.hpp"

class StaticMeshComponent final : public Component
{
public:
    explicit StaticMeshComponent(common::Model* model);

    void update(float deltaTime) override {}

    void render(std::unordered_map<int, Material*> *overrideMaterials = nullptr) const;

    [[nodiscard]] StaticModel* getModel() const;

    void setModel(StaticModel* model);
private:
    StaticModel* m_model{nullptr};
};

#endif //STATIC_MESH_COMPONENT_HPP
