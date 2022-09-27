#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// ----------------------------------------------------------------------------------------------------------\-
#include <gmpxx.h>
#include <iostream>
#include <vector>
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
    Buffer aliceAssocData = proto.calculateAssociatedData(aliceKeyBundle.identityKey, bobKeyBundle.identityKey);

    // >>> bob: [init rachet]
    proto.rachetInitBob(&bobRachetState, sharedBob, bobKeyBundle.signedPreKey);
    Buffer bobAssocData = proto.calculateAssociatedData(aliceKeyBundle.identityKey, bobKeyBundle.identityKey);

    /*  ==================================================================================================
                                            SEND FIRST MESSAGES
        ================================================================================================== */
    
    vector<Buffer> alicePlaintexts = {
                        Buffer("alice0"),
                        Buffer("alice1"), 
                        Buffer("alice2"), 
                        Buffer("alice3"), 
                        Buffer("alice4") 
                    };

    vector<Buffer> bobPlaintexts = {
                        Buffer("bob0"),
                        Buffer("bob1"),
                        Buffer("bob2")
                    };

    // >>> alice: [send initial message to bob]
    Buffer aliceCiphertext;
    RachetHeader aliceHeader;
    vector<Buffer> aliceCiphertexts;
    vector<RachetHeader> aliceHeaders;
    for (int i = 0; i < alicePlaintexts.size(); ++i) {
        aliceCiphertext = proto.signalEncrypt(&aliceRachetState, &aliceHeader, alicePlaintexts[i], aliceAssocData);
        aliceHeaders.push_back(aliceHeader);
        aliceCiphertexts.push_back(aliceCiphertext);
    }

    // >>> bob: [decrypt message from alice]
    Buffer bobDecryptedCiphertext;
    bobDecryptedCiphertext = proto.signalDecrypt(&bobRachetState, aliceHeaders[4], aliceCiphertexts[4], bobAssocData); bobDecryptedCiphertext.__debug__();
    bobDecryptedCiphertext = proto.signalDecrypt(&bobRachetState, aliceHeaders[1], aliceCiphertexts[1], bobAssocData); bobDecryptedCiphertext.__debug__();
    bobDecryptedCiphertext = proto.signalDecrypt(&bobRachetState, aliceHeaders[2], aliceCiphertexts[2], bobAssocData); bobDecryptedCiphertext.__debug__();
    bobDecryptedCiphertext = proto.signalDecrypt(&bobRachetState, aliceHeaders[3], aliceCiphertexts[3], bobAssocData); bobDecryptedCiphertext.__debug__();
    bobDecryptedCiphertext = proto.signalDecrypt(&bobRachetState, aliceHeaders[3], aliceCiphertexts[3], bobAssocData); bobDecryptedCiphertext.__debug__();
    bobDecryptedCiphertext = proto.signalDecrypt(&bobRachetState, aliceHeaders[0], aliceCiphertexts[0], bobAssocData); bobDecryptedCiphertext.__debug__();


    // >>> bob: [sends messages to alice]
    Buffer bobCiphertext;
    RachetHeader bobHeader;
    vector<Buffer> bobCiphertexts;
    vector<RachetHeader> bobHeaders;
    for (int i = 0; i < bobPlaintexts.size(); ++i) {
        bobCiphertext = proto.signalEncrypt(&bobRachetState, &bobHeader, bobPlaintexts[i], bobAssocData);
        bobHeaders.push_back(bobHeader);
        bobCiphertexts.push_back(bobCiphertext);
    }

    // >>> alice: [decrypt message from bob]
    Buffer aliceDecryptedCiphertext;
    aliceDecryptedCiphertext = proto.signalDecrypt(&aliceRachetState, bobHeaders[2], bobCiphertexts[2], aliceAssocData); aliceDecryptedCiphertext.__debug__();
    aliceDecryptedCiphertext = proto.signalDecrypt(&aliceRachetState, bobHeaders[1], bobCiphertexts[1], aliceAssocData); aliceDecryptedCiphertext.__debug__();
    aliceDecryptedCiphertext = proto.signalDecrypt(&aliceRachetState, bobHeaders[0], bobCiphertexts[0], aliceAssocData); aliceDecryptedCiphertext.__debug__();
    aliceDecryptedCiphertext = proto.signalDecrypt(&aliceRachetState, bobHeaders[1], bobCiphertexts[1], aliceAssocData); aliceDecryptedCiphertext.__debug__();

}
// ----------------------------------------------------------------------------------------------------------/-

#pragma GCC diagnostic pop