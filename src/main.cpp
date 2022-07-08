#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// ----------------------------------------------------------------------------------------------------------\-
#include <chrono>
#define measureTime( tstFunc ) do {                                                 \
    auto start = chrono::high_resolution_clock::now();                              \
    tstFunc                                                                         \
    auto stop  = chrono::high_resolution_clock::now();                              \
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);      \
    std::cerr<< "[] Time taken to run: " << duration.count() << "us" << std::endl;  \
} while(0)

#include <gmpxx.h>
#include <iostream>
#include "Crypto/x3DH.h"

using namespace std;

int main(int argc, char** argv)
{
    x3DHClass      x3DH;
    Int            privateKey = randbelow(Int("100000000000000000"));
    Buffer         signature = x3DH.XEdDSA_sign(privateKey, Buffer("Nonanonananona"));
    PointMongomery publicKey = x3DH.curve.xMUL(x3DH.curve.generatorPointMongomery(), privateKey);
    cout << "[ i ] Signature:\n"; signature.__debug__(); cout << "\n";
    cout << "[ i ] PublicKey:\n"; cout << publicKey << "\n\n";
    cout << "[ i ] Verify:\n";
    cout << x3DH.XEdDSA_verify(x3DH.serialize(publicKey), Buffer("Nonanonananona"), signature) << endl;

}
// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop