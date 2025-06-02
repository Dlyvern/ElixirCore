#ifndef CAMERA_COMPONENT_HPP
#define CAMERA_COMPONENT_HPP

#include "Component.hpp"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace elix
{
    class CameraComponent final  : public Component
    {
    public:
        CameraComponent();

        void update(float deltaTime) override;

        [[nodiscard]] glm::vec3 getPosition() const;
        [[nodiscard]] glm::vec3 getForward() const;
        [[nodiscard]] glm::vec3 getUp() const;
        [[nodiscard]] glm::mat4 getViewMatrix() const;
        [[nodiscard]] float getPitch() const;
        [[nodiscard]] float getYaw() const;
        [[nodiscard]] glm::mat4 getProjectionMatrix() const;

        void setYaw(float yaw);
        void setPitch(float pitch);
        void setPosition(const glm::vec3& position);

    private:
        void updateCameraVectors();

        glm::vec3 m_position{0.0f};
        glm::vec3 m_up{glm::vec3(0.0f, 1.0f, 0.0f)};
        glm::vec3 m_right{glm::vec3(0.0f, 1.0f, 0.0f)};
        glm::vec3 m_forward{glm::vec3(0.0f, 0.0f, -1.0f)};

        float m_yaw{-90.0f};
        float m_pitch{0.0f};

        float m_fov{60.0f};
        float m_aspect{1.77f};
        float m_near{0.1f};
        float m_far{1000.0f};
    };
} //namespace elix

#endif //CAMERA_COMPONENT_HPP
