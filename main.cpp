#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// ----------------------------------------------------------------------------------------------------------\-

#include <gmpxx.h>
#include <iostream>
#include "Utils/Utils.h"
#include "Crypto/x3DH.h"

using namespace std;

int main(int argc, char** argv)
{
    ProtocolCurve curve;
    curve.deserialize(curve.serialize(curve.generatorPoint())).__debug__();
}

// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop