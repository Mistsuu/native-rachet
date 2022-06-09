#pragma once

#include "../Utils/Utils.h"
#include "Hash/SHA256.h"
#include "Hash/HKDF.h"
#include "x3DHProtocolSetting.h"

class x3DHPreKeyBundle
{
    KeyPair identityKey;                                 // Random key, generate one time.
    KeyPair signedPreKey;                                // Random key, generate one time.
    Buffer  signature;                                   // Generated from identity key and signedPreKey.
    KeyPair oneTimePreKeys[ONETIME_PREKEYS_BATCH_SIZE];  // Random key, use one time, generate if none.
};

class x3DHEncryptKeyBundleA
{
public:
    KeyPair identityKey;                                 // Random key, generate one time.
    KeyPair ephemeralKey;                                // Random key, generate each protocol run.
};

class x3DHEncryptKeyBundleB
{
public:
    KeyPair identityKey;
    KeyPair signedPreKey;
    Buffer  signature;
    KeyPair oneTimePreKey;
};

class x3DHClass {
public:
    static ProtocolCurve curve;
    Buffer KDF(Buffer keyMaterial)
    {
        return HKDF(
            32, 
            F + keyMaterial,
            Buffer(""),
            KDF_INFO
        );
    }

    KeyPair generateKeyPair()
    {
        KeyPair newKeyPair;
        newKeyPair.privateKey = randbelow(curve.generatorOrder());
        newKeyPair.publicKey  = curve.xMUL(curve.generatorPoint(), newKeyPair.privateKey);
        return newKeyPair;
    }

    Buffer sign(KeyPair signKey, Buffer message, Buffer randomData)
    {
        
    }

    Point calculateDHSharedSecret(Int ourPrivateKey, Point& theirPublicKey)
    {
        if (ourPrivateKey == PRIVATE_KEY_NULL) {
            cerr << "[ ! ] Error! x3DH.h: calculateDHSharedSecret(): Private key is empty!" << endl;
            exit(EMPTY_KEY_ERROR_CODE);
        }
        return curve.xMUL(theirPublicKey, ourPrivateKey);
    }

    Buffer calculateSharedSecret(x3DHEncryptKeyBundleA ourKey, x3DHEncryptKeyBundleB theirKey)
    {
        Buffer DH1 = curve.serialize(this->calculateDHSharedSecret(ourKey.identityKey.privateKey,  theirKey.signedPreKey.publicKey));
        Buffer DH2 = curve.serialize(this->calculateDHSharedSecret(ourKey.ephemeralKey.privateKey, theirKey.identityKey.publicKey));
        Buffer DH3 = curve.serialize(this->calculateDHSharedSecret(ourKey.ephemeralKey.privateKey, theirKey.signedPreKey.publicKey));
        Buffer DH4 = curve.serialize(this->calculateDHSharedSecret(ourKey.ephemeralKey.privateKey, theirKey.oneTimePreKey.publicKey));
        return KDF((DH1 + DH2) + (DH3 + DH4));
    }

    Buffer calculateAssociatedData(x3DHEncryptKeyBundleA ourKey, x3DHEncryptKeyBundleB theirKey)
    {
        return curve.serialize(ourKey.identityKey.publicKey) + curve.serialize(theirKey.identityKey.publicKey);
    }
};