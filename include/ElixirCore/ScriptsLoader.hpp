#ifndef SCRIPTS_LOADER_HPP
#define SCRIPTS_LOADER_HPP

#include <string>

class ScriptsLoader
{
public:
    static ScriptsLoader& instance();

    void* loadLibrary(const std::string& libraryPath);

    void* getFunction(const std::string& functionName, void* library);

    void* library = nullptr;

    //TODO: do not forget to free memory here
    // void unloadLibrary(const std::string& libraryPath);
    ~ScriptsLoader() = default;
private:
    ScriptsLoader() = default;
    ScriptsLoader(const ScriptsLoader&) = delete;
    ScriptsLoader& operator=(const ScriptsLoader&) = delete;
};
//
// namespace loader
// {
//     void loadScripts()
//     {
//         void* handle = dlopen("/home/dlyvern/Projects/Elixir/libTestGame.so", RTLD_LAZY);
//         if (!handle) {
//             std::cerr << "Failed to load game: " << dlerror() << '\n';
//             return;
//         }
//
//         using InitFunc = const char**(*)(int*);
//         using UpdateFunc = void(*)(const char*, float);
//
//     #ifdef _WIN32
//         HMODULE lib = LoadLibrary("TestGame.dll");
//         if (!lib) { /* handle error */ }
//
//         InitScriptsFunc initScripts = reinterpret_cast<InitScriptsFunc>(
//             GetProcAddress(lib, "initScripts")
//         );
//
//         UpdateFunc update = reinterpret_cast<UpdateFunc>(GetProcAddress(lib, "updateScript"))
//     #else
//         InitFunc initScripts = (InitFunc)dlsym(handle, "initScripts");
//         if (!initScripts)
//         {
//             std::cerr << "Failed to find initScripts: " << dlerror() << '\n';
//             return;
//         }
//
//         UpdateFunc updateScript = (UpdateFunc)dlsym(handle, "updateScript");
//
//         if (!updateScript) {
//             std::cerr << "Failed to load updateScript: " << dlerror() << '\n';
//             return;
//         }
//     #endif
//
//         int count = 0;
//         const char** scripts = initScripts(&count);
//
//         for (int i = 0; i < count; ++i) {
//             std::string scriptName = scripts[i];
//             std::cout << "Available script: " << scriptName << std::endl;
//
//             updateScript(scriptName.c_str(), 0.f);
//         }
//     }
// }

#endif //SCRIPTS_LOADER_HPP