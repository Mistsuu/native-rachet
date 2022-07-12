#pragma once

#include <napi.h>
#include "Crypto/SignalProtocol.h"
#include "Const_Native.h"

class SignalProto_Native : public Napi::ObjectWrap<SignalProto_Native>
{
private:
    SignalProtocol proto;
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        Napi::Function func = 
            DefineClass(env,
                        "SignalProto_Native",
                        {InstanceMethod("generateKeyPair",        &SignalProto_Native::GenerateKeyPair),
                         InstanceMethod("calculateSignature",     &SignalProto_Native::CalculateSignature),
                         InstanceMethod("verifySignature",        &SignalProto_Native::VerifySignature),
                         InstanceMethod("calculateSharedSecretA", &SignalProto_Native::CalculateSharedSecretA),
                         InstanceMethod("calculateSharedSecretB", &SignalProto_Native::CalculateSharedSecretB),
                         InstanceMethod("rachetInitAlice",        &SignalProto_Native::RachetInitAlice),
                         InstanceMethod("rachetInitBob",          &SignalProto_Native::RachetInitBob),
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
        if (NKeyPair.Get(PUBLIC_KEY_STR).IsBuffer()
         && NKeyPair.Get(PRIVATE_KEY_STR).IsBuffer())
        {
            Napi::Buffer<u_char> NPublicKey  = NKeyPair.Get(PUBLIC_KEY_STR).As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NPrivateKey = NKeyPair.Get(PRIVATE_KEY_STR).As<Napi::Buffer<u_char>>();
            this->parsePublicKey(env, NPublicKey, keyPair.publicKey);
            this->parsePrivateKey(env, NPrivateKey, keyPair.privateKey);
            return true;
        }
        return false;
    }

    bool parsePreKeyBundleA(Napi::Env& env, Napi::Object& NAliceKeyBundle, x3DHPreKeyBundleA& aliceKeyBundle)
    {
        if (NAliceKeyBundle.Get(IDENTITY_KEY_STR).IsObject() 
         && NAliceKeyBundle.Get(EPHEMERAL_KEY_STR).IsObject()) 
        {
            Napi::Object NIdentityKey  = NAliceKeyBundle.Get(IDENTITY_KEY_STR).ToObject();
            Napi::Object NEphemeralKey = NAliceKeyBundle.Get(EPHEMERAL_KEY_STR).ToObject();
            if (!this->parseKeyPair(env, NIdentityKey, aliceKeyBundle.identityKey))
                return false;
            if (!this->parseKeyPair(env, NEphemeralKey, aliceKeyBundle.ephemeralKey))
                return false;
            return true;
        }
        return false;
    }

    bool parsePreKeyBundleB(Napi::Env& env, Napi::Object& NBobKeyBundle, x3DHPreKeyBundleB& bobKeyBundle)
    {
        if (NBobKeyBundle.Get(IDENTITY_KEY_STR).IsObject() 
         && NBobKeyBundle.Get(SIGNED_PREKEY_STR).IsObject() 
         && NBobKeyBundle.Get(ONETIME_PREKEY_STR).IsObject()) 
        {
            Napi::Object NIdentityKey   = NBobKeyBundle.Get(IDENTITY_KEY_STR).ToObject();
            Napi::Object NSignedPreKey  = NBobKeyBundle.Get(SIGNED_PREKEY_STR).ToObject();
            Napi::Object NOneTimePreKey = NBobKeyBundle.Get(ONETIME_PREKEY_STR).ToObject();
            if (!this->parseKeyPair(env, NIdentityKey, bobKeyBundle.identityKey))
                return false;
            if (!this->parseKeyPair(env, NSignedPreKey, bobKeyBundle.signedPreKey))
                return false;
            if (!this->parseKeyPair(env, NOneTimePreKey, bobKeyBundle.oneTimePreKey))
                return false;
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
        Buffer serializedPublicKey = this->proto.serialize(publicKey);
        return this->ToNapiObject(env, serializedPublicKey);
    }

    inline Napi::Object ToNapiObject(Napi::Env& env, KeyPair& keyPair)
    {
        Napi::Object NKeyPairObj = Napi::Object::New(env);
        NKeyPairObj.Set(PUBLIC_KEY_STR,  this->ToNapiObject(env, keyPair.publicKey));
        NKeyPairObj.Set(PRIVATE_KEY_STR, this->ToNapiObject(env, keyPair.privateKey));
        return NKeyPairObj;
    }

    inline Napi::Array ToNapiObject(Napi::Env& env, std::vector<SkippedKeyNode>& skippedKeys) // TODO: Create a template here.
    {
        Napi::Array NSkippedKeys = Napi::Array::New(env, skippedKeys.size());
        for (int i = 0; i < (int)skippedKeys.size(); ++i) {
            Napi::Object NSkippedKey = Napi::Object::New(env);
            NSkippedKey.Set(DHPUBLIC_STR,    this->ToNapiObject(env, skippedKeys[i].DHPublic));
            NSkippedKey.Set(IMESS_STR,       this->ToNapiNumber(env, skippedKeys[i].iMess));
            NSkippedKey.Set(MESSAGEKEY_STR,  this->ToNapiObject(env, skippedKeys[i].messageKey));
            NSkippedKeys.Set(i, NSkippedKey);
        }
        return NSkippedKeys;
    }

    inline Napi::Object ToNapiObject(Napi::Env& env, RachetState& state)
    {
        Napi::Object NRachetStateObj = Napi::Object::New(env);
        NRachetStateObj.Set(DHSEND_STR,       this->ToNapiObject(env, state.DHSend));
        NRachetStateObj.Set(DHRECV_STR,       this->ToNapiObject(env, state.DHRecv));
        NRachetStateObj.Set(ROOTKEY_STR,      this->ToNapiObject(env, state.rootKey));
        NRachetStateObj.Set(CHAINKEYSEND_STR, this->ToNapiObject(env, state.chainKeySend));
        NRachetStateObj.Set(CHAINKEYRECV_STR, this->ToNapiObject(env, state.chainKeyRecv));
        NRachetStateObj.Set(IMESSSEND_STR,    this->ToNapiNumber(env, state.iMessSend));
        NRachetStateObj.Set(IMESSRECV_STR,    this->ToNapiNumber(env, state.iMessRecv));
        NRachetStateObj.Set(PREVCHAINLEN_STR, this->ToNapiNumber(env, state.prevChainLen));
        NRachetStateObj.Set(SKIPPEDKEYS_STR,  this->ToNapiObject(env, state.skippedKeys));
        return NRachetStateObj;
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

            if (this->parsePreKeyBundleA(env, NAliceKeyBundle, aliceKeyBundle)
             && this->parsePreKeyBundleB(env, NBobKeyBundle, bobKeyBundle))
            {
                Buffer sharedSecret = this->proto.calculateSharedSecret(aliceKeyBundle, bobKeyBundle);
                return this->ToNapiObject(env, sharedSecret);
            }    
        }

        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Object aliceKeyBundle, Object bobKeyBundle)\n"
            "where:\n"
            "\n"                 
            "Object [aliceKeyBundle] contains:\n"
            "   - " IDENTITY_KEY_STR " @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
            "   - " EPHEMERAL_KEY_STR " @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"
            "\n"                 
            "Object [bobKeyBundle] contains:\n"
            "   - " IDENTITY_KEY_STR " @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
            "   - " SIGNED_PREKEY_STR " @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
            "   - " ONETIME_PREKEY_STR " @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
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

            if (this->parsePreKeyBundleA(env, NAliceKeyBundle, aliceKeyBundle)
             && this->parsePreKeyBundleB(env, NBobKeyBundle, bobKeyBundle))
            {
                Buffer sharedSecret = this->proto.calculateSharedSecret(bobKeyBundle, aliceKeyBundle);
                return this->ToNapiObject(env, sharedSecret);
            }    
        }

        throw Napi::TypeError::New(env, 
            "Argument supply should be: (Object aliceKeyBundle, Object bobKeyBundle)\n"
            "where:\n"
            "\n"                 
            "Object [bobKeyBundle] contains:\n"
            "   - " IDENTITY_KEY_STR " @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
            "   - " SIGNED_PREKEY_STR " @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
            "   - " ONETIME_PREKEY_STR " @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
            "\n"                 
            "Object [aliceKeyBundle] contains:\n"
            "   - " IDENTITY_KEY_STR " @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
            "   - " EPHEMERAL_KEY_STR " @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"
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
            "   @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
            "\n"                 
            "Object [bobIdentityKey] is:\n"
            "   @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
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
            "   @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
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
            "   @Object { " PRIVATE_KEY_STR " @Buffer, " PUBLIC_KEY_STR " @Buffer }\n"                 
        );    
    }

    Napi::Value SignalEncrypt(const Napi::CallbackInfo& info)
    {

    }

    Napi::Value SignalDecrypt(const Napi::CallbackInfo& info)
    {

    }
};