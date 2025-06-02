#include "DrawCall.hpp"
#include <glad/glad.h>

namespace
{
    GLenum toGL(const elix::DrawCall::DrawMode& mode)
    {
        switch (mode)
        {
            case elix::DrawCall::DrawMode::TRIANGLES: return GL_TRIANGLES;
            case elix::DrawCall::DrawMode::LINES: return GL_LINES;
        }

        return GL_NONE;
    }

    GLenum toGL(const elix::DrawCall::DrawType& type)
    {
        switch (type)
        {
            case elix::DrawCall::DrawType::UNSIGNED_INT: return GL_UNSIGNED_INT;
        }

        return GL_NONE;
    }
}

void elix::DrawCall::draw(DrawMode drawMode, size_t count, DrawType drawType, const void *indices)
{
    glDrawElements(toGL(drawMode), count, toGL(drawType), indices);
}

void elix::DrawCall::drawArrays(DrawMode drawMode, size_t first, size_t count)
{
    glDrawArrays(toGL(drawMode), first, count);
}
