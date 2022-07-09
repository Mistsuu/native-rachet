#pragma once

#include "Utils/Utils.h"

// ============================================== POINT ON MONGOMERY CURVE ==============================================
// =                                                                                                                    =
// =                                    Given X, Y that satisfy this equation:                                          =
// =                                        [   Y^2 = X^3 + AX^2 + X    ]                                               =
// =                          We define a projective point system with x,y,z such that:                                 =
// =                                            ==>  y=Y, x/z=X  <==                                                    =
// =                                y is not stored here to perform Mongomery ladder.                                   =
// =                                                                                                                    =
// ======================================================================================================================
class PointMongomery
{
public:
    Int x;
    Int z;
    PointMongomery(Int x)
    {
        this->x = x;
        this->z = 1;
    }

    PointMongomery(Int x, Int z)
    {
        this->x = x;
        this->z = z;
    }

    PointMongomery()
    {
        this->x = 0;
        this->z = 0;
    }

    static PointMongomery nullPoint()
    {
        return PointMongomery(EMPTY_COORDINATE_MONGOMERY);
    }

    bool isNull()
    {
        return this->x == EMPTY_COORDINATE_MONGOMERY;
    }
};

std::ostream& operator<<(std::ostream &os, PointMongomery const &P)
{
    return os << "(" << P.x << ":Y:" << P.z << ")";
}