#pragma once

#include <gmpxx.h>
#include <fstream>
#include <cstdlib>
#include "Type.h"
#include "Buffer.h"
#include "Calc.h"

Buffer urandom(uint size)
{
    Buffer buffer(size);

    FILE* urandomFilePtr = fopen("/dev/urandom", "rb");
    if (fread(buffer.data(), 1, size, urandomFilePtr) < size) {
        std::cerr << "[ ! ] Error! Random.h: urandom(uint size): Generate random buffer with size " << size << " failed." << std::endl;
        exit(BUFFER_ERROR_CODE);
    }
    fclose(urandomFilePtr);

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
    if (n < 0)
        n = -n;
    if (n == 0)
        return Int(0);
        
    uint l = bitLength(n);
    return getrandbits(l) % n;
}

Int getrandprime(uint nbits)
{
    Int randomNumber;
    do {
        randomNumber = getrandbits(nbits);
    } while (!isPrime(randomNumber));
    return randomNumber;
}