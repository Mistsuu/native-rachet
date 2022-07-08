#pragma once

#include "Utils/Utils.h"
#include "PointMongomery.h"
#include "PointEdwards.h"

class BaseCurve
{
protected:
    Int            a;               // Params in y^2 = x^3 + ax^2 + x 
    Int            p;               //           mod p
    PointMongomery Gmo;             // Generator point on Mongomery Curve.
    PointEdwards   Ged;             // Generator point on Edwards Curve.
    Int            q;               // Order of generator point.
    uint           lp;              // Bit length of p (|p|)
    uint           lq;              // Bit length of q (|q|)
    uint           pbytes;          // defined as ceil((|p| + 1)/8)
    uint           b;               // defined as 8 * ( ceil((|p| + 1)/8) )
    Int            d;               // d in equivalent Edward form: dx^2y^2 + 1 = -x^2 + y^2

    BaseCurve()
    {
        // This is put as protected to prevent it from being called
        // publicly.
    }

    // ---------------- ARITHMETICS BUT NO CHECK BEFORE USE ----------------
    PointMongomery xADD__unsafe__(PointMongomery& P, PointMongomery& Q, PointMongomery& P_Q)
    {
        Int V0,V1,V2,V3,V4, xR,zR;      // N = number of bits in p
        V0 = P.x + P.z;                 // V0 -> N+1 bits
        V1 = Q.x - Q.z;                 // V1 -> N+1 bits
        V1 = V1*V0    ;                 // V1 -> 2N+2 bits
        V0 = P.x - P.z;                 // V0 -> N+1 bits
        V2 = Q.x + Q.z;                 // V2 -> N+1 bits
        V2 = V2*V0    ;                 // V2 -> 2N+2 bits
        V3 = V1+V2    ;                 // V3 -> 2N+3 bits
        V3 = V3*V3    ;                 // V3 -> 4N+6 bits
        V4 = V1-V2    ;                 // V4 -> 2N+3 bits
        V4 = V4*V4    ;                 // V4 -> 4N+6 bits
        xR = mod(P_Q.z * V3, this->p);
        zR = mod(P_Q.x * V4, this->p);

        return PointMongomery(xR, zR);
    }

    PointMongomery xDBL__unsafe__(PointMongomery& P)
    {
        Int V1,V2,V3, xR,zR;
        V1 = P.x + P.z;
        V1 = V1*V1;
        V2 = P.x - P.z;
        V2 = V2*V2;
        xR = mod(V1*V2, this->p);
        V1 = V1-V2;
        V3 = (this->a + 2)/4 * V1;
        V3 = V3+V2;
        zR = mod(V1*V3, this->p);

        return PointMongomery(xR, zR);
    }

    PointEdwards edDBL__unsafe__(PointEdwards& P)
    {
        this->recoverXEdwards(&P);

        Int Px_2 = P.x*P.x;
        Int Py_2 = P.y*P.y;

        Int Rx = mod(((2*P.x*P.y) * inverse(  Py_2-Px_2, this->p)), this->p);
        Int Ry = mod(((Py_2+Px_2) * inverse(2-Py_2+Px_2, this->p)), this->p);
        return PointEdwards(
            Rx,
            Ry,
            (Rx % 2 == 1) ? 1 : 0
        );
    }

    PointEdwards edADD__unsafe__(PointEdwards& P, PointEdwards& Q)
    {
        this->recoverXEdwards(&P);
        this->recoverXEdwards(&Q);

        Int dPxPyQxQy = mod(this->d * P.x*Q.x*P.y*Q.y, this->p);
        Int Rx = mod(((P.x*Q.y + Q.x*P.y) * inverse(1 + dPxPyQxQy, this->p)), this->p);
        Int Ry = mod(((P.y*Q.y + P.x*Q.x) * inverse(1 - dPxPyQxQy, this->p)), this->p);
        return PointEdwards(
            Rx,
            Ry,
            (Rx % 2 == 1) ? 1 : 0
        );
    }

    void edNEG__unsafe__(PointEdwards* P)
    {
        if (P->x != EMPTY_X_COORDINATE_EDWARDS)
            P->x = this->p - P->x;
        P->s = 1 - P->s;
    }

public:

    // ------------------------------ DATA ABOUT CURVE ------------------------------
    string info()
    {
        return "Elliptic Curve y^2 = x^3 + " + a.get_str() + "x^2 + x mod " + p.get_str();
    }

    PointMongomery generatorPointMongomery()
    {
        return this->Gmo;
    }

    PointEdwards& generatorPointEdwards()
    {
        return this->Ged;
    }

    Int generatorOrder()
    {
        return this->q;
    }

    Int modulus()
    {
        return this->p;
    }

    Int mongomeryCoef()
    {
        return this->a;
    }

    Int edwardsCoef()
    {
        return this->d;
    }

    uint pBitLength()
    {
        return this->lp;
    }

    uint curveBitLength()
    {
        return this->b;
    }

    uint curveSizeBytes()
    {
        return this->pbytes;
    }


    // ------------------------------ ARITHMETICS ------------------------------
    bool onCurve(PointMongomery P)
    {
        if (P.z == 0)
            return true;

        // Eval x^3 + ax^2 + x ==> y^2
        Int x  = mod((P.x * inverse(P.z, this->p)), this->p);
        Int y2 = ((x + a) * x + 1) * x;

        // Check if y^2 is quadratic residue.
        return isQuadraticResidue(y2, this->p);
    }

    bool onCurve(PointEdwards P)
    {
        if (P.x == EMPTY_X_COORDINATE_EDWARDS) {
            Int x2 = mod(((P.y*P.y - 1) * inverse(this->d*P.y*P.y + 1, this->p)), this->p);
            return isQuadraticResidue(x2, this->p);
        } else {
            return (
                   mod((-P.x*P.x + P.y*P.y),            this->p) 
                == mod((1 + this->d * P.x*P.x*P.y*P.y), this->p)
            );
        }
    }

    void edNEG(PointEdwards* P)
    {
        if (!onCurve(*P)) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error: BaseCurve.h: edADD(): PointEdwards is not on curve.\n";
            errorStream << "[ ! ]     P: " << P << std::endl;
            throw InvalidPointException(errorStream.str());
        }
        edNEG__unsafe__(P);
    }


    PointEdwards edADD(PointEdwards& P, PointEdwards& Q)
    {
        if (!onCurve(P)) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error: BaseCurve.h: edADD(): PointEdwards is not on curve.\n";
            errorStream << "[ ! ]     P: " << P << std::endl;
            throw InvalidPointException(errorStream.str());
        }
        return edADD__unsafe__(P, Q);
    }

    PointEdwards edMUL(PointEdwards& P, Int n)
    {
        if (!onCurve(P)) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error: BaseCurve.h: edMUL(): PointEdwards is not on curve.\n";
            errorStream << "[ ! ]     P: " << P << std::endl;
            throw InvalidPointException(errorStream.str());
        }

        if (n == 0) 
            return PointEdwards(0, 1, 0);
        
        if (n < 0)
            n = -n;

        vector<Int> nBin;
        nBin = getBits(n);

        PointEdwards R0,R1, _R0,_R1;
        R0 = P;
        R1 = edDBL__unsafe__(P);

        for (int i = nBin.size() - 2; i >= 0; --i) {
            if (nBin[i] == 0) {
                _R0 = edDBL__unsafe__(R0);
                _R1 = edADD__unsafe__(R0, R1);
            } else {
                _R0 = edADD__unsafe__(R0, R1);
                _R1 = edDBL__unsafe__(R1);
            }

            R0 = _R0;
            R1 = _R1;
        }

        return R0;
    }

    PointMongomery xMUL(PointMongomery P, Int n)
    {
        if (!onCurve(P)) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error: BaseCurve.h: xMUL(): PointMongomery is not on curve.\n";
            errorStream << "[ ! ]     P: " << P << std::endl;
            throw InvalidPointException(errorStream.str());
        }

        if (n == 0) 
            return PointMongomery(0, 0);
        
        if (n < 0)
            n = -n;

        vector<Int> nBin;
        nBin = getBits(n);

        PointMongomery R0,R1, _R0,_R1;
        R0 = P;
        R1 = xDBL__unsafe__(P);

        for (int i = nBin.size() - 2; i >= 0; --i) {
            if (nBin[i] == 0) {
                _R0 = xDBL__unsafe__(R0);
                _R1 = xADD__unsafe__(R0, R1, P);
            } else {
                _R0 = xADD__unsafe__(R0, R1, P);
                _R1 = xDBL__unsafe__(R1);
            }

            R0 = _R0;
            R1 = _R1;
        }

        return R0;
    }

    // ------------------------------ "FIX" POINTS ------------------------------
    void normalize(PointMongomery* P) 
    {
        if (!onCurve(*P)) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error: BaseCurve.h: normalize(): PointMongomery is not on curve.\n";
            errorStream << "[ ! ]     P: " << *P << std::endl;
            throw InvalidPointException(errorStream.str());
        }

        if (P->z == 1)
            return;
        P->x = mod(P->x * inverse(P->z, this->p), this->p);
        P->z = 1;
    }

    void recoverXEdwards(PointEdwards* P)
    {
        if (P->x == EMPTY_X_COORDINATE_EDWARDS) {
            P->x = sqrtMod((P->y*P->y - 1) * inverse(this->d*P->y*P->y + 1, this->p), this->p);
            if ((P->s == 1 && P->x % 2 == 0) 
             || (P->s == 0 && P->x % 2 == 1))
                P->x = this->p - P->x;
        }
    }

    // ------------------------------ POINT CONVERTER -------------------------------
    PointEdwards mongomeryToEdwards(PointMongomery P)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! BaseCurve.h: mongomeryToEdwards() is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return PointEdwards();
    }

};

std::ostream& operator<<(std::ostream &os, BaseCurve &curve)
{
    return os << curve.info();
}