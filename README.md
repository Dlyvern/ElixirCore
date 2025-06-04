# ElixirCoreAPI (version 0.0.1)

**A high-performance game engine API** providing essential systems for game development

**Only for Linux.** Support for windows and Macos later

## Features

- **Physics**
- **Wrapper around OpenGL**
- **Wrapper around keyboard and mouse**
- **Animations**
- **ECS architecture**

## Quick start

### Dependencies

- CMake 3.16+
- Modern c++ 20 compiler
- OpenGL 4.3+
- The rest of libraries build in this API (works only for Linux ~~for now~~)

### Build

```bash
mkdir build
cd build
cmake ..
sudo make install
```


### CMake

```cmake
find_package(ElixirCore REQUIRED)

target_include_directories(GameLib PRIVATE ${ELIXIR_CORE_INCLUDE})

target_link_libraries(${PROJECT_NAME} PRIVATE Elixir::ElixirCore)
```

## Example

```c++
#include "Elixir/Application.hpp"

int main()
{
    elix::Application::instance().init();
    
    //your main loop of game or engine
    
    return 0;
}
```