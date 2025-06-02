#include "Buffer.hpp"
#include <glad/glad.h>

namespace
{
    GLenum toGL(const elix::Buffer::BufferType& type)
    {
        switch (type)
        {
            case elix::Buffer::BufferType::Vertex: return GL_ARRAY_BUFFER;
            case elix::Buffer::BufferType::Index: return GL_ELEMENT_ARRAY_BUFFER;
            case elix::Buffer::BufferType::Uniform: return GL_UNIFORM_BUFFER;
        }

        return 0;
    }

    GLenum toGL(const elix::Buffer::BufferUsage& type)
    {
        switch (type)
        {
            case elix::Buffer::BufferUsage::DynamicDraw: return GL_DYNAMIC_DRAW;
            case elix::Buffer::BufferUsage::StaticDraw: return GL_STATIC_DRAW;
            case elix::Buffer::BufferUsage::StreamDraw: return GL_STREAM_DRAW;
        }

        return 0;
    }
} //namespace

elix::Buffer::Buffer(BufferType type, BufferUsage usage) : m_bufferType(type), m_bufferUsage(usage)
{

}

void elix::Buffer::create()
{
    glGenBuffers(1, &m_id);
}

void elix::Buffer::bind() const
{
    glBindBuffer(toGL(m_bufferType), m_id);
}

void elix::Buffer::unbind()
{
    glBindBuffer(toGL(m_bufferType), 0);
}

void elix::Buffer::uploadRaw(const void *data, size_t size)
{
    bind();
    glBufferData(toGL(m_bufferType), size, data, toGL(m_bufferUsage));
    // unbind();
}

elix::Buffer::~Buffer()
{
    if (m_id)
        glDeleteBuffers(1, &m_id);
}
