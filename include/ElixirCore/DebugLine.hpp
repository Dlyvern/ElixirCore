#ifndef DEBUG_LINE_HPP
#define DEBUG_LINE_HPP

#include "Shader.hpp"
#include "Buffer.hpp"
#include "VertexArray.hpp"

namespace elix::debug
{
    class DebugLine
    {
    public:
        DebugLine();

        void draw(const glm::vec3& from, const glm::vec3& to, const glm::mat4 &view, const glm::mat4 &projection);

        void setLineWidth(float width);
        void setColor(const glm::vec4& color);

    private:
        elix::VertexArray m_vao;
        elix::Buffer m_vbo{elix::Buffer::BufferType::Vertex, elix::Buffer::BufferUsage::DynamicDraw};

        float m_lineWidth{2.0f};

        glm::vec4 m_color{0, 1, 0, 1};

        elix::Shader m_shader;
    };
} //namespace debug

#endif //DEBUG_LINE_HPP
