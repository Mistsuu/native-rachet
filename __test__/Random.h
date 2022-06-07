#pragma once

#include <gmpxx.h>
#include <fstream>
#include <cstdlib>
#include "Type.h"
#include "Buffer.h"
#include "Calc.h"

using namespace std;

Buffer urandom(uint size)
{
    Buffer buffer(size);

    ifstream urandomStream("/dev/urandom");
    urandomStream.read((char*)buffer.data(), size);
    urandomStream.close();

    return buffer;
}

Int getrandbits(uint nbits)
{
    uint   numBytes     = (nbits >> 3) + bool(nbits & 7);
    Buffer randomBytes  = urandom(numBytes);
    Int    randomNumber = 0;

    if (nbits & 7) {
        randomBytes[0] = randomBytes[0] >> (8 - (nbits & 7));
    }

    for (uint i = 0; i < numBytes; ++i) {
        randomNumber <<= 8;
        randomNumber +=  randomBytes[i];
    }

    return randomNumber;
}

uint bitLength(Int n)
{
    uint l = 0;
    while (n) {
        n >>= 1;
        l++;
    }
    return l;
}

Int randbelow(Int n)
{
    uint l = bitLength(n);
    return mod(getrandbits(l), n);
}

#define _PRIME_CHECK_THRESHOLD 40
Int getrandprime(uint nbits)
{
    Int randomNumber;
    do {
        randomNumber = getrandbits(nbits);
    } while (!mpz_probab_prime_p(randomNumber.get_mpz_t(), _PRIME_CHECK_THRESHOLD));
    return randomNumber;
}