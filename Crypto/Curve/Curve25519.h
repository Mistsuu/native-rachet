#pragma once

#include "../../Utils/Utils.h"
#include "Point.h"
#include "KeyPair.h"
#include "BaseCurve.h"

// ============================================== CURVE ==============================================
class Curve25519: public BaseCurve
{
public:
    // ------------------------------ CONSTRUCTOR() ------------------------------
    Curve25519()
    {
        this->a       = "486662";
        this->p       = "57896044618658097711785492504343953926634992332820282019728792003956564819949";
        this->G       = Point(9);
        this->q       = "7237005577332262213973186563042994240857116359379907606001950938285454250989";
        this->lp      = bitLength(this->p);
        this->lq      = bitLength(this->q);
        this->pbytes  = (this->lp >> 3) + 1;
        this->b       = this->pbytes << 3;
        this->d       = mod((this->p - 121665) * inverse(121666, this->p), this->p);
    }
};