#include "Application.hpp"

#include <iostream>
#include <stdexcept>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "WindowsManager.hpp"
#include "Logger.hpp"
#include <csignal>
#include <cstdlib>
#include <execinfo.h>

#ifdef _DEBUG
void GLAPIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
                              GLsizei length, const GLchar* message, const void* userParam)
{
    std::string msg = std::string("[GL CALLBACK] ") + message;

    elix::Logger::LogLevel level = elix::Logger::LogLevel::INFO;
    if (severity == GL_DEBUG_SEVERITY_HIGH)      level = elix::Logger::LogLevel::ERROR;
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM) level = elix::Logger::LogLevel::WARNING;
    else if (severity == GL_DEBUG_SEVERITY_LOW)    level = elix::Logger::LogLevel::INFO;
    else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;


    LOG_WARN(level, msg);
}

#endif

void signalHandler(int signal)
{
    void* array[25];
    size_t size = backtrace(array, 25);
    char** strings = backtrace_symbols(array, size);

    LOG_ERROR("Signal %d", signal);

    for (size_t i = 0; i < size; i++)
        LOG_ERROR(strings[i]);

    free(strings);

    elix::Application::instance().shutdown();

    std::exit(signal);
}

void terminateHandler()
{
    LOG_ERROR("Unhandled exception. Terminating...");

    elix::Application::instance().shutdown();

    std::abort();
}

void unexpectedHandler()
{
    LOG_ERROR("Unexpected exception thrown. Terminating...");

    elix::Application::instance().shutdown();

    std::abort();
}

elix::Application & elix::Application::instance()
{
    static Application instance;
    return instance;
}

void elix::Application::init()
{
    if (!glfwInit())
        throw std::runtime_error("Application::init(): Failed to initialize glfw");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window::WindowsManager::instance().setCurrentWindow(window::WindowsManager::instance().createWindow());

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw std::runtime_error("Application::init(): Failed to initialize GLAD");

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version  = glGetString(GL_VERSION);

    LOG_INFO(reinterpret_cast<const char *>(renderer));
    LOG_INFO(reinterpret_cast<const char *>(version));

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    const std::string ver = std::to_string(major) + "." + std::to_string(minor);

    LOG_INFO("Context version: ", ver.c_str());

#ifdef __APPLE__
    if (major < 4 || (major == 4 && minor < 1))
        throw std::runtime_error("OpenGL 4.6+ required");

#else
    if (major < 4 || (major == 4 && minor < 6))
        throw std::runtime_error("OpenGL 4.6+ required");
#endif

    auto mainWindow = window::WindowsManager::instance().getCurrentWindow();
    glfwSetKeyCallback(mainWindow->getOpenGLWindow(), input::KeysManager::keyCallback);
    glfwSetMouseButtonCallback(mainWindow->getOpenGLWindow(), input::MouseManager::mouseButtonCallback);
    glfwSetCursorPosCallback(mainWindow->getOpenGLWindow(), input::MouseManager::mouseCallback);
    glfwSetScrollCallback(mainWindow->getOpenGLWindow(), input::MouseManager::scrollCallback);
    glfwSetInputMode(mainWindow->getOpenGLWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL); //GLFW_CURSOR_NORMAL | GLFW_CURSOR_DISABLED

#ifdef _WIN32
    SetProcessDPIAware();
#endif

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
                              GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif

     glEnable(GL_DEPTH_TEST);
     glEnable(GL_CULL_FACE);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     glDepthFunc(GL_LESS);
     glEnable(GL_STENCIL_TEST);
     glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
     glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

     std::signal(SIGSEGV, signalHandler);
     std::signal(SIGABRT, signalHandler);
     std::signal(SIGFPE,  signalHandler);
     std::signal(SIGTERM, signalHandler);
     std::signal(SIGKILL, signalHandler);

     std::set_terminate(terminateHandler);
     std::set_unexpected(unexpectedHandler);
}

void elix::Application::shutdown()
{
    glfwTerminate();
}
