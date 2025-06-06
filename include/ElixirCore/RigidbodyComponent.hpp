#ifndef RIGID_BODY_COMPONENT_HPP
#define RIGID_BODY_COMPONENT_HPP

#include "Component.hpp"
#include "Physics.hpp"
#include <memory>

class RigidbodyComponent final : public Component
{
public:
    explicit RigidbodyComponent(const std::shared_ptr<GameObject>& object);

    void update(float deltaTime) override;

    [[nodiscard]] physx::PxRigidActor* getRigidActor() const;

    void destroy() override;
private:
    physx::PxRigidActor* m_rigidActor{nullptr};

    void onOwnerPositionChanged(const glm::vec3& position);
};

#endif //RIGID_BODY_COMPONENT_HPP
