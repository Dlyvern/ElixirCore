#ifndef VERTEX_ARRAY_HPP
#define VERTEX_ARRAY_HPP

#include <cstddef>

namespace elix
{
    class VertexArray
    {
    public:
        enum class Type
        {
            Float,
            Int
        };

        VertexArray();

        void create();

        void bind() const;

        void setAttribute(int index, size_t size, Type type, bool normalized, size_t stride, const void* data);

        unsigned int getId() const;

        void unbind() const;

        ~VertexArray();

    private:
        unsigned int m_id{0};
    };
} //namespace elix

#endif //VERTEX_ARRAY_HPP
