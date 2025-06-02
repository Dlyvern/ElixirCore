#include "ScriptsLoader.hpp"
#include <dlfcn.h>
#include <iostream>

ScriptsLoader& ScriptsLoader::instance()
{
    static ScriptsLoader instance;
    return instance;
}

void* ScriptsLoader::loadLibrary(const std::string &libraryPath)
{
#ifdef _WIN32
    HMODULE lib = LoadLibrary(libraryPath.c_str());

    if (!lib)
        std::cerr << "ScriptsLoader::loadLibrary(): Failed to load library "<< libraryPath << std::endl;

    return lib;
#else

    void* handle = dlopen(libraryPath.c_str(), RTLD_LAZY);

    if (!handle)
        std::cerr << "ScriptsLoader::loadLibrary(): Failed to load library "<< libraryPath << " " << dlerror() << std::endl;


// #ifdef _WIN32
//     FreeLibrary(handle);
// #else
//     dlclose(handle);
// #endif


    return handle;
#endif
}

void* ScriptsLoader::getFunction(const std::string &functionName, void *library)
{
#ifdef _WIN32
    void* function = GetProcAddress(library, functionName.c_str());

    if (!function)
        std::cerr << "ScriptsLoader::getFunction(): Failed to get function " << functionName << std::endl;

    return function;
#else

    void* function = dlsym(library, functionName.c_str());

    if (!function)
        std::cerr << "ScriptsLoader::getFunction(): Failed to get function " << functionName << " " << dlerror() << std::endl;

    return function;

#endif
}
