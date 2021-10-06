#pragma once
#include <cmath>

namespace geo
{
    struct Point
    {
        double x_ = NAN,
               y_ = NAN,
               z_ = NAN;

        bool CheckCorrect();
    };

    class Triangle
    {
        Point A_, B_, C_;

        public:
        bool CheckCorrect();
    };
    
} // end of namespace geo
