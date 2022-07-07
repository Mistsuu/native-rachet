#pragma once

#include "Type.h"
#include "Buffer.h"
#include "Const.h"
#include <gmp.h>
#include <iostream>
#include <vector>

using namespace std;

// --------------------------------- PRIME CHECKER ---------------------------------
inline bool isPrime(Int num)
{
    return mpz_probab_prime_p(num.get_mpz_t(), _PRIME_CHECK_THRESHOLD);
}

// --------------------------------- BASIC ARITHMETICS ---------------------------------
inline int ceildiv(int x, int y)
{
    return x / y + (x % y != 0);
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


// ---------------------------------- FIND ROOTS ---------------------------------
Int inth_root(Int num, unsigned long int n)
{
    // Sanity check here.
    if (n <= 0) {
        std::cerr << "[ ! ] Error in Calc.h: inth_root() Cannot take " << n << "th-root of \"" << num << "\"!" << std::endl;
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
        std::cerr << "[ ! ] Error in Calc.h: inth_root(): n=" << n << " is too big to fit in ulong." << std::endl;
        exit(MATH_ERROR_CODE);
    }
    if (n <= 0) {
        std::cerr << "[ ! ] Error in Calc.h: inth_root(): Cannot take " << n << "th-root of \"" << num << "\"!" << std::endl;
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

// ------------------------------------ MOD P ------------------------------------
Int inverse(Int x, Int p)
{
    Int result;
    if (!mpz_invert(
        result.get_mpz_t(),
        x.get_mpz_t(),
        p.get_mpz_t()
    )) {
        std::cerr << "[ ! ] Error! Calc.h: inverse(): inverse of \"" << x << "\" mod \"" << p << " does not exist." << std::endl;
        exit(MATH_ERROR_CODE);
    }
    return result;
}

Int mod(Int x, Int p)
{
    if (p == 0) {
        std::cerr << "[ ! ] Error! Calc.h: mod(): Try to mod 0..." << std::endl;
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


// ---------------------------------- SQRT MOD X ----------------------------------
// -                    For now, assuming X is prime...                           -
// --------------------------------------------------------------------------------
bool isQuadraticResidue(Int x, Int p)
{
    if (!isPrime(p)) {
        std::cerr << "[ ! ] Error! Calc.h: legendreSymbol(Int x, Int p): non prime p is not supported yet!" << std::endl;
        std::cerr << "[ ! ] Debug: x: " << x << std::endl;
        std::cerr << "[ ! ] Debug: p: " << p << std::endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    if (mpz_legendre(x.get_mpz_t(), p.get_mpz_t()) == 1)
        return true;
    return false;
}

Int sqrt_mod(Int x, Int p)
{
    // Check prime :)
    if (!isPrime(p)) {
        std::cerr << "[ ! ] Error! Calc.h: sqrt_mod(Int x, Int p): non prime p is not supported yet!" << std::endl;
        std::cerr << "[ ! ] Debug: x: " << x << std::endl;
        std::cerr << "[ ! ] Debug: p: " << p << std::endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    // Check if x is a quadratic residue.
    if (!isQuadraticResidue(x, p)) {
        std::cerr << "[ ! ] Error! Calc.h: sqrt_mod(Int x, Int p): Cannot find sqrt(x) mod p!" << std::endl;
        std::cerr << "[ ! ] Debug: x: " << x << std::endl;
        std::cerr << "[ ! ] Debug: p: " << p << std::endl;
        exit(MATH_ERROR_CODE);
    }

    // Handle current cases
    if (p % 4 == 3)
        return sqrt_mod__p_3_mod_4__unsafe__(x, p);


    // Well.. not yet prepared
    std::cerr << "[ ! ] Error! Calc.h: sqrt_mod(Int x, Int p): Not implemented for this kind of p." << std::endl;
    std::cerr << "[ ! ] Debug: x: " << x << std::endl;
    std::cerr << "[ ! ] Debug: p: " << p << std::endl;
    exit(NOT_IMPLEMENTED_ERROR_CODE);
}


// ------------------------------------ GET BIT ARRAYS ------------------------------------
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