#pragma once

#include <iostream>
#include "Utils/Utils.h"

// ============================================== POINT ON EDWARDS CURVE ==============================================
// =                                                                                                                    =
// =                                    Given X, Y that satisfy this equation:                                          =
// =                                      [   -X^2 + Y^2 = 1 + dX^2Y^2    ]                                             =
// =                                                                                                                    =
// ======================================================================================================================
class PointEdwards
{
public:
    Int y;
    int s;
    PointEdwards(Int y)
    {
        this->y = y;
        this->s = 1;
    }

    PointEdwards(Int y, int s)
    {
        this->y = y;
        this->s = s;
    }

    PointEdwards()
    {
        this->y = 0;
        this->s = 0;
    }

    void __debug__()
    {
        std::cout << "(" << this->y << ":" << this->s << ")" << std::endl;
    }
};

std::ostream& operator<<(std::ostream &os, PointEdwards const &P)
{
    return os << "(" << P.y << ":" << P.s << ")";
}