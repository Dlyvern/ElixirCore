#include "StaticMesh.hpp"
#include <glad/glad.h>
#include "Buffer.hpp"

#include "DrawCall.hpp"

StaticMesh::StaticMesh(const std::vector<common::Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    setVerticesAndIndices(vertices, indices);
    loadFromRaw();
}

StaticMesh::StaticMesh() = default;

void StaticMesh::draw()
{
    m_vertexArray.bind();
    elix::DrawCall::draw(elix::DrawCall::DrawMode::TRIANGLES, m_indicesCount, elix::DrawCall::DrawType::UNSIGNED_INT, nullptr);
    m_vertexArray.unbind();
}

void StaticMesh::setVerticesAndIndices(const std::vector<common::Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    m_vertices = vertices;
    m_indices = indices;
}

void StaticMesh::loadFromRaw()
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

    m_vertexArray.unbind();
    vbo.unbind();
    ebo.unbind();

    m_indicesCount = m_indices.size();
}

StaticMesh::~StaticMesh() = default;
