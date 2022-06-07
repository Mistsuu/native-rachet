#pragma once

#include "Type.h"
#include "Buffer.h"
#include "Const.h"
#include <gmp.h>
#include <iostream>
#include <vector>

using namespace std;

int ceildiv(int x, int y)
{
    if (x % y == 0)
        return x / y;
    else
        return x / y + 1;
}

Int pow(Int base, Int exp, Int mod)
{
    Int result;
    mpz_powm(
        result.get_mpz_t(), 
        base.get_mpz_t(), 
        exp.get_mpz_t(), 
        mod.get_mpz_t()
    );
    return result;
}

Int inth_root(Int num, unsigned long int n)
{
    // Sanity check here.
    if (n <= 0) {
        cerr << "[ ! ] Error in Calc.h: inth_root() Cannot take " << n << "th-root of \"" << num << "\"!" << endl;
        exit(MATH_ERROR_CODE);
    }
    
    Int result;
    mpz_root(
        result.get_mpz_t(),
        num.get_mpz_t(),
        n
    );
    return result;
}

Int inth_root(Int num, Int n)
{
    // Sanity check here.
    if (!n.fits_ulong_p()) {
        cerr << "[ ! ] Error in Calc.h: inth_root(): n=" << n << " is too big to fit in ulong." << endl;
        exit(MATH_ERROR_CODE);
    }
    if (n <= 0) {
        cerr << "[ ! ] Error in Calc.h: inth_root(): Cannot take " << n << "th-root of \"" << num << "\"!" << endl;
        exit(MATH_ERROR_CODE);
    }

    Int   result;
    ulong n_as_u_long = n.get_ui();
    mpz_root(
        result.get_mpz_t(),
        num.get_mpz_t(),
        n_as_u_long
    );
    return result;
}

Int isqrt(Int num)
{
    Int result;
    mpz_sqrt(
        result.get_mpz_t(),
        num.get_mpz_t()
    );
    return result;
}

Int legendreSymbol(Int x, Int p)
{
    // Could put a sanity check
    // to see if p an odd prime.
    // ...

    // Actual implementation
    return pow(x, (p-1)/2, p);
}

bool isQuadraticResidue(Int x, Int p)
{
    // Could put a sanity check
    // to see if p an odd prime.
    // ...
    
    Int L = legendreSymbol(x, p);
    if (L == 0 || L == 1)
        return true;
    return false;
}

Int inverse(Int x, Int p)
{
    Int result;
    if (!mpz_invert(
        result.get_mpz_t(),
        x.get_mpz_t(),
        p.get_mpz_t()
    )) {
        cerr << "[ ! ] Error in Calc.h: inverse(): inverse of \"" << x << "\" mod \"" << p << " does not exist." << endl;
        exit(MATH_ERROR_CODE);
    }
    return result;
}

Int mod(Int x, Int p)
{
    if (p == 0) {
        cerr << "[ ! ] Error in Calc.h: mod(): Try to mod 0..." << endl;
        exit(MATH_ERROR_CODE);
    }

    Int result;
    mpz_mod(
        result.get_mpz_t(),
        x.get_mpz_t(),
        p.get_mpz_t()
    );
    return result;
}

vector<Int> getBits(Int x) 
{
    Int bit;
    vector<Int> bits;
    while (x) {
        bit = x & 1;
        bits.push_back(bit);
        x >>= 1;
    }   
    return bits;
}