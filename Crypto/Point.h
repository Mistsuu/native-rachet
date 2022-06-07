#pragma once

#include "../Utils/Type.h"
#include "../Utils/Calc.h"
#include "../Utils/Random.h"
#include "../Utils/Const.h"
#include "../Utils/Buffer.h"

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
};

std::ostream& operator<<(std::ostream &os, Point const &P)
{
    return os << "(" << P.x << ":Y:" << P.z << ")";
}