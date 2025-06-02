#include "MainWindow.hpp"
#include <stdexcept>

window::MainWindow::MainWindow(const std::string& windowTitle, const WindowData& windowData) : m_windowName(windowTitle), m_currentWindowData(windowData)
{
    m_window = glfwCreateWindow(m_currentWindowData.width, m_currentWindowData.height, m_windowName.c_str(), nullptr, nullptr);
    
    if (!m_window)
    {
        glfwTerminate();
        throw std::runtime_error("MainWindow::MainWindow(): could not create a window");
    }

    glfwMakeContextCurrent(m_window);

    viewport();
}

bool window::MainWindow::isWindowOpened() const
{
    return !(glfwWindowShouldClose(m_window));
}

int window::MainWindow::getHeight() const
{
    return m_currentWindowData.height;
}

int window::MainWindow::getWidth() const
{
    return m_currentWindowData.width;
}

window::WindowData window::MainWindow::getWindowData() const
{
    return m_currentWindowData;
}

GLFWwindow *window::MainWindow::getOpenGLWindow() const
{
    return m_window;
}

void window::MainWindow::pollEvents()
{
    glfwPollEvents();
}

float window::MainWindow::getTime()
{
    return glfwGetTime();
}

void window::MainWindow::setViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

void window::MainWindow::clear(ClearFlag flags)
{
    GLbitfield mask = 0;
    if (flags & ClearFlag::COLOR_BUFFER_BIT)   mask |= GL_COLOR_BUFFER_BIT;
    if (flags & ClearFlag::DEPTH_BUFFER_BIT)   mask |= GL_DEPTH_BUFFER_BIT;
    if (flags & ClearFlag::STENCIL_BUFFER_BIT) mask |= GL_STENCIL_BUFFER_BIT;

    glClear(mask);
}

void window::MainWindow::setCullMode(CullMode mode)
{
    glCullFace(mode == CullMode::BACK ? GL_BACK : GL_FRONT);
}

void window::MainWindow::setDepthFunc(bool enabled)
{
    glDepthFunc(enabled ? GL_LEQUAL : GL_LESS);
}

void window::MainWindow::lineWidth(float lineWidth)
{
    glLineWidth(lineWidth);
}

void window::MainWindow::viewport() const
{
    glViewport(0, 0, m_currentWindowData.width, m_currentWindowData.height);
}

void window::MainWindow::setSize(int width, int height)
{
    m_currentWindowData.width = width;
    m_currentWindowData.height = height;

    glfwSetWindowSize(m_window, width, height);
}
