cmake_minimum_required(VERSION 3.16)
project(ElixirCore)

if(WIN32)
    set(ELIXIR_PLATFORM "windows")
    set(ELIXIR_PLATFORM_WINDOWS ON)
    message(STATUS "Building for Windows platform")
elseif(UNIX)
    set(ELIXIR_PLATFORM "linux")
    set(ELIXIR_PLATFORM_LINUX ON)
    message(STATUS "Building for Linux platform")
elseif(APPLE)
    message(FATAL_ERROR "macOS is not currently supported")
#    set(ELIXIR_PLATFORM "macos")
#    set(ELIXIR_PLATFORM_MACOS ON)
#    message(STATUS "Building for macOS platform")
else()
    message(FATAL_ERROR "Unsupported platform! Only Windows and Linux are supported")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === Paths ===
set(INCLUDE_DIR include/ElixirCore)
set(SRC_DIR src)
set(LIBRARIES_DIR libraries)
set(EXTERNAL_DIR external)

set(GLM_ROOT ${CMAKE_SOURCE_DIR}/external/glm)
set(PHYSX_ROOT ${CMAKE_SOURCE_DIR}/external/physx)
set(GLFW_ROOT ${CMAKE_SOURCE_DIR}/external/glfw)
set(ASSIMP_ROOT ${CMAKE_SOURCE_DIR}/external/assimp)
set(GLAD_ROOT ${CMAKE_SOURCE_DIR}/external/glad)
set(FREETYPE_ROOT ${CMAKE_SOURCE_DIR}/external/freetype)
set(LIBZ_ROOT ${CMAKE_SOURCE_DIR}/external/libz)

set(PHYSX_BUILD_TYPE "checked" CACHE STRING "The build type of PhysX")

set_property(CACHE PHYSX_BUILD_TYPE PROPERTY STRINGS debug checked profile release)

# === Source Files ===
file(GLOB_RECURSE PROJECT_HEADERS ${INCLUDE_DIR}/*.h ${INCLUDE_DIR}/*.hpp)
file(GLOB_RECURSE PROJECT_SOURCES ${SRC_DIR}/*.cpp)

# === Build Mode ===
if(NOT CMAKE_BUILD_TYPE)
    if(PHYSX_BUILD_TYPE STREQUAL "debug" OR PHYSX_BUILD_TYPE STREQUAL "checked")
        set(CMAKE_BUILD_TYPE "Debug")
    endif()
endif()

# === Output Messages ===
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building in Debug with PhysX ${PHYSX_BUILD_TYPE}")
    add_compile_definitions(_DEBUG)
else()
    message("Building in Release with PhysX ${PHYSX_BUILD_TYPE}")
    add_compile_definitions(NDEBUG)
endif()

# === External Dependencies ===
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

if(ELIXIR_PLATFORM_WINDOWS)
    find_package(directx REQUIRED)
    add_definitions(-DNOMINMAX)
endif()

# === Create Static Library ===
add_library(${PROJECT_NAME} STATIC ${PROJECT_HEADERS} ${PROJECT_SOURCES})
add_library(Elixir::ElixirCore ALIAS ElixirCore)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# === Public Includes ===
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARIES_DIR}>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ElixirCore>
        $<INSTALL_INTERFACE:include>

        $<BUILD_INTERFACE:${PHYSX_ROOT}/include/>
        $<INSTALL_INTERFACE:include/ElixirCore/PhysX>

        $<BUILD_INTERFACE:${GLM_ROOT}/include/>
        $<INSTALL_INTERFACE:include/ElixirCore/glm>

        $<BUILD_INTERFACE:${GLFW_ROOT}/include/>
        $<INSTALL_INTERFACE:include/ElixirCore/glfw>

        $<BUILD_INTERFACE:${GLAD_ROOT}/include/>
        $<INSTALL_INTERFACE:include/ElixirCore/glad>

        $<BUILD_INTERFACE:${FREETYPE_ROOT}/include/>
        $<INSTALL_INTERFACE:include/ElixirCore/freetype>

        $<BUILD_INTERFACE:${ASSIMP_ROOT}/include/>
        $<INSTALL_INTERFACE:include/ElixirCore/assimp>

        ${OPENGL_INCLUDE_DIRS}
)

# === Link Libraries ===
if(ELIXIR_PLATFORM_WINDOWS)
    target_link_libraries(${PROJECT_NAME} PUBLIC
            ${PHYSX_ROOT}/lib/windows/${PHYSX_BUILD_TYPE}/PhysXExtensions_static_64.lib
            ${PHYSX_ROOT}/lib/windows/${PHYSX_BUILD_TYPE}/PhysX_static_64.lib
            ${PHYSX_ROOT}/lib/windows/${PHYSX_BUILD_TYPE}/PhysXPvdSDK_static_64.lib
            ${PHYSX_ROOT}/lib/windows/${PHYSX_BUILD_TYPE}/PhysXCommon_static_64.lib
            ${PHYSX_ROOT}/lib/windows/${PHYSX_BUILD_TYPE}/PhysXFoundation_static_64.lib
            ${PHYSX_ROOT}/lib/windows/${PHYSX_BUILD_TYPE}/PhysXCharacterKinematic_static_64.lib
            OpenGL::OpenGL
            ${GLM_ROOT}/lib/glm.lib
            ${GLFW_ROOT}/lib/glfw3.lib
            ${GLAD_ROOT}/lib/glad.lib
            ${ASSIMP_ROOT}/lib/assimp.lib
            ${FREETYPE_ROOT}/lib/freetype.lib
            Threads::Threads
            ${LIBZ_ROOT}/lib/zlib.lib
            opengl32.lib
    )
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC
            -Wl,--start-group
            ${PHYSX_ROOT}/lib/linux/${PHYSX_BUILD_TYPE}/libPhysXExtensions_static_64.a
            ${PHYSX_ROOT}/lib/linux/${PHYSX_BUILD_TYPE}/libPhysX_static_64.a
            ${PHYSX_ROOT}/lib/linux/${PHYSX_BUILD_TYPE}/libPhysXPvdSDK_static_64.a
            ${PHYSX_ROOT}/lib/linux/${PHYSX_BUILD_TYPE}/libPhysXCommon_static_64.a
            ${PHYSX_ROOT}/lib/linux/${PHYSX_BUILD_TYPE}/libPhysXFoundation_static_64.a
            ${PHYSX_ROOT}/lib/linux/${PHYSX_BUILD_TYPE}/libPhysXCharacterKinematic_static_64.a
            -Wl,--end-group
            OpenGL::OpenGL
            ${GLM_ROOT}/lib/libglm.a
            ${GLFW_ROOT}/lib/libglfw3.a
            ${GLAD_ROOT}/lib/libglad.a
            ${ASSIMP_ROOT}/lib/libassimp.a
            ${FREETYPE_ROOT}/lib/libfreetype.a
            Threads::Threads
            ${LIBZ_ROOT}/lib/libz.a
    )
endif()
# === Install ElixirCore and Export Targets ===
install(TARGETS ElixirCore
        EXPORT ElixirCoreTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY ${PHYSX_ROOT}/include/ DESTINATION include/ElixirCore/PhysX)
install(DIRECTORY ${GLM_ROOT}/include/ DESTINATION include/ElixirCore/glm)
install(DIRECTORY ${GLFW_ROOT}/include/ DESTINATION include/ElixirCore/glfw)
install(DIRECTORY ${ASSIMP_ROOT}/include/ DESTINATION include/ElixirCore/assimp)
install(DIRECTORY ${GLAD_ROOT}/include/ DESTINATION include/ElixirCore/glad)
install(DIRECTORY ${FREETYPE_ROOT}/include/ DESTINATION include/ElixirCore/freetype)

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


#set(REQUIRED_PHYSX_LIBS
#        libPhysXExtensions_static_64.a
#        libPhysX_static_64.a
#        libPhysXPvdSDK_static_64.a
#        libPhysXCommon_static_64.a
#        libPhysXFoundation_static_64.a
#        libPhysXCharacterKinematic_static_64.a
#)
#
#foreach(lib ${REQUIRED_PHYSX_LIBS})
#    if(NOT EXISTS "${PHYSX_ROOT}/lib/linux/${PHYSX_BUILD_TYPE}/${lib}")
#        message(FATAL_ERROR "Missing PhysX library: ${lib}")
#    endif()
#endforeach()
