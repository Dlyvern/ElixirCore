#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP


namespace elix
{
    class FrameBuffer
    {
    public:
        void create(int width, int height);

        void bind() const;

        static void unbind();

    private:
        unsigned int m_fbo{0};
        unsigned int m_rbo{0};
    };
} //namespace elix

#endif //FRAME_BUFFER_HPP
