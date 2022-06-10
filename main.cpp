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
    Curve25519 curve;
    cout << curve.generatorOrder() << endl;
}

// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop