#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include "Shader.hpp"
#include <vector>
#include "Buffer.hpp"
#include "VertexArray.hpp"

namespace elix
{

class Skybox
{
public:
    explicit Skybox(const std::vector<std::string>& faces);

    Skybox();

    void init(const std::vector<std::string>& faces);

    void render(const glm::mat4& view, const glm::mat4& projection) const;

    void loadFromHDR(const std::string& path);

    std::string getAssetPath() const;

private:
    static constexpr unsigned int CUBE_MAP_SIZE{2048};
    unsigned int m_cubeMapTextureId;

    elix::VertexArray m_vertexArray;


    std::string m_assetPath;

    elix::Buffer m_vbo{elix::Buffer::BufferType::Vertex, elix::Buffer::BufferUsage::StaticDraw};

    static unsigned int loadCubemap(const std::vector<std::string>& faces);
};

} //namespace elix

#endif //SKYBOX_HPP
