cmake_minimum_required(VERSION 3.16)
project(ElixirCore)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === Paths ===
set(INCLUDE_DIR include/ElixirCore)
set(SRC_DIR src)
set(LIBRARIES_DIR libraries)
#set(PHYSX_ROOT "$ENV{HOME}/Libraries/PhysX/physx") # Use env var for ~
#set(PHYSX_BUILD_TYPE "checked" CACHE STRING "The build type of PhysX")
#set_property(CACHE PHYSX_BUILD_TYPE PROPERTY STRINGS debug checked profile release)

# === Source Files ===
file(GLOB_RECURSE PROJECT_HEADERS ${INCLUDE_DIR}/*.h ${INCLUDE_DIR}/*.hpp)
file(GLOB_RECURSE PROJECT_SOURCES ${SRC_DIR}/*.cpp)

# === Build Mode ===
#if(NOT CMAKE_BUILD_TYPE)
#    if(PHYSX_BUILD_TYPE STREQUAL "debug" OR PHYSX_BUILD_TYPE STREQUAL "checked")
#        set(CMAKE_BUILD_TYPE "Debug")
#    endif()
#endif()

# === Output Messages ===
#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    message("Building in Debug with PhysX ${PHYSX_BUILD_TYPE}")
#    add_compile_definitions(_DEBUG)
#else()
#    message("Building in Release with PhysX ${PHYSX_BUILD_TYPE}")
#    add_compile_definitions(NDEBUG)
#endif()
#
# === Backend API Switch ===
option(USE_VULKAN "Use Vulkan backend" OFF)

if(USE_VULKAN)
    add_definitions(-DVULKAN_API)
    add_subdirectory(API/VulkanShit)
else()
    add_definitions(-DOPENGL_API)
    add_subdirectory(API/GLitch)
endif()

# === External Dependencies ===
find_package(OpenGL REQUIRED)
find_package(glfw3 3.4 REQUIRED)
find_package(assimp REQUIRED)
find_package(glm REQUIRED)
find_package(Freetype REQUIRED)
find_package(Threads REQUIRED)

#link_directories("${PHYSX_ROOT}/bin/linux.x86_64/${PHYSX_BUILD_TYPE}")
link_directories(/usr/local/lib)

# === Create Static Library ===
add_library(${PROJECT_NAME} STATIC ${PROJECT_HEADERS} ${PROJECT_SOURCES})
add_library(Elixir::ElixirCore ALIAS ElixirCore)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
# === Public Includes ===
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ElixirCore>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARIES_DIR}>
        $<BUILD_INTERFACE:${PHYSX_ROOT}/include>
        $<INSTALL_INTERFACE:include>
        ${OPENGL_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
        ${GLM_INCLUDE_DIR}
        ${FREETYPE_INCLUDE_DIRS}
)

# === Backend-Specific Files ===
if(USE_VULKAN)
    target_include_directories(${PROJECT_NAME} PUBLIC API/VulkanShit/include)
    target_sources(${PROJECT_NAME} PRIVATE API/VulkanShit/src/Shader.cpp)
else()
    target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/API/GLitch/include>)
    target_sources(${PROJECT_NAME} PRIVATE
            API/GLitch/src/Shader.cpp
            API/GLitch/src/Texture.cpp
    )
endif()

# === Link Libraries ===
target_link_libraries(${PROJECT_NAME} PUBLIC
        OpenGL::OpenGL
        glfw
        glad
        assimp::assimp
        glm::glm
        Threads::Threads
        Freetype::Freetype
#        -Wl,--start-group
#        PhysXExtensions_static_64
#        PhysX_static_64
#        PhysXPvdSDK_static_64
#        PhysXCommon_static_64
#        PhysXFoundation_static_64
#        PhysXCharacterKinematic_static_64
#        -Wl,--end-group
        pthread
        dl)

# === Install ElixirCore and Export Targets ===
install(TARGETS ElixirCore
        EXPORT ElixirCoreTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

# === Install CMake Config Package ===
install(EXPORT ElixirCoreTargets
        FILE ElixirCoreTargets.cmake
        NAMESPACE Elixir::
        DESTINATION lib/cmake/ElixirCore
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/ElixirCoreConfigVersion.cmake"
        VERSION 1.0.0
        COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ElixirCoreConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/ElixirCoreConfig.cmake"
        INSTALL_DESTINATION lib/cmake/ElixirCore
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ElixirCoreConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ElixirCoreConfigVersion.cmake"
        DESTINATION lib/cmake/ElixirCore
)