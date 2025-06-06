#include "Mouse.hpp"

#include "DebugTextHolder.hpp"

input::MouseManager::MouseManager()
{
}

input::MouseManager &input::MouseManager::instance()
{
    static MouseManager mouseManager;
    return mouseManager;
}

void input::MouseManager::mouseCallback(GLFWwindow *window, double x, double y)
{
    Mouse.window = window;
    glfwGetCursorPos(window, &input::Mouse.m_x, &input::Mouse.m_y);
}

std::string mouseToString(input::MouseButton button)
{
    switch (button)
    {
        case input::MouseButton::LEFT:
            return "Left";
        case input::MouseButton::RIGHT:
            return "Right";
        case input::MouseButton::NONE:
            return "None";
    }
    return {""};
}

void input::MouseManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            Mouse.m_leftButtonPressed = true;
        } else if (action == GLFW_RELEASE) {
            Mouse.m_leftButtonPressed = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            Mouse.m_prevHeldButton = Mouse.m_heldButton;
            Mouse.m_rightButtonPressed = true;
            Mouse.m_heldButton = MouseButton::RIGHT;
        }
        else if (action == GLFW_RELEASE)
        {
            Mouse.m_prevHeldButton = Mouse.m_heldButton;
            Mouse.m_heldButton = MouseButton::NONE;
            Mouse.m_rightButtonPressed = false;
        }
    }
}

void input::MouseManager::scrollCallback(GLFWwindow *window, double x, double y)
{

}

double input::MouseManager::getX() const
{
    return m_x;
}

double input::MouseManager::getY() const
{
    return m_y;
}

bool input::MouseManager::isLeftButtonPressed() const
{
    return m_leftButtonPressed;
}

bool input::MouseManager::isRightButtonPressed() const
{
    return m_rightButtonPressed;
}

bool input::MouseManager::isLeftButtonReleased()
{
    return !m_leftButtonPressed;
}

bool input::MouseManager::isRightButtonReleased()
{
    return m_heldButton == MouseButton::NONE && m_prevHeldButton == MouseButton::RIGHT;
}
