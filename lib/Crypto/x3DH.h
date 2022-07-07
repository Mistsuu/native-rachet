#pragma once

#include "Utils/Utils.h"
#include "Hash/SHA256.h"
#include "Signing/HKDF.h"
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
    ProtocolCurve curve;

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
        newKeyPair.publicKey  = curve.xMUL(curve.generatorPointMongomery(), newKeyPair.privateKey);
        return newKeyPair;
    }


    // -------------------- Serialize data ----------------------
    inline Buffer serialize(PointMongomery P)
    {
        return curve.serialize(P); // output.len() == curve.curveSizeBytes()
    }

    inline Buffer serialize(Int num)
    {
        return Buffer::fromInt(num, curve.curveSizeBytes());
    }

    inline PointMongomery deserializePoint(Buffer pointBuffer)
    {
        return curve.deserialize(pointBuffer);
    }

    inline Int deserializeInt(Buffer intBuffer)
    {
        return intBuffer.toInt();
    }


    // -------------------- Hash functions --------------------
    inline Int hash(Buffer input)
    {
        return ProtocolHash(input).intdigest();
    }

    inline Int hash_i(uint i, Buffer input)
    {
        Buffer head = this->serialize((Int(1) << curve.curveBitLength()) - 1);
        return this->hash(head + input);
    }


    // -------------------- Signing functions --------------------
    PointEdwards calculateKeyPair(Int privateKey)
    {
        PointEdwards E = curve.edMUL(curve.generatorPointEdwards(), privateKey);
    }

    Buffer XEdDSA_sign(KeyPair signKey, Buffer message)
    {
        // Generate random buffer...
        Buffer randomData = urandom(64);
        
        // Serialize some data
        Buffer serializedPubKey = this->serialize(signKey.publicKey);
        Buffer serializedPrvKey = this->serialize(signKey.privateKey);
        
        // Multiply points
        Int    r = this->hash_i(1, serializedPrvKey + message + randomData) % curve.generatorOrder();
        Buffer R = this->serialize(curve.xMUL(curve.generatorPointMongomery(), r));
        Int    h = this->hash(R + serializedPubKey + message);
        Int    s = (r + h*signKey.privateKey) % curve.generatorOrder();

        return R + this->serialize(s);
    }


    PointMongomery calculateDHSharedSecret(Int ourPrivateKey, PointMongomery& theirPublicKey)
    {
        if (ourPrivateKey == PRIVATE_KEY_NULL) {
            std::cerr << "[ ! ] Error! x3DH.h: calculateDHSharedSecret(): Private key is empty!" << std::endl;
            exit(EMPTY_KEY_ERROR_CODE);
        }
        return curve.xMUL(theirPublicKey, ourPrivateKey);
    }

    Buffer calculateSharedSecret(x3DHEncryptKeyBundleA ourKey, x3DHEncryptKeyBundleB theirKey)
    {
        Buffer DH1 = this->serialize(this->calculateDHSharedSecret(ourKey.identityKey.privateKey,  theirKey.signedPreKey.publicKey));
        Buffer DH2 = this->serialize(this->calculateDHSharedSecret(ourKey.ephemeralKey.privateKey, theirKey.identityKey.publicKey));
        Buffer DH3 = this->serialize(this->calculateDHSharedSecret(ourKey.ephemeralKey.privateKey, theirKey.signedPreKey.publicKey));
        Buffer DH4 = this->serialize(this->calculateDHSharedSecret(ourKey.ephemeralKey.privateKey, theirKey.oneTimePreKey.publicKey));
        return KDF((DH1 + DH2) + (DH3 + DH4));
    }

    Buffer calculateAssociatedData(x3DHEncryptKeyBundleA ourKey, x3DHEncryptKeyBundleB theirKey)
    {
        return this->serialize(ourKey.identityKey.publicKey) + this->serialize(theirKey.identityKey.publicKey);
    }
};