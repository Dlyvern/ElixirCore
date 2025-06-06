#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <GLFW/glfw3.h>
#include <string>

namespace window
{
    enum class WindowMode : uint8_t
    {
        FULLSCREEN = 0,
        WINDOWED = 1
    };

    struct WindowData
    {
    public:
        int width{0};
        int height{0};
        WindowMode windowMode;
    };

    enum class ClearFlag : uint8_t
    {
        COLOR_BUFFER_BIT = 1 << 0,
        DEPTH_BUFFER_BIT = 1 << 1,
        STENCIL_BUFFER_BIT = 1 << 2
    };

    enum class CullMode : uint8_t
    {
        FRONT = 0,
        BACK = 1,
    };

    inline ClearFlag operator|(ClearFlag a, ClearFlag b) {
        return static_cast<ClearFlag>(
            static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    inline bool operator&(ClearFlag a, ClearFlag b) {
        return static_cast<uint8_t>(a) & static_cast<uint8_t>(b);
    }

    constexpr WindowData FULLSCREEN_WINDOW_DATA{1920, 1080, WindowMode::FULLSCREEN};
    constexpr WindowData WINDOWED_WINDOW_DATA{800, 600, WindowMode::WINDOWED};

    class MainWindow
    {
    public:
        explicit MainWindow(const std::string& windowTitle = "DefaultWindow", const WindowData& windowData = FULLSCREEN_WINDOW_DATA);

        [[nodiscard]] bool isWindowOpened() const;
        [[nodiscard]] int getHeight() const;
        [[nodiscard]] int getWidth() const;
        [[nodiscard]] WindowData getWindowData() const;
        [[nodiscard]] GLFWwindow* getOpenGLWindow() const;


        static void pollEvents();

        static float getTime();

        static void setViewport(int x, int y, int width, int height);

        static void clear(ClearFlag flags);

        static void setCullMode(CullMode mode);

        static void setDepthFunc(bool enabled);

        static void lineWidth(float lineWidth);

        void viewport() const;

        void swapBuffers() const;

        void setSize(int width, int height);

    private:
        GLFWwindow* m_window{nullptr};
        WindowData m_currentWindowData{window::FULLSCREEN_WINDOW_DATA};
        bool m_hasFocus{false};


        static int viewportX;
        static int viewportY;
        static int viewportWidth;
        static int viewportHeight;


        
        const std::string m_windowName;
    };
}

#endif //MAIN_WINDOW_HPP