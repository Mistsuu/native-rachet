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
    ProtocolCurve curve;
    cout << curve.edMUL(curve.generatorPointEdwards(), 10) << endl;
}

// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop