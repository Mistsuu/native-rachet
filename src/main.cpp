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
        bobKeyBundle.oneTimePreKey = proto.generateKeyPair();
        

        // >>> alice -> bob: [her keys + onetime prekey she used]
        // ...
        // >>> alice: [calculate share secret]
        Buffer sharedAlice = proto.calculateSharedSecret(aliceKeyBundle, bobKeyBundle); 
        
        // >>> bob: [calculate share secret]
        Buffer sharedBob = proto.calculateSharedSecret(bobKeyBundle, aliceKeyBundle);

        /*  ==================================================================================================
                                            VERIFY SIGN PREKEY...
         ================================================================================================== */

        /*  ==================================================================================================
                                                INIT RACHET
         ================================================================================================== */

        RachetState aliceRachetState;
        RachetState bobRachetState;

        // >>> alice: [init rachet]
        proto.rachetInitAlice(&aliceRachetState, sharedAlice, bobKeyBundle.signedPreKey.publicKey);
        Buffer aliceAssocData = proto.calculateAssociatedData(aliceKeyBundle, bobKeyBundle);

        // >>> bob: [init rachet]
        proto.rachetInitBob(&bobRachetState, sharedBob, bobKeyBundle.signedPreKey);
        Buffer bobAssocData = proto.calculateAssociatedData(aliceKeyBundle, bobKeyBundle);

        /*  ==================================================================================================
                                                SEND FIRST MESSAGES
         ================================================================================================== */
        
        RachetHeader aliceHeader;
        RachetHeader bobHeader;
        Buffer aliceMessage;
        Buffer aliceMessageRandom;
        Buffer aliceMessageRandom2;

        // >>> alice: [send initial message to bob]
        aliceMessage = proto.rachetEncrypt(&aliceRachetState, Buffer("helloooo1"), aliceAssocData, &aliceHeader);
        aliceMessage = proto.rachetEncrypt(&aliceRachetState, Buffer("helloooo2"), aliceAssocData, &aliceHeader);
        RachetHeader randomHeader(aliceHeader.publicKey, aliceHeader.prevChainLen, aliceHeader.iMess);
        aliceMessageRandom = aliceMessage;
        aliceMessage = proto.rachetEncrypt(&aliceRachetState, Buffer("helloooo3"), aliceAssocData, &aliceHeader);
        aliceMessage = proto.rachetEncrypt(&aliceRachetState, Buffer("helloooo4"), aliceAssocData, &aliceHeader);
        aliceMessage = proto.rachetEncrypt(&aliceRachetState, Buffer("helloooo5"), aliceAssocData, &aliceHeader);
        RachetHeader randomHeader2(aliceHeader.publicKey, aliceHeader.prevChainLen, aliceHeader.iMess);
        aliceMessageRandom2 = aliceMessage;
        aliceMessage = proto.rachetEncrypt(&aliceRachetState, Buffer("helloooo6"), aliceAssocData, &aliceHeader);



        // >>> bob: [decrypt message from alice]
        Buffer bobDecryptMessage = proto.rachetDecrypt(&bobRachetState, aliceHeader, aliceMessage, bobAssocData);
        bobDecryptMessage.__debug__();
        cout << bobRachetState.skippedKeys.size() << endl;

        bobDecryptMessage = proto.rachetDecrypt(&bobRachetState, randomHeader, aliceMessageRandom, bobAssocData);
        bobDecryptMessage.__debug__();
        cout << bobRachetState.skippedKeys.size() << endl;

        bobDecryptMessage = proto.rachetDecrypt(&bobRachetState, randomHeader2, aliceMessageRandom2, bobAssocData);
        bobDecryptMessage.__debug__();
        cout << bobRachetState.skippedKeys.size() << endl;

    }   
    catch (NotImplementedException e) 
    {
        cout << e.what() << endl;
    }
    catch (CryptFailException e)
    {
        cout << e.what() << endl;
    }

    
}
// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop