#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// ----------------------------------------------------------------------------------------------------------\-

#include <gmpxx.h>
#include <iostream>
#include "Utils/Utils.h"
#include "Crypto/x3DH.h"
#include "Crypto/Hash/HMAC_SHA256.h"

using namespace std;

int main(int argc, char** argv)
{
    HMAC_SHA256().digest(Buffer("key"), Buffer("The quick brown fox jumps over the lazy dog")).__debug__();
}

// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop