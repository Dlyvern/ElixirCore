#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstddef>

namespace elix
{
    class Buffer
    {
    public:
        enum class BufferType
        {
            Vertex,
            Index,
            Uniform
        };

        enum class BufferUsage
        {
            StaticDraw,
            DynamicDraw,
            StreamDraw,
        };


        Buffer(BufferType type, BufferUsage usage);

        void create();

        void bind() const;

        void unbind();

        void uploadRaw(const void* data, size_t size);

        ~Buffer();
    private:
        unsigned int m_id{0};
        BufferType m_bufferType;
        BufferUsage m_bufferUsage;
    };
} //namespace elix
#endif //BUFFER_HPP
