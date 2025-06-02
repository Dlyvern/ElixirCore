#ifndef DRAW_CALL_HPP
#define DRAW_CALL_HPP

#include <cstddef>

namespace elix
{
    class DrawCall
    {
    public:
        enum class DrawMode
        {
            TRIANGLES,
            LINES
        };

        enum class DrawType
        {
            UNSIGNED_INT
        };

        static void draw(DrawMode drawMode, size_t count, DrawType drawType = DrawType::UNSIGNED_INT, const void* indices = nullptr);

        static void drawArrays(DrawMode drawMode, size_t first, size_t count);
    };
} //namespace elix

#endif //DRAW_CALL_HPP
