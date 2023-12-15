module;

#include <cassert>

export module Shapes:Square;

import :Base;
import :Rectangle;
import :Point;

namespace Shapes
{
    export class Square : public Shape
    {
        Rectangle rect_;

    public:
        static constexpr const char* id = "Square";

        Square(int x = 0, int y = 0, int size = 0);

        Point coord() const;

        void set_coord(const Point& pt);

        int size() const;

        void set_size(int size);

        void draw() const override;

        void move(int dx, int dy) override;
    };
}