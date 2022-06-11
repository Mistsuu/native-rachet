#pragma once

#include "../../Utils/Utils.h"
#include "Point.h"
#include "KeyPair.h"
#include "BaseCurve.h"
#include <iostream>

using namespace std;

// ============================================== CURVE ==============================================

class Curve448: public BaseCurve
{
public:
    // ------------------------------ CONSTRUCTOR() ------------------------------
    Curve448()
    {
        this->a       = "156326";
        this->p       = "726838724295606890549323807888004534353641360687318060281490199180612328166730772686396383698676545930088884461843637361053498018365439";
        this->G       = Point(5);
        this->q       = "181709681073901722637330951972001133588410340171829515070372549795146003961539585716195755291692375963310293709091662304773755859649779";
        this->lp      = bitLength(this->p);
        this->lq      = bitLength(this->q);
        this->pbytes  = (this->lp >> 3) + 1;
        this->b       = this->pbytes << 3;
        this->d       = mod(39082 * inverse(39081, this->p), this->p);
    }
};