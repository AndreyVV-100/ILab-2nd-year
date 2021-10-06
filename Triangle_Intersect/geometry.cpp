#include "geometry.h"

namespace geo
{
    bool Point :: CheckCorrect()
    {
        using std::isfinite;
        return isfinite (x_) && isfinite (y_) && isfinite (z_); 
    }

    bool Triangle :: CheckCorrect()
    {
        // ToDo: check triangle inequality
        return A_.CheckCorrect() && B_.CheckCorrect() && C_.CheckCorrect();
    }
}
