#ifndef DEPTH_FRAME_BUFFER_HPP
#define DEPTH_FRAME_BUFFER_HPP

namespace elix
{
    class DepthFrameBuffer
    {
    public:
        void init(unsigned int textureId);

        [[nodiscard]] unsigned int getId() const;

        void bind() const;

        void unbind() const;

    private:
        unsigned int m_id{0};
    };
} //namespace elix
#endif //DEPTH_FRAME_BUFFER_HPP
