#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// ----------------------------------------------------------------------------------------------------------\-
#include <gmpxx.h>
#include <iostream>
#include "Crypto/SignalProtocol.h"

using namespace std;

int main(int argc, char** argv)
{
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
    aliceMessage = proto.signalEncrypt(&aliceRachetState, &aliceHeader, Buffer("helloooo1"), aliceAssocData);
    aliceMessage = proto.signalEncrypt(&aliceRachetState, &aliceHeader, Buffer("helloooo2"), aliceAssocData);
    RachetHeader randomHeader(aliceHeader.publicKey, aliceHeader.prevChainLen, aliceHeader.iMess);
    aliceMessageRandom = aliceMessage;
    aliceMessage = proto.signalEncrypt(&aliceRachetState, &aliceHeader, Buffer("helloooo3"), aliceAssocData);
    aliceMessage = proto.signalEncrypt(&aliceRachetState, &aliceHeader, Buffer("helloooo4"), aliceAssocData);
    aliceMessage = proto.signalEncrypt(&aliceRachetState, &aliceHeader, Buffer("helloooo5"), aliceAssocData);
    RachetHeader randomHeader2(aliceHeader.publicKey, aliceHeader.prevChainLen, aliceHeader.iMess);
    aliceMessageRandom2 = aliceMessage;
    aliceMessage = proto.signalEncrypt(&aliceRachetState, &aliceHeader, Buffer("helloooo6"), aliceAssocData);


    // >>> bob: [decrypt message from alice]
    Buffer bobDecryptMessage;
    bobDecryptMessage = proto.signalDecrypt(&bobRachetState, aliceHeader, aliceMessage, bobAssocData);
    bobDecryptMessage.__debug__();
    cout << bobRachetState.skippedKeys.size() << endl;

    bobDecryptMessage = proto.signalDecrypt(&bobRachetState, randomHeader, aliceMessageRandom, bobAssocData);
    bobDecryptMessage.__debug__();
    cout << bobRachetState.skippedKeys.size() << endl;

    bobDecryptMessage = proto.signalDecrypt(&bobRachetState, randomHeader2, aliceMessageRandom2, bobAssocData);
    bobDecryptMessage.__debug__();
    cout << bobRachetState.skippedKeys.size() << endl;

    bobDecryptMessage = proto.rachetDecrypt(&bobRachetState, randomHeader2, aliceMessageRandom2, bobAssocData);
    bobDecryptMessage.__debug__();
    cout << bobRachetState.skippedKeys.size() << endl;

}
// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop