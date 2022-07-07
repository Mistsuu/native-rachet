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
    Int x;
    Int y;
    int s;
    PointEdwards(Int y)
    {
        this->x = EMPTY_X_COORDINATE_EDWARDS;
        this->y = y;
        this->s = 1;
    }

    PointEdwards(Int y, int s)
    {
        this->x = EMPTY_X_COORDINATE_EDWARDS;
        this->y = y;
        this->s = s;
    }

    PointEdwards(Int x, Int y, int s)
    {
        this->x = x;
        this->y = y;
        this->s = s;
    }

    PointEdwards()
    {
        this->x = EMPTY_X_COORDINATE_EDWARDS;
        this->y = 0;
        this->s = 0;
    }

    void __debug__()
    {
        if (this->x == EMPTY_X_COORDINATE_EDWARDS)
            std::cout << "(X:" << this->y << ")" << std::endl;
        else
            std::cout << "(" << this->x << ":" << this->y << ")" << std::endl;
    }
};

std::ostream& operator<<(std::ostream &os, PointEdwards const &P)
{
    if (P.x == EMPTY_X_COORDINATE_EDWARDS)
        return os << "(X:" << P.y << ")";
    else 
        return os << "(" << P.x << ":" << P.y << ")";
}