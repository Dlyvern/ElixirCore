#include "CameraComponent.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

elix::CameraComponent::CameraComponent()
{
    updateCameraVectors();
}

void elix::CameraComponent::update(float deltaTime)
{
    updateCameraVectors();
}

glm::vec3 elix::CameraComponent::getPosition() const
{
    return m_position;
}

glm::vec3 elix::CameraComponent::getForward() const
{
    return m_forward;
}

glm::vec3 elix::CameraComponent::getUp() const
{
    return m_up;
}

glm::mat4 elix::CameraComponent::getViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_forward, m_up);
}

float elix::CameraComponent::getPitch() const
{
    return m_pitch;
}

float elix::CameraComponent::getYaw() const
{
    return m_yaw;
}

glm::mat4 elix::CameraComponent::getProjectionMatrix() const
{
    return glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
}

void elix::CameraComponent::setYaw(float yaw)
{
    m_yaw = yaw;
    updateCameraVectors();
}

void elix::CameraComponent::setPosition(const glm::vec3 &position)
{
    m_position = position;
}

void elix::CameraComponent::setPitch(float pitch)
{
    m_pitch = glm::clamp(pitch, -89.0f, 89.0f);
    updateCameraVectors();
}

void elix::CameraComponent::updateCameraVectors()
{
    glm::vec3 forward;
    forward.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    forward.y = sin(glm::radians(m_pitch));
    forward.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_forward = glm::normalize(forward);
    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}
