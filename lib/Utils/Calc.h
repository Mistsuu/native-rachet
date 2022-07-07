#pragma once

#include "Type.h"
#include "Buffer.h"
#include "Const.h"

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
        std::stringstream errorStream;
        errorStream << "[ ! ] Error in Calc.h: inth_root() Cannot take " << n << "th-root of \"" << num << "\"!" << std::endl;
        throw MathErrorException(errorStream.str());
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
        std::stringstream errorStream;
        errorStream << "[ ! ] Error in Calc.h: inth_root(): n=" << n << " is too big to fit in ulong." << std::endl;
        throw MathErrorException(errorStream.str());
    }
    if (n <= 0) {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error in Calc.h: inth_root(): Cannot take " << n << "th-root of \"" << num << "\"!" << std::endl;
        throw MathErrorException(errorStream.str());
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
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! Calc.h: inverse(): inverse of \"" << x << "\" mod \"" << p << " does not exist." << std::endl;
        throw MathErrorException(errorStream.str());
    }
    return result;
}

Int mod(Int x, Int p)
{
    if (p == 0) {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! Calc.h: mod(): Try to mod 0..." << std::endl;
        throw MathErrorException(errorStream.str());
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
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! Calc.h: legendreSymbol(Int x, Int p): non prime p is not supported yet!" << std::endl;
        errorStream << "[ ! ] Debug: x: " << x << std::endl;
        errorStream << "[ ! ] Debug: p: " << p << std::endl;
        throw NotImplementedException(errorStream.str());
    }

    if (mpz_legendre(x.get_mpz_t(), p.get_mpz_t()) == 1)
        return true;
    return false;
}

Int sqrt_mod__p_3_mod_4__unsafe__(Int x, Int p)
{
    std::stringstream errorStream;
    errorStream << "[ ! ] Error! Calc.h: sqrt_mod__p_3_mod_4__unsafe__(Int x, Int p): Square root mod p where p=3 mod 4 is not support yet!" << std::endl;
    errorStream << "[ ! ] Debug: x: " << x << std::endl;
    errorStream << "[ ! ] Debug: p: " << p << std::endl;
    throw NotImplementedException(errorStream.str());
    return Int();
}

Int sqrt_mod__p_5_mod_8__unsafe__(Int x, Int p)
{
    Int result = pow(x, (p+3)/8, p);
    if (pow(result, 2, p) == x) 
        return result;

    result *= pow(2, (p-1)/4, p);
    result  = mod(result, p);
    if (pow(result, 2, p) == x)
        return result;

    std::stringstream errorStream;
    errorStream << "[ ! ] Error! Calc.h: sqrt_mod__p_5_mod_8__unsafe__(Int x, Int p): Square root x mod p where p=5 mod 8 does not not exist." << std::endl;
    errorStream << "[ ! ] Debug: x: " << x << std::endl;
    errorStream << "[ ! ] Debug: p: " << p << std::endl;
    throw MathErrorException(errorStream.str());
    return Int();
}

Int sqrtMod(Int x, Int p)
{
    // Check prime :)
    if (!isPrime(p)) {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! Calc.h: sqrt_mod(Int x, Int p): Non-prime p is not supported yet!" << std::endl;
        errorStream << "[ ! ] Debug: x: " << x << std::endl;
        errorStream << "[ ! ] Debug: p: " << p << std::endl;
        throw NotImplementedException(errorStream.str());
    }

    // Check if x is a quadratic residue.
    if (!isQuadraticResidue(x, p)) {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! Calc.h: sqrt_mod(Int x, Int p): Cannot find sqrt(x) mod p!" << std::endl;
        errorStream << "[ ! ] Debug: x: " << x << std::endl;
        errorStream << "[ ! ] Debug: p: " << p << std::endl;
        throw MathErrorException(errorStream.str());
    }

    // Handle current cases
    if (p % 4 == 3)
        return sqrt_mod__p_3_mod_4__unsafe__(mod(x, p), p);
    else if (p % 8 == 5)
        return sqrt_mod__p_5_mod_8__unsafe__(mod(x, p), p);

    // Well.. not yet prepared
    std::stringstream errorStream;
    errorStream << "[ ! ] Error! Calc.h: sqrt_mod(Int x, Int p): Not implemented for this kind of p." << std::endl;
    errorStream << "[ ! ] Debug: x: " << x << std::endl;
    errorStream << "[ ! ] Debug: p: " << p << std::endl;
    throw NotImplementedException(errorStream.str());
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