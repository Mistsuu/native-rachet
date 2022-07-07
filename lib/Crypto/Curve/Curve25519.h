#pragma once

#include "Utils/Utils.h"
#include "PointMongomery.h"
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
        this->Gmo     = PointMongomery(9);
        this->Ged     = PointEdwards((8 * inverse(10, this->p)) % p, 0);
        this->q       = "7237005577332262213973186563042994240857116359379907606001950938285454250989";
        this->lp      = bitLength(this->p);
        this->lq      = bitLength(this->q);
        this->pbytes  = (this->lp >> 3) + 1;
        this->b       = this->pbytes << 3;
        this->d       = mod((this->p - 121665) * inverse(121666, this->p), this->p);
    }

    // ------------------------------ POINT CONVERTER -------------------------------
    PointEdwards mongomeryToEdwards(PointMongomery P)
    {
        if (!onCurve(P)) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error: Curve25519.h: mongomeryToEdwards(): PointMongomery is not on curve.\n";
            errorStream << "[ ! ]     P: " << P << std::endl;
            throw InvalidPointException(errorStream.str());
        }

        if (P.z == 0)
            return PointEdwards(0, 1, 0);
        
        Int X = (P.x * inverse(P.z, this->p)) % this->p;
        return PointEdwards(
            ((X-1) * inverse(X+1, this->p)) % this->p,
            0
        );
    }

};