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
    cout << curve.onCurve(PointMongomery(9)) << endl;
    cout << curve.onCurve(PointMongomery(123)) << endl;
    cout << curve.onCurve(PointMongomery(1234)) << endl;
    cout << curve.onCurve(PointMongomery(12345)) << endl;
    cout << curve.onCurve(PointMongomery(123456789)) << endl;
}

// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop