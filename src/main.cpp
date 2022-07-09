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
        SignalProtocol proto;

        /* ==================================================================================================
                                                X3DH KEY EXCHANGE
        ================================================================================================*/
        x3DHPreKeyBundleA aliceKeyBundle;
        x3DHPreKeyBundleB bobKeyBundle;

        // >>> alice: [create key]
        aliceKeyBundle.identityKey = proto.generateKeyPair(); 
        aliceKeyBundle.ephemeralKey = proto.generateKeyPair(); 

        // >>> bob: [create key]
        bobKeyBundle.identityKey = proto.generateKeyPair();
        bobKeyBundle.signedPreKey = proto.generateKeyPair();
        bobKeyBundle.signature = proto.XEdDSA_sign(
                                    bobKeyBundle.identityKey.privateKey, 
                                    proto.serialize(bobKeyBundle.signedPreKey.publicKey)
                                );
        cout << "here" << endl;
        bobKeyBundle.oneTimePreKey = proto.generateKeyPair();
        

        // >>> alice -> bob: [her keys + onetime prekey she used]
        // ...
        // >>> alice: [calculate share secret]
        Buffer sharedAlice = proto.calculateSharedSecret(aliceKeyBundle, bobKeyBundle); 
        
        // >>> bob: [calculate share secret]
        Buffer sharedBob = proto.calculateSharedSecret(bobKeyBundle, aliceKeyBundle);

        sharedAlice.__debug__();
        sharedBob.__debug__();

    }   
    catch (NotImplementedException e) 
    {
        cout << e.what() << endl;
    }

    /*  ==================================================================================================

    ================================================================================================== */
}
// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop