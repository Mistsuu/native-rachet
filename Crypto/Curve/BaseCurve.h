#pragma once

#include "../../Utils/Utils.h"
#include "Point.h"
#include "KeyPair.h"
#include <iostream>

class BaseCurve
{
protected:
    Int   a;      // Params in y^2 = x^3 + ax^2 + x 
    Int   p;      //           mod p
    Point G;      // Generator point.
    Int   q;      // Order of generator point.
    uint  lp;     // Bit length of p (|p|)
    uint  lq;     // Bit length of q (|q|)
    uint  pbytes; // defined as ceil((|p| + 1)/8)
    uint  b;      // defined as 8 * ( ceil((|p| + 1)/8) )
    Int   d;      // d in equivalent Edward form: dx^2y^2 + 1 = -x^2 + y^2

    BaseCurve()
    {
        // This is put as protected to prevent it from being called
        // publicly.
    }

    // ---------------- ARITHMETICS BUT NO CHECK BEFORE USE ----------------
    Point xADD__unsafe__(Point& P, Point& Q, Point& P_Q)
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

        return Point(xR, zR);
    }

    Point xDBL__unsafe__(Point& P)
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

        return Point(xR, zR);
    }

public:

    // ------------------------------ DATA ABOUT CURVE ------------------------------
    string info()
    {
        return "Elliptic Curve y^2 = x^3 + " + a.get_str() + "x^2 + x mod " + p.get_str();
    }

    Point generatorPoint()
    {
        return G;
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
    bool onCurve(Point& P)
    {
        if (P.z == 0)
            return true;

        // Eval x^3 + ax^2 + x ==> y^2
        Int x  = (P.x * inverse(P.z, this->p)) % this->p;
        Int y2 = ((x + a) * x + 1) * x;

        // Check if y^2 is quadratic residue.
        return isQuadraticResidue(y2, this->p);
    }

    Point xMUL(Point P, Int n)
    {
        if (!onCurve(P)) {
            cerr << "[ ! ] Error: BaseCurve.h: xMUL(): Point is not on curve.\n";
            cerr << "[ ! ]     P: " << P << endl;
            exit(INVALID_POINT_ERROR_CODE);
        }

        if (n == 0) 
            return Point(0, 0);
        
        if (n < 0)
            n = -n;

        vector<Int> nBin;
        nBin = getBits(n);

        Point R0,R1, _R0,_R1;
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
    void normalize(Point* P) 
    {
        if (!onCurve(*P)) {
            cerr << "[ ! ] Error: BaseCurve.h: normalize(): Point is not on curve.\n";
            cerr << "[ ! ]     P: " << *P << endl;
            exit(INVALID_POINT_ERROR_CODE);
        }

        if (P->z == 1)
            return;
        P->x = mod(P->x * inverse(P->z, this->p), this->p);
        P->z = 1;
    }

    // ------------------------------ POINTS TO BUFFER ------------------------------
    Buffer serialize(Point P)
    {
        if (!onCurve(P)) {
            cerr << "[ ! ] Error: BaseCurve.h: serialize(): Point is not on curve.\n";
            cerr << "[ ! ]     P: " << P << endl;
            exit(INVALID_POINT_ERROR_CODE);
        }


        // Create buffer;
        if (!P.z) {
            Buffer serializedInfinityPoint(this->pbytes);
            serializedInfinityPoint.zeroAll();
            return serializedInfinityPoint;
        }


        // 99.999999...% (1-(1/p)) of the cases:
        //     P is not infinity
        // Normalized x and just send its coordinate in bytes form, with \x01 at the beginning.
        Int    x               = mod(P.x * inverse(P.z, this->p), this->p);
        Buffer serializedPoint = Buffer::fromInt(x, this->pbytes);
        serializedPoint[0] ^= 1 << (8 - this->b + this->lp);
        return serializedPoint;

    }

    Point deserialize(Buffer serializedPoint)
    {
        // Catch invalid length input!
        if (serializedPoint.len() != this->pbytes) {
            cerr << "[ ! ] Error: BaseCurve.h: deserialize(): Invalid buffer: Length incorrect: " << serializedPoint.len() << " instead of " << this->pbytes + 1 << "\n";
            cerr << "[ ! ]     buffer.hex(): " << serializedPoint.toHex() << endl;
            exit(INVALID_POINT_ERROR_CODE);
        }


        // Infinity, don't give a shit about the rest... (should we...?)
        if (!(serializedPoint[0] >> (8 - this->b + this->lp)))
            return Point(0, 0);
        

        // Normal point with Z=1
        if (serializedPoint[0] >> (8 - this->b + this->lp) == 1) {

            // Filter out higher bits of the first byte.
            serializedPoint[0] &= ((1 << (8 - this->b + this->lp)) - 1);
            
            // 
            Point P(bytesToInt(serializedPoint.data(), this->pbytes));
            if (!onCurve(P)) {
                cerr << "[ ! ] Error: BaseCurve.h: deserialize(): Invalid buffer: Point not on curve.\n";
                cerr << "[ ! ]     buffer.hex(): " << serializedPoint.toHex() << endl;
                cerr << "[ ! ]     decoded P:    " << P << endl;
                exit(INVALID_POINT_ERROR_CODE);
            }

            return P;
        } 


        // Junk buffer detected...?
        cerr << "[ ! ] Error: BaseCurve.h: deserialize(): Invalid format buffer: The first " << (8 - this->b + this->lp) << " bit(s) is/are not 1 or 0.\n";
        cerr << "[ ! ]     buffer.hex(): " << serializedPoint.toHex() << endl;
        exit(INVALID_POINT_ERROR_CODE);
    }

};

std::ostream& operator<<(std::ostream &os, BaseCurve &curve)
{
    return os << curve.info();
}