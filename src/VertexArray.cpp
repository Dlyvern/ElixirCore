#include "VertexArray.hpp"
#include <glad/glad.h>

namespace
{
    GLenum toGL(const elix::VertexArray::Type& type)
    {
        switch (type)
        {
            case elix::VertexArray::Type::Float: return GL_FLOAT;
            case elix::VertexArray::Type::Int: return GL_INT;
        }

        return GL_NONE;
    }
}

elix::VertexArray::VertexArray() = default;

void elix::VertexArray::create()
{
    glGenVertexArrays(1, &m_id);
}

void elix::VertexArray::bind() const
{
    glBindVertexArray(m_id);
}

void elix::VertexArray::setAttribute(int index, size_t size, Type type, bool normalized, size_t stride, const void *data)
{
    glEnableVertexAttribArray(index);


    if (type == elix::VertexArray::Type::Float)
        glVertexAttribPointer(index, size, toGL(type), normalized ? GL_TRUE : GL_FALSE, stride, data);
    else if (type == elix::VertexArray::Type::Int)
        glVertexAttribIPointer(index, size, toGL(type), stride, data);

}

unsigned int elix::VertexArray::getId() const
{
    return m_id;
}

void elix::VertexArray::unbind() const
{
    glBindVertexArray(0);
}

elix::VertexArray::~VertexArray()
{
    // if (m_id)
        // glDeleteVertexArrays(1, &m_id);
}
