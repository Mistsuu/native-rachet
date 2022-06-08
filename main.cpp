#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// ----------------------------------------------------------------------------------------------------------\-

#include <gmpxx.h>
#include <iostream>
#include "Utils/Utils.h"
#include "Crypto/x3DH.h"
#include "Crypto/Hash/HKDF.h"

using namespace std;

int main(int argc, char** argv)
{
    ProtocolCurve curve;
    curve.serialize(curve.generatorPoint()).__debug__();
    cout << curve.deserialize(curve.serialize(curve.generatorPoint())) << endl;

    // HKDF(
    //     42,
    //     Buffer::fromHex("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"),
    //     Buffer::fromHex("000102030405060708090a0b0c"),
    //     Buffer::fromHex("f0f1f2f3f4f5f6f7f8f9")
    // ).__debug__();

    // Buffer::fromHex("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b").__debug__();
    // Buffer::fromHex("000102030405060708090a0b0c").__debug__();
    // Buffer::fromHex("f0f1f2f3f4f5f6f7f8f9").__debug__();
}

// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop