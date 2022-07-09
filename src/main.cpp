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
    try {
        ProtocolCrypt crypt;
        Buffer IV = urandom(16);
        Buffer key = urandom(32);
        Buffer message = Buffer("hello, this is to becrypted!");
        Buffer ciphertext = crypt.encrypt(IV, key, message);
        
        crypt.decrypt(IV, key, ciphertext).__debug__();
    } catch (CryptFailException e) {
        cout << e.what() << endl;
    }
    
}
// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop