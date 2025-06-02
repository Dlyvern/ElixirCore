#ifndef APPLICATION_HPP
#define APPLICATION_HPP


namespace elix
{
    class Application
    {
    public:
        static Application& instance();

        void init();

        void shutdown();

    private:
        Application() = default;
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
    };
}

#endif //APPLICATION_HPP
