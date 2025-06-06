#ifndef MESH_COMPONENT_HPP
#define MESH_COMPONENT_HPP

#include "Component.hpp"
#include "Model.hpp"
#include "Material.hpp"

class MeshComponent final : public Component
{
public:
    explicit MeshComponent(elix::Model* model) : m_model(model) {}

    void render(std::unordered_map<int, Material*> *overrideMaterials = nullptr) const
    {
        overrideMaterials ? m_model->drawWithMaterials(*overrideMaterials) : m_model->draw();
    }

    void update(float deltaTime) override {}

    [[nodiscard]] elix::Model* getModel() const {return m_model;}
private:
    elix::Model* m_model{nullptr};
};

#endif //MESH_COMPONENT_HPP
