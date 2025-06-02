#include "StaticMeshComponent.hpp"

StaticMeshComponent::StaticMeshComponent(common::Model *model)
{
    m_model = dynamic_cast<StaticModel*>(model);
}

void StaticMeshComponent::render(std::unordered_map<int, Material*> *overrideMaterials) const
{
    overrideMaterials ? m_model->drawWithMaterials(*overrideMaterials) : m_model->draw();
}

StaticModel* StaticMeshComponent::getModel() const
{
    return m_model;
}

void StaticMeshComponent::setModel(StaticModel *model)
{
    m_model = model;
}
