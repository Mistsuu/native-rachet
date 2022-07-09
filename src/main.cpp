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
#include "Crypto/SignalProtocol.h"

using namespace std;

int main(int argc, char** argv)
{
    SignalProtocol signalProto;

    RachetState state;
    KeyPair keyPair1 = signalProto.generateKeyPair();
    KeyPair keyPair2 = signalProto.generateKeyPair();
    signalProto.RachetInitAlice(
        &state,
        keyPair1.publicKey,
        keyPair2.publicKey
    );

}
// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop