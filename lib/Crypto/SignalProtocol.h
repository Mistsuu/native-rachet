#pragma once

#include "Utils/Utils.h"
#include "Hash/SHA512.h"
#include "SignalProtocolSettings.h"

class x3DHPreKeyBundle
{
    KeyPair identityKey;                                 // Random key, generate one time.
    KeyPair signedPreKey;                                // Random key, generate one time.
    Buffer  signature;                                   // Generated from identity key and signedPreKey.
    KeyPair oneTimePreKeys[ONETIME_PREKEYS_BATCH_SIZE];  // Random key, use one time, generate if none.
};

class x3DHPreKeyBundleA
{
public:
    KeyPair identityKey;                                 // Random key, generate one time.
    KeyPair ephemeralKey;                                // Random key, generate each protocol run.
};

class x3DHPreKeyBundleB
{
public:
    KeyPair identityKey;
    KeyPair signedPreKey;
    Buffer  signature;
    KeyPair oneTimePreKey;
};

class SkippedKeyNode
{
public:
    PointMongomery DHPublic;
    Int            iMess;

    SkippedKeyNode(PointMongomery DHPublic, Int iMess) 
    {
        this->DHPublic = DHPublic;
        this->iMess = iMess;
    }
};

class RachetState
{
public:
    KeyPair        DHSend;
    PointMongomery DHRecv;
    Buffer         rootKey;
    Buffer         chainKeySend, chainKeyRecv;
    Int            iMessSend, iMessRecv;
    Int            prevChainLen;
    std::vector<
        std::pair<SkippedKeyNode, Buffer>
    > skippedKeys;
};

class RachetHeader
{
public:
    PointMongomery publicKey;
    Int            prevChainLen;
    Int            iMess;

    RachetHeader(PointMongomery publicKey, Int prevChainLen, Int iMess)
    {
        this->publicKey = publicKey;
        this->prevChainLen = prevChainLen;
        this->iMess = iMess;
    }

    RachetHeader() {}
};

class SignalProtocol {
public:
    ProtocolCurve curve;

    // -------------------- KDF functions -----------------------
    Buffer HKDF(int length, Buffer inputKeyMaterial, Buffer salt, Buffer info)
    {
        ProtocolHMAC HMACObj;

        if (salt.len() == 0)
            salt = Buffer('\x00') * HMACObj.outputSize;
            
        Buffer prk = HMACObj.digest(salt, inputKeyMaterial);
        Buffer tmp = Buffer();
        Buffer outputKeyMaterial = Buffer();
        for (int i = 0; i < ceildiv(length, HMACObj.outputSize); ++i) {
            tmp = HMACObj.digest(prk, tmp + info + Buffer((char)(i+1)));
            outputKeyMaterial += tmp;
        }

        return outputKeyMaterial[{length}];
    }

    Buffer KDF(Buffer keyMaterial)
    {
        return this->HKDF(
            32, 
            F + keyMaterial,
            Buffer(""),
            KDF_INFO_SIGN
        );
    }

    Buffer updateRootKey(Buffer* rootKey, Buffer sharedDHInput)
    {
        Buffer hashOutput = this->HKDF(
            64, 
            sharedDHInput,
            *rootKey,
            KDF_INFO_UPDATE_ROOTKEY
        );

        (*rootKey) = hashOutput[{32}];
        return hashOutput[{-32}];
    }

    Buffer updateChainKey(Buffer* chainKey)
    {
        Buffer newMessageKey = ProtocolHMAC()
                                .digest(
                                    *chainKey, 
                                    HMAC_INPUT_GET_MESSAGEKEY
                                )[{32}];

        (*chainKey) = ProtocolHMAC()
                        .digest(
                            *chainKey, 
                            HMAC_INPUT_UPDATE_CHAINKEY
                        )[{32}];

        return newMessageKey;
    }

    // -------------------- Key generator -----------------------
    KeyPair generateKeyPair()
    {
        KeyPair newKeyPair;
        newKeyPair.privateKey = randbelow(curve.generatorOrder());
        newKeyPair.publicKey  = curve.xMUL(curve.generatorPointMongomery(), newKeyPair.privateKey);
        return newKeyPair;
    }


    // -------------------- Serialize data ----------------------
    inline Buffer serialize(PointEdwards P)
    {
        // Check if valid
        if (!curve.onCurve(P)) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: serialize(): Serializing PointEdwards P not on curve!" << std::endl;
            errorStream << "[ ! ]     P: " << P << std::endl;
            throw InvalidPointException(errorStream.str());
        }

        Buffer serializedPoint = Buffer::fromInt(P.y, curve.curveSizeBytes());
        serializedPoint[0] ^= P.s << 7;
        return serializedPoint;
    }

    inline Buffer serialize(PointMongomery P)
    {
        // Check if valid
        if (!curve.onCurve(P)) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: serialize(): Serializing PointMongomery P not on curve!" << std::endl;
            errorStream << "[ ! ]     P: " << P << std::endl;
            throw InvalidPointException(errorStream.str());
        }

        // Normalize point
        curve.normalize(&P);

        // ... and now serialize :)
        Buffer serializedPoint = Buffer::fromInt(P.x, curve.curveSizeBytes());
        serializedPoint[0] ^= (P.z == 1 ? 1 : 0) << 7;
        return serializedPoint;
    }

    inline Buffer serialize(Int num)
    {
        return Buffer::fromInt(num, curve.curveSizeBytes());
    }

    inline Buffer serialize(RachetHeader header)
    {
        return this->serialize(header.publicKey) + Buffer::fromInt(header.prevChainLen, 4) + Buffer::fromInt(header.iMess, 4);
    }

    inline PointEdwards deserializeEdwardPoint(Buffer edwardPointBuffer)
    {
        if (edwardPointBuffer.len() != curve.curveSizeBytes()) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: deserializeEdwardPoint(): Cannot deserialize the buffer! Wrong length! Got " << edwardPointBuffer.len() << " instead of " << curve.curveSizeBytes() << "!" << std::endl;
            errorStream << "[ ! ]     Buffer (in hex): " << edwardPointBuffer.toHex() << std::endl;
            throw DeserializeErrorException(errorStream.str());
        }

        // Parse point
        PointEdwards parsedEdwardPoint;
        parsedEdwardPoint.s   = edwardPointBuffer[0] >> 7;
        edwardPointBuffer[0] &= 0x7f;
        parsedEdwardPoint.y   = edwardPointBuffer.toInt();

        // Check if valid
        if (!curve.onCurve(parsedEdwardPoint)) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: deserializeEdwardPoint(): Deserialize buffer leads to point not on curve!" << std::endl;
            errorStream << "[ ! ]     Buffer (in hex): " << edwardPointBuffer.toHex() << std::endl;
            errorStream << "[ ! ]     Parsed point: "    << parsedEdwardPoint         << std::endl;
            throw InvalidPointException(errorStream.str());
        }

        // ... then return!!
        return parsedEdwardPoint;
    }

    inline PointMongomery deserializeMongomeryPoint(Buffer mongomeryPointBuffer)
    {
        if (mongomeryPointBuffer.len() != curve.curveSizeBytes()) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: deserializeMongomeryPoint(): Cannot deserialize the buffer! Wrong length! Got " << mongomeryPointBuffer.len() << " instead of " << curve.curveSizeBytes() << "!" << std::endl;
            errorStream << "[ ! ]     Buffer (in hex): " << mongomeryPointBuffer.toHex() << std::endl;
            throw DeserializeErrorException(errorStream.str());
        }

        // Parse point
        PointMongomery parsedMongomeryPoint;
        parsedMongomeryPoint.z   = mongomeryPointBuffer[0] >> 7;
        mongomeryPointBuffer[0] &= 0x7f;
        parsedMongomeryPoint.x   = mongomeryPointBuffer.toInt();

        // Check if valid
        if (!curve.onCurve(parsedMongomeryPoint)) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: deserializeMongomeryPoint(): Deserialize buffer leads to point not on curve!" << std::endl;
            errorStream << "[ ! ]     Buffer (in hex): " << mongomeryPointBuffer.toHex() << std::endl;
            errorStream << "[ ! ]     Parsed point: "    << parsedMongomeryPoint         << std::endl;
            throw InvalidPointException(errorStream.str());
        }

        // ... then return!!
        return parsedMongomeryPoint;
    }

    inline Int deserializeInt(Buffer intBuffer)
    {
        return intBuffer.toInt();
    }

    inline RachetHeader deserializeRachetHeader(Buffer headerBuffer)
    {
        if (headerBuffer.len() != curve.curveSizeBytes() + 8) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: deserializeMongomeryPoint(): Cannot deserialize the buffer! Wrong length! Got " << headerBuffer.len() << " instead of " << curve.curveSizeBytes() + 8 << "!" << std::endl;
            errorStream << "[ ! ]     Buffer (in hex): " << headerBuffer.toHex() << std::endl;
            throw DeserializeErrorException(errorStream.str());
        }

        return RachetHeader(
            this->deserializeMongomeryPoint(headerBuffer[{(int)curve.curveSizeBytes()}]),
            headerBuffer[{-8, -4}].toInt(),
            headerBuffer[{    -4}].toInt()
        );
    }


    // -------------------- Hash functions --------------------
    inline Int hash(Buffer input)
    {
        return SHA512Hash(input).intdigest();
    }

    inline Int hash_i(uint i, Buffer input)
    {
        Buffer head = this->serialize((Int(1) << curve.curveBitLength()) - 1);
        return this->hash(head + input);
    }


    // -------------------- Signing functions --------------------
    PointEdwards calculateKeyPair(Int& privateKey)
    {
        PointEdwards edwardPublicFull  = curve.edMUL(curve.generatorPointEdwards(), privateKey);
        PointEdwards edwardPublicTrunc = PointEdwards(edwardPublicFull.y, 0);

        if (edwardPublicFull.s == 1)
            privateKey = mod(-privateKey, curve.generatorOrder());
        else
            privateKey = mod( privateKey, curve.generatorOrder());
        
        return edwardPublicTrunc;
    }

    Buffer XEdDSA_sign(Int mongomeryPrivateKey, Buffer message)
    {
        // Generate random buffer...
        Buffer randomData = urandom(64);
        
        // Do signing...
        PointEdwards A = this->calculateKeyPair(mongomeryPrivateKey);
        Int          r = this->hash_i(1, this->serialize(mongomeryPrivateKey) + message + randomData);
        PointEdwards R = curve.edMUL(curve.generatorPointEdwards(), r);
        Int          h = this->hash(this->serialize(R) + this->serialize(A) + message);
        Int          s = mod((r + h*mongomeryPrivateKey), curve.generatorOrder());

        // Return serialized buffer...
        return this->serialize(R) + this->serialize(s);
    }

    bool XEdDSA_verify(Buffer mongomeryPublicKey, Buffer message, Buffer signature)
    {
        try {
            // Throw exception here.
            if (signature.len() != curve.curveSizeBytes() * 2) {
                std::stringstream errorStream;
                errorStream << "[ ! ] Error! SignalProtocol.h: XEdDSA_verify(): Signature is in wrong length! Got " << signature.len() << " instead of " << curve.curveSizeBytes() * 2 << "!" << std::endl;
                errorStream << "[ ! ]     Buffer (in hex): " << signature.toHex() << std::endl;
                throw DeserializeErrorException(errorStream.str());
            }

            // Verify part.
            PointMongomery U                  = this->deserializeMongomeryPoint(mongomeryPublicKey);
            Buffer         Rserialize         = signature[{(int)curve.curveSizeBytes()}];
            PointEdwards   R                  = this->deserializeEdwardPoint(Rserialize);
            Int            s                  = this->deserializeInt(signature[{-(int)curve.curveSizeBytes()}]);
            PointEdwards   A                  = curve.mongomeryToEdwards(U);
            Int            h                  = mod(this->hash(Rserialize + this->serialize(A) + message), curve.generatorOrder());
            PointEdwards   sB                 = curve.edMUL(curve.generatorPointEdwards(), s);
            PointEdwards   hA                 = curve.edMUL(A, h); curve.edNEG(&hA);
            PointEdwards   Rverify            = curve.edADD(sB, hA);
            if (Rverify.y == R.y && Rverify.s == R.s)
                return true;
            return false;

        } catch (InvalidPointException e) {
            return false;
        } catch (DeserializeErrorException e) {
            return false;
        }
        return false;
    }


    // --------------------- Encryption!! --------------------------
    Buffer innerEncrypt(Buffer messageKey, Buffer plaintext, Buffer associatedData)
    {
        Buffer encryptMaterial = this->HKDF(
                                    80,
                                    messageKey,
                                    Buffer(),
                                    KDF_INFO_GET_ENCRYPTKEY
                                 );

        Buffer encryptKey = encryptMaterial[{32}];
        Buffer authKey    = encryptMaterial[{32, 64}];
        Buffer IV         = encryptMaterial[{-16}];
        Buffer ciphertext = ProtocolCrypt().encrypt(IV, encryptKey, plaintext);
        Buffer authData   = ProtocolHMAC().digest(authKey, associatedData);
        return associatedData + ciphertext + authData; 
    }

    Buffer innerDecrypt(Buffer messageKey, Buffer ciphertext, Buffer associatedData)
    {
        Buffer encryptMaterial = this->HKDF(
                                    80,
                                    messageKey,
                                    Buffer(),
                                    KDF_INFO_GET_ENCRYPTKEY
                                 );

        Buffer encryptKey = encryptMaterial[{32}];
        Buffer authKey    = encryptMaterial[{32, 64}];
        Buffer IV         = encryptMaterial[{-16}];

        Buffer theirAssocData = ciphertext[{ (int)(curve.curveSizeBytes() * 3 + 8) }];
        Buffer ourAuthData    = ProtocolHMAC().digest(authKey, theirAssocData);
        Buffer theirAuthData  = ciphertext[{ -(int)ProtocolHMAC().outputSize }];
        if (ourAuthData != theirAuthData) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: innerDecrypt(): AuthData not match!" << std::endl;
            errorStream << "[ ! ]     IV               (in hex): " << IV.toHex() << std::endl;
            errorStream << "[ ! ]     authKey          (in hex): " << authKey.toHex() << std::endl;
            errorStream << "[ ! ]     encryptKey       (in hex): " << encryptKey.toHex() << std::endl;
            errorStream << "[ ! ]     messageKey       (in hex): " << messageKey.toHex() << std::endl;
            errorStream << "[ ! ]     ourAssocData     (in hex): " << associatedData.toHex() << std::endl;
            errorStream << "[ ! ]     theirAssocData   (in hex): " << theirAssocData.toHex() << std::endl;
            errorStream << "[ ! ]     ourAuthData      (in hex): " << ourAuthData.toHex() << std::endl;
            errorStream << "[ ! ]     theirAuthData    (in hex): " << theirAuthData.toHex() << std::endl;
            throw CryptFailException(errorStream.str());
        }

        ciphertext = ciphertext[{ (int)theirAssocData.len(), -(int)ourAuthData.len() }];
        return ProtocolCrypt().decrypt(IV, encryptKey, ciphertext);
    }

    // -------------------- Key-exchange functions --------------------
    PointMongomery calculateDHSharedSecret(Int ourPrivateKey, PointMongomery theirPublicKey)
    {
        if (ourPrivateKey == PRIVATE_KEY_NULL) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: calculateDHSharedSecret(): Private key is empty!" << std::endl;
            throw KeyErrorException(errorStream.str());
        }
        return curve.xMUL(theirPublicKey, ourPrivateKey);
    }

    Buffer calculateSharedSecret(x3DHPreKeyBundleA keyAlice, x3DHPreKeyBundleB keyBob)
    {
        Buffer DH1 = this->serialize(this->calculateDHSharedSecret(keyAlice.identityKey.privateKey,  keyBob.signedPreKey.publicKey));
        Buffer DH2 = this->serialize(this->calculateDHSharedSecret(keyAlice.ephemeralKey.privateKey, keyBob.identityKey.publicKey));
        Buffer DH3 = this->serialize(this->calculateDHSharedSecret(keyAlice.ephemeralKey.privateKey, keyBob.signedPreKey.publicKey));
        Buffer DH4 = this->serialize(this->calculateDHSharedSecret(keyAlice.ephemeralKey.privateKey, keyBob.oneTimePreKey.publicKey));
        return KDF((DH1 + DH2) + (DH3 + DH4));
    }

    Buffer calculateSharedSecret(x3DHPreKeyBundleB keyBob, x3DHPreKeyBundleA keyAlice)
    {
        Buffer DH1 = this->serialize(this->calculateDHSharedSecret(keyBob.signedPreKey.privateKey,  keyAlice.identityKey.publicKey));
        Buffer DH2 = this->serialize(this->calculateDHSharedSecret(keyBob.identityKey.privateKey,   keyAlice.ephemeralKey.publicKey));
        Buffer DH3 = this->serialize(this->calculateDHSharedSecret(keyBob.signedPreKey.privateKey,  keyAlice.ephemeralKey.publicKey));
        Buffer DH4 = this->serialize(this->calculateDHSharedSecret(keyBob.oneTimePreKey.privateKey, keyAlice.ephemeralKey.publicKey));
        return KDF((DH1 + DH2) + (DH3 + DH4));
    }

    Buffer calculateAssociatedData(x3DHPreKeyBundleA keyAlice, x3DHPreKeyBundleB keyBob)
    {
        return this->serialize(keyAlice.identityKey.publicKey) + this->serialize(keyBob.identityKey.publicKey);
    }


    // -------------------- Rachet-encryption functions -----------------------
    void rachetInitAlice(RachetState* state, Buffer sharedSecret, PointMongomery bobPublicKey)
    {
        if (state) {
            state->DHSend       = this->generateKeyPair();
            state->DHRecv       = bobPublicKey;
            state->rootKey      = sharedSecret;
            state->chainKeyRecv = Buffer("");
            state->iMessSend    = 0;
            state->iMessRecv    = 0;
            state->prevChainLen = 0;
            state->skippedKeys  = {};
            state->chainKeySend = this->updateRootKey(
                                    &state->rootKey, 
                                    this->serialize(this->calculateDHSharedSecret(state->DHSend.privateKey, state->DHRecv))
                                );
        }
    }

    void rachetInitBob(RachetState* state, Buffer sharedSecret, KeyPair bobKeyPair)
    {
        if (state) {
            state->DHSend       = bobKeyPair;
            state->DHRecv       = PointMongomery::nullPoint();
            state->rootKey      = sharedSecret;
            state->chainKeySend = Buffer(""); 
            state->chainKeyRecv = Buffer("");
            state->iMessSend    = 0; 
            state->iMessRecv    = 0; 
            state->prevChainLen = 0;
            state->skippedKeys  = {}; 
        }
    }

    Buffer rachetEncrypt(RachetState* state, RachetHeader* header, Buffer plaintext, Buffer associatedData)
    {
        if (state && header) {
            Buffer messageKey = this->updateChainKey(&state->chainKeySend);
            *header = RachetHeader(
                        state->DHSend.publicKey, 
                        state->prevChainLen, 
                        state->iMessSend
                      );
            state->iMessSend += 1;
            return this->innerEncrypt(messageKey, plaintext, associatedData + this->serialize(*header));
        } 
        
        else {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: rachetEncrypt(): NULL pointers state & header detected!" << std::endl;
            throw CryptFailException(errorStream.str());
            return Buffer();
        }
    }

    Buffer tryDecryptBySkippedKeys(RachetState* state, RachetHeader header, Buffer ciphertext, Buffer associatedData, bool* success)
    {
        for (int i = 0; i < state->skippedKeys.size(); ++i) {
            SkippedKeyNode node = SkippedKeyNode(
                state->skippedKeys[i].first.DHPublic,
                state->skippedKeys[i].first.iMess                
            );

            if (curve.comparePoint(node.DHPublic, state->DHRecv) && node.iMess == header.iMess) {
                Buffer messageKey = state->skippedKeys[i].second; 
                state->skippedKeys.erase(state->skippedKeys.begin() + i);
                *success = true;
                return this->innerDecrypt(messageKey, ciphertext, associatedData + this->serialize(header));
            }
        }

        *success = false;
        return Buffer();
    }

    void trySkipMessageKeys(RachetState* state, Int iMess)
    {
        if (state->iMessRecv + MAX_SKIP_MESSAGE_KEYS < iMess) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: trySkipMessageKeys(): iMess overflow! Got " << iMess << "! The upper index is " << state->iMessRecv + MAX_SKIP_MESSAGE_KEYS << "." << std::endl;
            errorStream << "[ ! ]     iMess: " << iMess << std::endl;
            throw CryptFailException(errorStream.str());
        }

        if (state->chainKeyRecv.len() != 0) {
            Buffer messageKey;
            while (state->iMessRecv < iMess) {
                messageKey = this->updateChainKey(&state->chainKeyRecv);
                state->skippedKeys.push_back(
                                      std::make_pair(
                                        SkippedKeyNode(state->DHRecv, state->iMessRecv),
                                        messageKey
                                      )
                                   );
                state->iMessRecv++;
            }
        }
    }

    void dhRachet(RachetState* state, RachetHeader header)
    {
        state->prevChainLen = state->iMessRecv;
        state->iMessSend    = 0;
        state->iMessRecv    = 0;
        state->DHRecv       = header.publicKey;
        state->chainKeyRecv = this->updateRootKey(&state->rootKey, this->serialize(this->calculateDHSharedSecret(state->DHSend.privateKey, state->DHRecv)));
        state->DHSend       = this->generateKeyPair();
        state->chainKeySend = this->updateRootKey(&state->rootKey, this->serialize(this->calculateDHSharedSecret(state->DHSend.privateKey, state->DHRecv)));
    }

    Buffer rachetDecrypt(RachetState* state, RachetHeader header, Buffer ciphertext, Buffer associatedData)
    {
        if (state) {
            bool success = false;
            Buffer plaintext = this->tryDecryptBySkippedKeys(state, header, ciphertext, associatedData, &success);
            if (success)
                return plaintext;

            if (!curve.comparePoint(header.publicKey, state->DHRecv)) {
                this->trySkipMessageKeys(state, header.prevChainLen);
                this->dhRachet(state, header);
            }

            this->trySkipMessageKeys(state, header.iMess);
            Buffer messageKey = this->updateChainKey(&state->chainKeyRecv);
            state->iMessRecv++;

            return this->innerDecrypt(messageKey, ciphertext, associatedData + this->serialize(header));
        } 
        
        else {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! SignalProtocol.h: rachetEncrypt(): NULL pointers state detected!" << std::endl;
            throw CryptFailException(errorStream.str());
            return Buffer();
        }
    }

    // ------------------- Signal-wrapped crypto functions ---------------------
    Buffer signalEncrypt(RachetState* state, RachetHeader* header, Buffer plaintext, Buffer associatedData)
    {
        try {
            return this->rachetEncrypt(state, header, plaintext, associatedData);
        } catch (std::exception e) {
            return Buffer();
        }
    } 

    Buffer signalDecrypt(RachetState* state, RachetHeader header, Buffer ciphertext, Buffer associatedData)
    {
        try {
            // Try duplicate state, then update it if protocol is successful.
            RachetState _state = *state;
            Buffer plaintext = this->rachetDecrypt(&_state, header, ciphertext, associatedData);
            *state = _state;
            return plaintext;
        } catch (std::exception e) {
            return Buffer();
        }
    }
};