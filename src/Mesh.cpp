#include "Mesh.hpp"
#include "Buffer.hpp"
#include "DrawCall.hpp"

elix::Mesh::Mesh(const std::vector<common::Vertex> &vertices, const std::vector<unsigned int> &indices) : m_vertices(vertices), m_indices(indices)
{
    elix::Buffer vbo(elix::Buffer::BufferType::Vertex, elix::Buffer::BufferUsage::StaticDraw);
    elix::Buffer ebo(elix::Buffer::BufferType::Index, elix::Buffer::BufferUsage::StaticDraw);

    m_vertexArray.create();
    vbo.create();
    ebo.create();

    m_vertexArray.bind();

    vbo.uploadRaw(m_vertices.data(), m_vertices.size() * sizeof(common::Vertex));

    ebo.uploadRaw(m_indices.data(), m_indices.size() * sizeof(unsigned int));

    m_vertexArray.setAttribute(0, 3, elix::VertexArray::Type::Float, false, sizeof(common::Vertex),  (void*)offsetof(common::Vertex, position));
    m_vertexArray.setAttribute(1, 3, elix::VertexArray::Type::Float, false, sizeof(common::Vertex),  (void*)offsetof(common::Vertex, normal));
    m_vertexArray.setAttribute(2, 2, elix::VertexArray::Type::Float, false, sizeof(common::Vertex),  (void*)offsetof(common::Vertex, textureCoordinates));

    if (hasBones())
    {
        m_vertexArray.setAttribute(3, 3, elix::VertexArray::Type::Float, false, sizeof(common::Vertex),  (void*)offsetof(common::Vertex, tangent));
        m_vertexArray.setAttribute(4, 3, elix::VertexArray::Type::Float, false, sizeof(common::Vertex),  (void*)offsetof(common::Vertex, bitangent));
        m_vertexArray.setAttribute(5, 4, elix::VertexArray::Type::Int, false, sizeof(common::Vertex),  (void*)offsetof(common::Vertex, boneID));
        m_vertexArray.setAttribute(6, 4, elix::VertexArray::Type::Float, false, sizeof(common::Vertex),  (void*)offsetof(common::Vertex, weight));
    }

    m_vertexArray.unbind();
    vbo.unbind();
    ebo.unbind();

    m_indicesCount = m_indices.size();
}

bool elix::Mesh::hasBones() const
{
    for (const auto& vertex : m_vertices)
        if (vertex.boneID[0] != -1)
            return true;

    return false;
}

void elix::Mesh::draw() const
{
    m_vertexArray.bind();
    elix::DrawCall::draw(elix::DrawCall::DrawMode::TRIANGLES, m_indicesCount, elix::DrawCall::DrawType::UNSIGNED_INT, nullptr);
    m_vertexArray.unbind();
}

void elix::Mesh::setMaterial(Material *material)
{
    m_material = material;
}

Material * elix::Mesh::getMaterial() const
{
    return m_material;
}
