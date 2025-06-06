#include "ShaderManager.hpp"
#include "Filesystem.hpp"
#include "Skeleton.hpp"
#include "EmbeddedShaders.hpp"
#include <iostream>


ShaderManager& ShaderManager::instance()
{
    static ShaderManager instance;
    return instance;
}

elix::Shader* ShaderManager::getShader(const ShaderType& type)
{
    return &m_shaders[type];
}

void ShaderManager::preLoadShaders()
{
    auto createShader = [](const char* vert, const char* frag)
    {
        elix::Shader shader;
        shader.loadBinaries(vert, frag);
        return shader;
    };

    m_shaders[SKELETON] = createShader(shader_skeleton_vert, shader_skeleton_frag);
    m_shaders[STATIC] = createShader(shader_cube_vert, shader_cube_frag);
    m_shaders[STATIC_SHADOW] = createShader(shader_shadow_map_vert, shader_shadow_map_frag);
    m_shaders[SKELETON_SHADOW] = createShader(shader_shadow_vert, shader_shadow_frag);
    m_shaders[POST_PROCESSING] = createShader(shader_post_processing_vert, shader_post_processing_frag);
    m_shaders[LINE] = createShader(shader_line_vert, shader_line_frag);
    m_shaders[TEXT] = createShader(shader_text_vert, shader_text_frag);
    m_shaders[STATIC_STENCIL] = createShader(shader_cube_vert, shader_stencil_frag);
    m_shaders[SKELETON_STENCIL] = createShader(shader_skeleton_vert, shader_stencil_frag);
    m_shaders[SKYBOX] = createShader(shader_skybox_vert, shader_skybox_frag);
    m_shaders[EQUIRECTANGULAR_TO_CUBEMAP] = createShader(shader_equirectangular_to_cubemap_vert, shader_equirectangular_to_cubemap_frag);
}
