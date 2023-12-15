module;

#include <cassert>

module Shapes;

import :Rectangle;

namespace Shapes
{
    Square::Square(int x, int y, int size)
        : rect_{x, y, size, size}
    {
    }

    void Square::move(int dx, int dy)
    {
        rect_.move(dx, dy);
    }

    Point Square::coord() const
    {
        return rect_.coord();
    }

    void Square::set_coord(const Point& pt)
    {
        rect_.set_coord(pt);
    }

    int Square::size() const
    {
        return rect_.height();
    }

    void Square::set_size(int size)
    {
        rect_.set_width(size);
        rect_.set_height(size);

        assert(rect_.width() == rect_.height());
    }

    void Square::draw() const
    {
        rect_.draw();
    }
}