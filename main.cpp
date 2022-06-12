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
#include "Utils/Utils.h"
#include "Crypto/x3DH.h"

using namespace std;

int main(int argc, char** argv)
{
    x3DHClass x3DH;
        Buffer buf(1000);
        buf.randomAll();
        buf.__debug__();
}

// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop