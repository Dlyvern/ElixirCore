#include "FrameBuffer.hpp"
#include <glad/glad.h>

namespace
{
    GLenum toGL(const elix::FrameBuffer::InternalFormat& internalFormat)
    {
        switch (internalFormat)
        {
            case  elix::FrameBuffer::InternalFormat::DEPTH24: return GL_DEPTH_COMPONENT24;
            case elix::FrameBuffer::InternalFormat::DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8;
        }
    }

    GLenum toGL(const elix::FrameBuffer::Attachment& attachment)
    {
        switch (attachment)
        {
            case elix::FrameBuffer::Attachment::DEPTH: return GL_DEPTH_ATTACHMENT;
            case elix::FrameBuffer::Attachment::DEPTH_STENCIL: return GL_DEPTH_STENCIL_ATTACHMENT;
        }
    }
}

void elix::FrameBuffer::create(int width, int height, InternalFormat format)
{
    m_width = width;
    m_height = height;
    m_internalFormat = format;

    glGenFramebuffers(1, &m_fbo);
    glGenRenderbuffers(1, &m_rbo);
}

void elix::FrameBuffer::addTextureId(unsigned int id)
{
    m_textureIds.push_back(id);
}

void elix::FrameBuffer::addAttachment(Attachment attachment)
{
    m_attachments.insert(attachment);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);

    if (!m_textureIds.empty())
    {
        int index = 0;

        for (const auto& texture : m_textureIds)
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, texture, 0);

        ++index;
    }

    glRenderbufferStorage(GL_RENDERBUFFER, toGL(m_internalFormat), m_width, m_height);

    for (const auto& attach : m_attachments)
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, toGL(attach), GL_RENDERBUFFER, m_rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void elix::FrameBuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void elix::FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

elix::FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteRenderbuffers(1, &m_rbo);
}
