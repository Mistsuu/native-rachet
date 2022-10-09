#pragma once

#include <napi.h>
#include "Crypto/SignalProtocol.h"
#include "Const_Native.h"

class SignalProto_Native : public Napi::ObjectWrap<SignalProto_Native>
{
private:
    SignalProtocol proto;
public:
    static Napi::Object Init(Napi::Env& env, Napi::Object& exports)
    {
        Napi::Function func = 
            DefineClass(env,
                        "SignalProto_Native",
                        {InstanceMethod("generateKeyPair",         &SignalProto_Native::GenerateKeyPair),
                         InstanceMethod("calculateSignature",      &SignalProto_Native::CalculateSignature),
                         InstanceMethod("verifySignature",         &SignalProto_Native::VerifySignature),
                         InstanceMethod("calculateSharedSecretA",  &SignalProto_Native::CalculateSharedSecretA),
                         InstanceMethod("calculateSharedSecretB",  &SignalProto_Native::CalculateSharedSecretB),
                         InstanceMethod("calculateAssociatedData", &SignalProto_Native::CalculateAssociatedData),
                         InstanceMethod("rachetInitAlice",         &SignalProto_Native::RachetInitAlice),
                         InstanceMethod("rachetInitBob",           &SignalProto_Native::RachetInitBob),
                         InstanceMethod("innerEncrypt",            &SignalProto_Native::InnerEncrypt),
                         InstanceMethod("innerDecrypt",            &SignalProto_Native::InnerDecrypt),
                         InstanceMethod("signalEncrypt",           &SignalProto_Native::SignalEncrypt),
                         InstanceMethod("signalDecrypt",           &SignalProto_Native::SignalDecrypt),
                         InstanceMethod("serializeRachetHeader",   &SignalProto_Native::SerializeRachetHeader),
                         InstanceMethod("deserializeRachetHeader", &SignalProto_Native::DeserializeRachetHeader),
                         InstanceMethod("test",                    &SignalProto_Native::TestFunc),
                        });

        Napi::FunctionReference* constructor = new Napi::FunctionReference();

        // Create a persistent reference to the class constructor. This will allow
        // a function called on a class prototype and a function
        // called on instance of a class to be distinguished from each other.
        *constructor = Napi::Persistent(func);
        exports.Set("SignalProto_Native", func);

        // Store the constructor as the add-on instance data. This will allow this
        // add-on to support multiple instances of itself running on multiple worker
        // threads, as well as multiple instances of itself running in different
        // contexts on the same thread.
        //
        // By default, the value set on the environment here will be destroyed when
        // the add-on is unloaded using the `delete` operator, but it is also
        // possible to supply a custom deleter.
        env.SetInstanceData<Napi::FunctionReference>(constructor);

        return exports;
    }

    SignalProto_Native(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<SignalProto_Native>(info)
    {
    }


    void parseNumber(Napi::Env& env, Napi::Number NNum, Int& num)
    {
        num = NNum.ToString().Utf8Value();
    }

    void parseBuffer(Napi::Env& env, Napi::Buffer<u_char> NBuffer, Buffer& buf)
    {
        buf.set((char*)NBuffer.Data(), NBuffer.Length());
    }

    void parsePrivateKey(Napi::Env& env, Napi::Buffer<u_char>& NPrivateKey, Int& privateKey)
    {
        privateKey = this->proto.deserializeInt(Buffer((char*)NPrivateKey.Data(), NPrivateKey.Length()));
    }

    void parsePublicKey(Napi::Env& env, Napi::Buffer<u_char>& NPublicKey, PointMongomery& publicKey)
    {
        try {
            publicKey = this->proto.deserializeMongomeryPoint(Buffer((char*)NPublicKey.Data(), NPublicKey.Length()));
        } catch (DeserializeErrorException& e) {
            throw Napi::Error::New(env, e.what());
        } catch (InvalidPointException& e) {
            throw Napi::Error::New(env, e.what());
        }
    }

    bool parseKeyPair(Napi::Env& env, Napi::Object& NKeyPair, KeyPair& keyPair)
    {
        if (NKeyPair.Get(PUBLIC_KEY).IsBuffer()
         && NKeyPair.Get(PRIVATE_KEY).IsBuffer())
        {
            Napi::Buffer<u_char> NPublicKey  = NKeyPair.Get(PUBLIC_KEY).As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NPrivateKey = NKeyPair.Get(PRIVATE_KEY).As<Napi::Buffer<u_char>>();
            this->parsePublicKey(env, NPublicKey, keyPair.publicKey);
            this->parsePrivateKey(env, NPrivateKey, keyPair.privateKey);
            return true;
        }
        return false;
    }

    bool parsePreKeyBundleA(Napi::Env& env, Napi::Object& NAliceKeyBundle, x3DHPreKeyBundleA& aliceKeyBundle)
    {
        if (NAliceKeyBundle.Get(IDENTITY_KEY).IsObject() 
         && NAliceKeyBundle.Get(EPHEMERAL_KEY).IsObject()) 
        {
            Napi::Object NIdentityKey  = NAliceKeyBundle.Get(IDENTITY_KEY).ToObject();
            Napi::Object NEphemeralKey = NAliceKeyBundle.Get(EPHEMERAL_KEY).ToObject();
            if (!this->parseKeyPair(env, NIdentityKey, aliceKeyBundle.identityKey))
                return false;
            if (!this->parseKeyPair(env, NEphemeralKey, aliceKeyBundle.ephemeralKey))
                return false;
            return true;
        }
        return false;
    }

    int parsePreKeyBundleB(Napi::Env& env, Napi::Object& NBobKeyBundle, x3DHPreKeyBundleB& bobKeyBundle)
    {
        if (NBobKeyBundle.Get(IDENTITY_KEY).IsObject() 
         && NBobKeyBundle.Get(SIGNED_PREKEY).IsObject() 
         && NBobKeyBundle.Get(ONETIME_PREKEY).IsObject()) 
        {
            Napi::Object NIdentityKey   = NBobKeyBundle.Get(IDENTITY_KEY).ToObject();
            Napi::Object NSignedPreKey  = NBobKeyBundle.Get(SIGNED_PREKEY).ToObject();
            Napi::Object NOneTimePreKey = NBobKeyBundle.Get(ONETIME_PREKEY).ToObject();
            if (!this->parseKeyPair(env, NIdentityKey, bobKeyBundle.identityKey))
                return PARSE_BOBPREKEY_FAILED;
            if (!this->parseKeyPair(env, NSignedPreKey, bobKeyBundle.signedPreKey))
                return PARSE_BOBPREKEY_FAILED;
            if (!this->parseKeyPair(env, NOneTimePreKey, bobKeyBundle.oneTimePreKey))
                return PARSE_BOBPREKEY_ONETIME_MISSING;
            return PARSE_BOBPREKEY_SUCCESS;
        }
        return PARSE_BOBPREKEY_FAILED;
    }

    bool parseSkippedKeys(Napi::Env& env, Napi::Array& NSkippedKeys, std::vector<SkippedKeyNode>& skippedKeys)
    {
        skippedKeys.resize(NSkippedKeys.Length());
        for (int i = 0; i < (int)NSkippedKeys.Length(); ++i) {
            if (!NSkippedKeys.Get(i).IsObject())
                return false;
            
            Napi::Object NSkippedKey = NSkippedKeys.Get(i).ToObject();
            if (!NSkippedKey.Get(DHPUBLIC).IsBuffer()
             || !NSkippedKey.Get(IMESS).IsNumber()
             || !NSkippedKey.Get(MESSAGEKEY).IsBuffer())
                return false;
                
            Napi::Buffer<u_char> NDHPublic   = NSkippedKey.Get(DHPUBLIC).As<Napi::Buffer<u_char>>();
            Napi::Number         NIMess      = NSkippedKey.Get(IMESS).ToNumber();
            Napi::Buffer<u_char> NMessageKey = NSkippedKey.Get(MESSAGEKEY).As<Napi::Buffer<u_char>>();

            this->parsePublicKey(env, NDHPublic, skippedKeys[i].DHPublic);
            this->parseNumber(env, NIMess, skippedKeys[i].iMess);
            this->parseBuffer(env, NMessageKey, skippedKeys[i].messageKey);
        }
        return true;
    }

    bool parseRachetState(Napi::Env& env, Napi::Object& NRachetStateObj, RachetState& state)
    {
        if (NRachetStateObj.Get(DHSEND).IsObject()
         && NRachetStateObj.Get(DHRECV).IsBuffer()
         && NRachetStateObj.Get(ROOTKEY).IsBuffer()
         && NRachetStateObj.Get(CHAINKEYSEND).IsBuffer()
         && NRachetStateObj.Get(CHAINKEYRECV).IsBuffer()
         && NRachetStateObj.Get(IMESSSEND).IsNumber()
         && NRachetStateObj.Get(IMESSRECV).IsNumber()
         && NRachetStateObj.Get(PREVCHAINLEN).IsNumber()
         && NRachetStateObj.Get(SKIPPEDKEYS).IsArray())
        {
            Napi::Object            NDHSend         = NRachetStateObj.Get(DHSEND).ToObject();
            Napi::Buffer<u_char>    NDHRecv         = NRachetStateObj.Get(DHRECV).As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char>    NRootKey        = NRachetStateObj.Get(ROOTKEY).As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char>    NChainKeySend   = NRachetStateObj.Get(CHAINKEYSEND).As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char>    NChainKeyRecv   = NRachetStateObj.Get(CHAINKEYRECV).As<Napi::Buffer<u_char>>();
            Napi::Number            NIMessSend      = NRachetStateObj.Get(IMESSSEND).ToNumber();
            Napi::Number            NIMessRecv      = NRachetStateObj.Get(IMESSRECV).ToNumber();
            Napi::Number            NPrevChainLen   = NRachetStateObj.Get(PREVCHAINLEN).ToNumber();
            Napi::Array             NSkippedKeys    = NRachetStateObj.Get(SKIPPEDKEYS).As<Napi::Array>();
            
            if (!this->parseKeyPair(env, NDHSend, state.DHSend))
                return false;
            if (!this->parseSkippedKeys(env, NSkippedKeys, state.skippedKeys))
                return false;
            this->parsePublicKey(env, NDHRecv, state.DHRecv);
            this->parseBuffer(env, NRootKey, state.rootKey);
            this->parseBuffer(env, NChainKeySend, state.chainKeySend);
            this->parseBuffer(env, NChainKeyRecv, state.chainKeyRecv);
            this->parseNumber(env, NIMessSend, state.iMessSend);
            this->parseNumber(env, NIMessRecv, state.iMessRecv);
            this->parseNumber(env, NPrevChainLen, state.prevChainLen);
            return true;
        }
        return false;
    }

    bool parseRachetHeader(Napi::Env& env, Napi::Object& NRachetHeaderObj, RachetHeader& header)
    {
        if (NRachetHeaderObj.Get(PUBLIC_KEY).IsBuffer()
         && NRachetHeaderObj.Get(IMESS).IsNumber()
         && NRachetHeaderObj.Get(PREVCHAINLEN).IsNumber())
        {
            Napi::Buffer<u_char> NPublicKey    = NRachetHeaderObj.Get(PUBLIC_KEY).As<Napi::Buffer<u_char>>();
            Napi::Number         NIMess        = NRachetHeaderObj.Get(IMESS).ToNumber();
            Napi::Number         NPrevChainLen = NRachetHeaderObj.Get(PREVCHAINLEN).ToNumber();

            this->parsePublicKey(env, NPublicKey, header.publicKey);
            this->parseNumber(env, NIMess, header.iMess);
            this->parseNumber(env, NPrevChainLen, header.prevChainLen);

            return true;
        }
        return false;
    }


    inline Napi::Number ToNapiNumber(Napi::Env& env, Int& num)
    {
        if (num.fits_ulong_p())
            return Napi::Number::New(env, num.get_ui());
        else 
            return Napi::Number::New(env, -1);
    }

    inline Napi::Boolean ToNapiObject(Napi::Env& env, bool val)
    {
        return Napi::Boolean::New(env, val);
    }

    inline Napi::Buffer<u_char> ToNapiObject(Napi::Env& env, Buffer& buf)
    {
        return Napi::Buffer<u_char>::Copy(env, buf.data(), buf.len());
    }

    inline Napi::Buffer<u_char> ToNapiObject(Napi::Env& env, Int& privateKey)
    {
        Buffer serializedPrivateKey = this->proto.serialize(privateKey);
        return this->ToNapiObject(env, serializedPrivateKey);
    }

    inline Napi::Buffer<u_char> ToNapiObject(Napi::Env& env, PointMongomery& publicKey)
    {
        try {
            Buffer serializedPublicKey = this->proto.serialize(publicKey);
            return this->ToNapiObject(env, serializedPublicKey);
        } catch (InvalidPointException& e) {
            throw Napi::Error::New(env, e.what());
        }
    }

    inline Napi::Object ToNapiObject(Napi::Env& env, KeyPair& keyPair)
    {
        Napi::Object NKeyPairObj = Napi::Object::New(env);
        NKeyPairObj.Set(PUBLIC_KEY,  this->ToNapiObject(env, keyPair.publicKey));
        NKeyPairObj.Set(PRIVATE_KEY, this->ToNapiObject(env, keyPair.privateKey));
        return NKeyPairObj;
    }

    inline Napi::Array ToNapiObject(Napi::Env& env, std::vector<SkippedKeyNode>& skippedKeys) // TODO: Create a template here.
    {
        Napi::Array NSkippedKeys = Napi::Array::New(env, skippedKeys.size());
        for (int i = 0; i < (int)skippedKeys.size(); ++i) {
            Napi::Object NSkippedKey = Napi::Object::New(env);
            NSkippedKey.Set(DHPUBLIC,    this->ToNapiObject(env, skippedKeys[i].DHPublic));
            NSkippedKey.Set(IMESS,       this->ToNapiNumber(env, skippedKeys[i].iMess));
            NSkippedKey.Set(MESSAGEKEY,  this->ToNapiObject(env, skippedKeys[i].messageKey));
            NSkippedKeys.Set(i, NSkippedKey);
        }
        return NSkippedKeys;
    }

    inline Napi::Object ToNapiObject(Napi::Env& env, RachetState& state)
    {
        Napi::Object NRachetStateObj = Napi::Object::New(env);
        NRachetStateObj.Set(DHSEND,       this->ToNapiObject(env, state.DHSend));
        NRachetStateObj.Set(DHRECV,       this->ToNapiObject(env, state.DHRecv));
        NRachetStateObj.Set(ROOTKEY,      this->ToNapiObject(env, state.rootKey));
        NRachetStateObj.Set(CHAINKEYSEND, this->ToNapiObject(env, state.chainKeySend));
        NRachetStateObj.Set(CHAINKEYRECV, this->ToNapiObject(env, state.chainKeyRecv));
        NRachetStateObj.Set(IMESSSEND,    this->ToNapiNumber(env, state.iMessSend));
        NRachetStateObj.Set(IMESSRECV,    this->ToNapiNumber(env, state.iMessRecv));
        NRachetStateObj.Set(PREVCHAINLEN, this->ToNapiNumber(env, state.prevChainLen));
        NRachetStateObj.Set(SKIPPEDKEYS,  this->ToNapiObject(env, state.skippedKeys));
        return NRachetStateObj;
    }

    inline Napi::Object ToNapiObject(Napi::Env& env, RachetHeader& header)
    {
        Napi::Object NRachetHeaderObj = Napi::Object::New(env);
        NRachetHeaderObj.Set(PUBLIC_KEY,    this->ToNapiObject(env, header.publicKey));
        NRachetHeaderObj.Set(IMESS,         this->ToNapiNumber(env, header.iMess));
        NRachetHeaderObj.Set(PREVCHAINLEN,  this->ToNapiNumber(env, header.prevChainLen));
        return NRachetHeaderObj;
    }

    
    Napi::Value GenerateKeyPair(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();
        KeyPair keyPair = this->proto.generateKeyPair();
        return this->ToNapiObject(env, keyPair);
    }

    Napi::Value CalculateSignature(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 2)
            throw Napi::TypeError::New(env, "Argument supply should be: (Buffer privateKey, Buffer input)");

        if (info[0].IsBuffer() && info[1].IsBuffer()) {
            Napi::Buffer<u_char> NPrivateKey = info[0].As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NInput      = info[1].As<Napi::Buffer<u_char>>();

            Buffer serializedPrivateKey((char*)NPrivateKey.Data(), NPrivateKey.Length());
            Buffer input               ((char*)NInput.Data(),      NInput.Length());
            Buffer signature = this->proto.XEdDSA_sign(
                                this->proto.deserializeInt(serializedPrivateKey),
                                input
                               );
                               
            return this->ToNapiObject(env, signature);
        }

        throw Napi::TypeError::New(env, "Argument supply should be: (Buffer privateKey, Buffer input)");
    }

    Napi::Value VerifySignature(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 3)
            throw Napi::TypeError::New(env, "Argument supply should be: (Buffer publicKey, Buffer input, Buffer signature)");

        if (info[0].IsBuffer() && info[1].IsBuffer() && info[2].IsBuffer()) {
            Napi::Buffer<u_char> NPublicKey = info[0].As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NInput     = info[1].As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NSignature = info[2].As<Napi::Buffer<u_char>>();

            Buffer publicKey((char*)NPublicKey.Data(), NPublicKey.Length());
            Buffer input    ((char*)NInput.Data(), NInput.Length());
            Buffer signature((char*)NSignature.Data(), NSignature.Length());

            return this->ToNapiObject(
                    env,
                    this->proto.XEdDSA_verify(publicKey, input, signature)
                   );
        }

        throw Napi::TypeError::New(env, "Argument supply should be: (Buffer publicKey, Buffer input, Buffer signature)");
    }

    Napi::Value CalculateSharedSecretA(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();
        if (info.Length() >= 2 && info[0].IsObject() && info[1].IsObject()) {
            Napi::Object NAliceKeyBundle = info[0].ToObject();
            Napi::Object NBobKeyBundle   = info[1].ToObject();

            x3DHPreKeyBundleA aliceKeyBundle;
            x3DHPreKeyBundleB bobKeyBundle;
            
            int parseBobStatus = PARSE_BOBPREKEY_FAILED;
            if (                  this->parsePreKeyBundleA(env, NAliceKeyBundle, aliceKeyBundle)
             && (parseBobStatus = this->parsePreKeyBundleB(env, NBobKeyBundle, bobKeyBundle)) != PARSE_BOBPREKEY_FAILED)
            {
                Buffer sharedSecret = this->proto.calculateSharedSecret(aliceKeyBundle, bobKeyBundle, parseBobStatus == PARSE_BOBPREKEY_SUCCESS);
                return this->ToNapiObject(env, sharedSecret);
            }    
        }

        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Object aliceKeyBundle, Object bobKeyBundle)\n"
            "where:\n"
            "\n"                 
            "Object [aliceKeyBundle] contains:\n"
            "   - " IDENTITY_KEY " @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
            "   - " EPHEMERAL_KEY " @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"
            "\n"                 
            "Object [bobKeyBundle] contains:\n"
            "   - " IDENTITY_KEY " @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
            "   - " SIGNED_PREKEY " @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
            "   - " ONETIME_PREKEY " (optional) @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
        );
        
    }

    Napi::Value CalculateSharedSecretB(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();
        if (info.Length() >= 2 && info[0].IsObject() && info[1].IsObject()) {
            Napi::Object NBobKeyBundle   = info[0].ToObject();
            Napi::Object NAliceKeyBundle = info[1].ToObject();

            x3DHPreKeyBundleB bobKeyBundle;
            x3DHPreKeyBundleA aliceKeyBundle;
            
            int parseBobStatus = PARSE_BOBPREKEY_FAILED;
            if (                  this->parsePreKeyBundleA(env, NAliceKeyBundle, aliceKeyBundle)
             && (parseBobStatus = this->parsePreKeyBundleB(env, NBobKeyBundle, bobKeyBundle)) != PARSE_BOBPREKEY_FAILED)
            {
                Buffer sharedSecret = this->proto.calculateSharedSecret(bobKeyBundle, aliceKeyBundle, parseBobStatus == PARSE_BOBPREKEY_SUCCESS);
                return this->ToNapiObject(env, sharedSecret);
            }    
        }

        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Object aliceKeyBundle, Object bobKeyBundle)\n"
            "where:\n"
            "\n"                 
            "Object [bobKeyBundle] contains:\n"
            "   - " IDENTITY_KEY " @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
            "   - " SIGNED_PREKEY " @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
            "   - " ONETIME_PREKEY " (optional) @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
            "\n"                 
            "Object [aliceKeyBundle] contains:\n"
            "   - " IDENTITY_KEY " @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
            "   - " EPHEMERAL_KEY " @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"
        );
        
    }

    Napi::Value CalculateAssociatedData(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();
        if (info.Length() >= 2 && info[0].IsObject() && info[1].IsObject()) {
            Napi::Object NAliceIdentityKey = info[0].ToObject();
            Napi::Object NBobIdentityKey   = info[1].ToObject();

            KeyPair aliceIdentityKey;
            KeyPair bobIdentityKey;
            if (this->parseKeyPair(env, NAliceIdentityKey, aliceIdentityKey)
             && this->parseKeyPair(env, NBobIdentityKey, bobIdentityKey))
            {
                Buffer associatedData = this->proto.calculateAssociatedData(aliceIdentityKey, bobIdentityKey);
                return this->ToNapiObject(env, associatedData);
            }  
        }      

        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Object aliceIdentityKey, Object bobIdentityKey)\n"
            "where:\n"
            "\n"                 
            "Object [aliceIdentityKey] is:\n"
            "   @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
            "\n"                 
            "Object [bobIdentityKey] is:\n"
            "   @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
        );    
    }

    Napi::Value RachetInitAlice(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();
        if (info.Length() >= 2 && info[0].IsBuffer() && info[1].IsObject()) {
            Napi::Buffer<u_char> NSharedSecret = info[0].As<Napi::Buffer<u_char>>();
            Napi::Object         NBobKeyPair   = info[1].ToObject();

            // Parse key pair            
            KeyPair bobKeyPair;
            this->parseKeyPair(env, NBobKeyPair, bobKeyPair);
            // Parse buffer
            Buffer sharedSecret((char*)NSharedSecret.Data(), NSharedSecret.Length());
            // Return state
            RachetState state;
            this->proto.rachetInitAlice(&state, sharedSecret, bobKeyPair.publicKey);
            return this->ToNapiObject(env, state);
        }
        
        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Buffer sharedSecret, Object bobKeyPair)\n"
            "where:\n"
            "\n"                 
            "Object [bobKeyPair] is:\n"
            "   @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
        );    
    }

    Napi::Value RachetInitBob(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();
        if (info.Length() >= 2 && info[0].IsBuffer() && info[1].IsObject()) {
            Napi::Buffer<u_char> NSharedSecret = info[0].As<Napi::Buffer<u_char>>();
            Napi::Object         NBobKeyPair   = info[1].ToObject();

            // Parse key pair            
            KeyPair bobKeyPair;
            this->parseKeyPair(env, NBobKeyPair, bobKeyPair);
            // Parse buffer
            Buffer sharedSecret((char*)NSharedSecret.Data(), NSharedSecret.Length());
            // Return state
            RachetState state;
            this->proto.rachetInitBob(&state, sharedSecret, bobKeyPair);
            return this->ToNapiObject(env, state);
        }
        
        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Buffer sharedSecret, Object bobKeyPair)\n"
            "where:\n"
            "\n"                 
            "Object [bobKeyPair] is:\n"
            "   @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
        );    
    }

    Napi::Value InnerEncrypt(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() >= 3 && info[0].IsBuffer() && info[1].IsBuffer() && info[2].IsBuffer()) {
            Napi::Buffer<u_char> NMessageKey       = info[0].As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NPlaintext        = info[1].As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NAssociatedData   = info[2].As<Napi::Buffer<u_char>>();
            
            Buffer messageKey;
            Buffer plaintext;
            Buffer associatedData;
            this->parseBuffer(env, NMessageKey, messageKey);
            this->parseBuffer(env, NPlaintext, plaintext);
            this->parseBuffer(env, NAssociatedData, associatedData);

            Buffer ciphertext = this->proto.innerEncrypt(messageKey, plaintext, associatedData);
            return this->ToNapiObject(env, ciphertext);
        }

        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Buffer messageKey, Buffer plaintext, Buffer associatedData)\n"
        ); 
    }

    Napi::Value InnerDecrypt(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() >= 3 && info[0].IsBuffer() && info[1].IsBuffer() && info[2].IsBuffer()) {
            Napi::Buffer<u_char> NMessageKey       = info[0].As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NCiphertext       = info[1].As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NAssociatedData   = info[2].As<Napi::Buffer<u_char>>();
            
            Buffer messageKey;
            Buffer ciphertext;
            Buffer associatedData;
            this->parseBuffer(env, NMessageKey, messageKey);
            this->parseBuffer(env, NCiphertext, ciphertext);
            this->parseBuffer(env, NAssociatedData, associatedData);

            Buffer plaintext = this->proto.innerDecrypt(messageKey, ciphertext, associatedData);
            return this->ToNapiObject(env, plaintext);
        }

        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Buffer messageKey, Buffer ciphertext, Buffer associatedData)\n"
        ); 
    }

    Napi::Value SignalEncrypt(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() >= 3 && info[0].IsObject() && info[1].IsBuffer() && info[2].IsBuffer()) {
            Napi::Object         NRachetState      = info[0].ToObject();
            Napi::Buffer<u_char> NPlaintext        = info[1].As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NAssociatedData   = info[2].As<Napi::Buffer<u_char>>();
            
            RachetState rachetState;
            RachetHeader rachetHeader;
            Buffer plaintext;
            Buffer associatedData;
            if (this->parseRachetState(env, NRachetState, rachetState)) {
                this->parseBuffer(env, NPlaintext, plaintext);
                this->parseBuffer(env, NAssociatedData, associatedData);

                Buffer ciphertext = this->proto.signalEncrypt(&rachetState, &rachetHeader, plaintext, associatedData);

                Napi::Object NCiphertextObj = Napi::Object::New(env);
                NCiphertextObj.Set(RACHET_STATE, this->ToNapiObject(env, rachetState));
                NCiphertextObj.Set(RACHET_HEADER, this->ToNapiObject(env, rachetHeader));
                NCiphertextObj.Set(CIPHERTEXT, this->ToNapiObject(env, ciphertext));
                return NCiphertextObj;
            }
        }

        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Object rachetState, Buffer plaintext, Buffer associatedData)\n"
            "where:\n"
            "\n"                 
            "Object [rachetState] contains:\n"
            "   - @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"                 
        ); 
    }

    Napi::Value SignalDecrypt(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() >= 4 && info[0].IsObject() && info[1].IsObject() && info[2].IsBuffer() && info[3].IsBuffer()) {
            Napi::Object         NRachetState      = info[0].ToObject();
            Napi::Object         NRachetHeader     = info[1].ToObject();
            Napi::Buffer<u_char> NCiphertext       = info[2].As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NAssociatedData   = info[3].As<Napi::Buffer<u_char>>();
            
            RachetState rachetState;
            RachetHeader rachetHeader;
            Buffer ciphertext;
            Buffer associatedData;
            if (this->parseRachetState(env, NRachetState, rachetState)
             && this->parseRachetHeader(env, NRachetHeader, rachetHeader)) {
                this->parseBuffer(env, NCiphertext, ciphertext);
                this->parseBuffer(env, NAssociatedData, associatedData);

                Buffer plaintext = this->proto.signalDecrypt(&rachetState, rachetHeader, ciphertext, associatedData);

                Napi::Object NPlaintextObj = Napi::Object::New(env);
                NPlaintextObj.Set(RACHET_STATE, this->ToNapiObject(env, rachetHeader));
                NPlaintextObj.Set(PLAINTEXT, this->ToNapiObject(env, plaintext));
                return NPlaintextObj;
            }
        }

        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Object rachetState, Object rachetHeader, Buffer ciphertext, Buffer associatedData)\n"
            "where:\n"
            "\n"                 
            "Object [rachetState] contains:\n"
            "   - @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"           
            "\n"                 
            "Object [rachetHeader] contains:\n"
            "   - @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"        
        ); 
    }


    Napi::Value SerializeRachetHeader(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() >= 1 && info[0].IsObject()) {
            Napi::Object NRachetHeader = info[0].ToObject();
            
            RachetHeader rachetHeader;
            if (this->parseRachetHeader(env, NRachetHeader, rachetHeader)) {
                Buffer serializedRachetHeader = this->proto.serialize(rachetHeader);
                return this->ToNapiObject(env, serializedRachetHeader);
            }
        }

        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Object rachetHeader)\n"
            "where:\n"
            "\n"                 
            "Object [rachetHeader] contains:\n"
            "   - @Object { " PRIVATE_KEY " @Buffer, " PUBLIC_KEY " @Buffer }\n"        
        ); 
    }

    Napi::Value DeserializeRachetHeader(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() >= 1 && info[0].IsBuffer()) {
            Napi::Buffer<u_char> NSerializedRachetHeader = info[0].As<Napi::Buffer<u_char>>();

            Buffer serializedRachetHeader;
            this->parseBuffer(env, NSerializedRachetHeader, serializedRachetHeader);

            RachetHeader rachetHeader = this->proto.deserializeRachetHeader(serializedRachetHeader);
            return this->ToNapiObject(env, rachetHeader);
        }

        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Buffer serializedRachetHeader)\n"
        ); 
    } 


    Napi::Value TestFunc(const Napi::CallbackInfo& info) 
    {
        return Napi::Number::New(info.Env(), 128);
    }
};