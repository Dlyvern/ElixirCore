#include "DebugLine.hpp"
#include "DrawCall.hpp"
#include "Filesystem.hpp"
#include "WindowsManager.hpp"
#include "ShaderManager.hpp"

elix::debug::DebugLine::DebugLine()
{
    m_vao.create();
    m_vbo.create();
}

void elix::debug::DebugLine::draw(const glm::vec3 &from, const glm::vec3 &to, const glm::mat4 &view, const glm::mat4 &projection)
{
    window::MainWindow::lineWidth(m_lineWidth);

    const float vertices[] = {
        from.x, from.y, from.z,
        to.x, to.y, to.z
    };

    m_vao.bind();
    m_vbo.bind();

    m_vbo.uploadRaw(vertices, sizeof(vertices));

    m_vao.setAttribute(0, 3, VertexArray::Type::Float, false, 3 * sizeof(float), nullptr);

    const auto shader = ShaderManager::instance().getShader(ShaderManager::ShaderType::LINE);

    shader->bind();

    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setVec4("uColor", m_color);

    elix::DrawCall::drawArrays(elix::DrawCall::DrawMode::LINES, 0, 2);

    m_vao.unbind();
}

void elix::debug::DebugLine::setLineWidth(float width)
{
    m_lineWidth = width;
}

void elix::debug::DebugLine::setColor(const glm::vec4 &color)
{
    m_color = color;
}
