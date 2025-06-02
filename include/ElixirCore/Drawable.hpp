#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

class Drawable
{
public:
    virtual void draw() = 0;
    virtual ~Drawable() = default;
};

#endif //DRAWABLE_HPP
