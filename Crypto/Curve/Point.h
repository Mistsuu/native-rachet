#pragma once

#include <iostream>
#include "../../Utils/Utils.h"

// ============================================== POINT ==============================================
class Point
{
public:
    Int x;
    Int z;
    Point(Int x)
    {
        this->x = x;
        this->z = 1;
    }

    Point(Int x, Int z)
    {
        this->x = x;
        this->z = z;
    }

    Point()
    {
        this->x = 0;
        this->z = 0;
    }

    void __debug__()
    {
        std::cout << "(" << this->x << ":Y:" << this->z << ")" << std::endl;
    }
};

std::ostream& operator<<(std::ostream &os, Point const &P)
{
    return os << "(" << P.x << ":Y:" << P.z << ")";
}