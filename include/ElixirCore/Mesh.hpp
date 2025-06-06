#ifndef MESH_HPP
#define MESH_HPP

#include "Common.hpp"
#include "VertexArray.hpp"
#include "Material.hpp"

namespace elix
{
    class Mesh
    {
    public:
        Mesh(const std::vector<common::Vertex>& vertices, const std::vector<unsigned int>& indices);

        void draw() const;

        void setMaterial(Material* material);

        [[nodiscard]] Material* getMaterial() const;

        [[nodiscard]] bool hasBones() const;
    private:
        uint32_t m_indicesCount{0};

        elix::VertexArray m_vertexArray;

        Material* m_material{Material::getDefaultMaterial().get()};

        std::vector<common::Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
    };
}

#endif //MESH_HPP
