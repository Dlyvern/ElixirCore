#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include <set>
#include <vector>

namespace elix
{
    class FrameBuffer
    {
    public:

        enum class InternalFormat
        {
            DEPTH24,
            DEPTH24_STENCIL8,
        };

        enum class Attachment
        {
            DEPTH,
            DEPTH_STENCIL,
        };

        void create(int width, int height, InternalFormat format);

        void addTextureId(unsigned int id);

        void addAttachment(Attachment attachment);

        void bind() const;

        static void unbind();

        ~FrameBuffer();

    private:
        std::vector<unsigned int> m_textureIds;

        int m_width{0};
        int m_height{0};

        InternalFormat m_internalFormat;
        std::set<Attachment> m_attachments;

        unsigned int m_fbo{0};
        unsigned int m_rbo{0};
    };
} //namespace elix

#endif //FRAME_BUFFER_HPP
