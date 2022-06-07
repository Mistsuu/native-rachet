#pragma once

#include "../Utils/Utils.h"
#include "Hash/SHA256.h"
#include "ProtocolSetting.h"

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
    // Buffer KDF()
    // {
    // }

    // Buffer calculateSecret(x3DHEncryptKeyBundleA ourKey, x3DHEncryptKeyBundleB theirKey)
    // {
    //     Buffer DH1 = curve.serialize(curve.calculateSharedSecret(ourKey.identityKey.privateKey,  theirKey.signedPreKey.publicKey));
    //     Buffer DH2 = curve.serialize(curve.calculateSharedSecret(ourKey.ephemeralKey.privateKey, theirKey.identityKey.publicKey));
    //     Buffer DH3 = curve.serialize(curve.calculateSharedSecret(ourKey.ephemeralKey.privateKey, theirKey.signedPreKey.publicKey));
    //     Buffer DH4 = curve.serialize(curve.calculateSharedSecret(ourKey.ephemeralKey.privateKey, theirKey.oneTimePreKey.publicKey));
    //     return KDF(DH1 + DH2 + DH3 + DH4).digest();
    // }

    // Buffer calculateAssociatedData(x3DHEncryptKeyBundleA ourKey, x3DHEncryptKeyBundleB theirKey)
    // {
    //     return curve.serialize(ourKey.identityKey.publicKey) + curve.serialize(theirKey.identityKey.publicKey);
    // }
};