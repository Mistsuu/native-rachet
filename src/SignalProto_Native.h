#pragma once

#include <napi.h>
#include "Crypto/SignalProtocol.h"

#define PUBLIC_KEY_STR "publicKey"
#define PRIVATE_KEY_STR "privateKey"
#define IDENTITY_KEY_STR "identityKey"
#define EPHEMERAL_KEY_STR "ephemeralKey"
#define SIGNATURE_STR "signature"
#define SIGNED_PREKEY_STR "signedPreKey"
#define ONETIME_PREKEY_STR "oneTimePreKey"

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

    Napi::Value GenerateKeyPair(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        KeyPair keyPair = this->proto.generateKeyPair();
        Buffer serializedPublicKey = this->proto.serialize(keyPair.publicKey);
        Buffer serializedPrivateKey = this->proto.serialize(keyPair.privateKey);

        Napi::Object NKeyPairObj = Napi::Object::New(env);
        NKeyPairObj.Set(Napi::String::New(env, PUBLIC_KEY_STR), Napi::Buffer<u_char>::Copy(env, serializedPublicKey.data(), serializedPublicKey.len()));
        NKeyPairObj.Set(Napi::String::New(env, PRIVATE_KEY_STR), Napi::Buffer<u_char>::Copy(env, serializedPrivateKey.data(), serializedPrivateKey.len()));
        return NKeyPairObj;
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
                               
            return Napi::Buffer<u_char>::Copy(
                    env,
                    signature.data(),
                    signature.len()
                   );
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

            return Napi::Boolean::New(
                    env,
                    this->proto.XEdDSA_verify(publicKey, input, signature)
                   );
        }

        throw Napi::TypeError::New(env, "Argument supply should be: (Buffer publicKey, Buffer input, Buffer signature)");
    }

    bool parseKeyPair(Napi::Env& env, Napi::Object NKeyPair, KeyPair& keyPair)
    {
        if (NKeyPair.Get(PUBLIC_KEY_STR).IsBuffer()
         && NKeyPair.Get(PRIVATE_KEY_STR).IsBuffer())
        {
            Napi::Buffer<u_char> NPublicKey  = NKeyPair.Get(PUBLIC_KEY_STR).As<Napi::Buffer<u_char>>();
            Napi::Buffer<u_char> NPrivateKey = NKeyPair.Get(PRIVATE_KEY_STR).As<Napi::Buffer<u_char>>();

            try {
                keyPair.publicKey = this->proto.deserializeMongomeryPoint(Buffer((char*)NPublicKey.Data(), NPublicKey.Length()));
            } catch (DeserializeErrorException& e) {
                throw Napi::Error::New(env, e.what());
            } catch (InvalidPointException& e) {
                throw Napi::Error::New(env, e.what());
            }

            keyPair.privateKey = this->proto.deserializeInt(Buffer((char*)NPrivateKey.Data(), NPrivateKey.Length()));
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

    Napi::Value CalculateSharedSecretA(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 2)
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

        if (info[0].IsObject() && info[1].IsObject()) {
            Napi::Object NAliceKeyBundle = info[0].ToObject();
            Napi::Object NBobKeyBundle   = info[1].ToObject();

            x3DHPreKeyBundleA aliceKeyBundle;
            x3DHPreKeyBundleB bobKeyBundle;

            if (this->parsePreKeyBundleA(env, NAliceKeyBundle, aliceKeyBundle)
             && this->parsePreKeyBundleB(env, NBobKeyBundle, bobKeyBundle))
            {
                Buffer sharedSecret = this->proto.calculateSharedSecret(aliceKeyBundle, bobKeyBundle);
                return Napi::Buffer<u_char>::Copy(
                    env,
                    sharedSecret.data(),
                    sharedSecret.len()
                );
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

        if (info.Length() < 2)
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

        if (info[0].IsObject() && info[1].IsObject()) {
            Napi::Object NBobKeyBundle   = info[0].ToObject();
            Napi::Object NAliceKeyBundle = info[1].ToObject();

            x3DHPreKeyBundleB bobKeyBundle;
            x3DHPreKeyBundleA aliceKeyBundle;

            if (this->parsePreKeyBundleA(env, NAliceKeyBundle, aliceKeyBundle)
             && this->parsePreKeyBundleB(env, NBobKeyBundle, bobKeyBundle))
            {
                Buffer sharedSecret = this->proto.calculateSharedSecret(bobKeyBundle, aliceKeyBundle);
                return Napi::Buffer<u_char>::Copy(
                    env,
                    sharedSecret.data(),
                    sharedSecret.len()
                );
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
};