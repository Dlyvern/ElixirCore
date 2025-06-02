#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/mat4x4.hpp>

namespace lighting
{
enum class LightType : uint8_t
{
    DIRECTIONAL = 0,
    POINT = 1,
    SPOT = 2
};

class Light
{
public:
    glm::vec3 position{1.0f};
    glm::vec3 color{1, 0.7799999713897705, 0.5289999842643738};
    float strength{0.3f};
    float radius{2.0f};
    LightType type{LightType::DIRECTIONAL};
    glm::vec3 direction;

    float cutoff = glm::cos(glm::radians(12.5f));
    float outerCutoff = glm::cos(glm::radians(17.5f));

    glm::mat4 lightSpaceMatrix;
};
} //namespace lighting

#endif //LIGHT_HPP