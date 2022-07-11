#pragma once

#include <napi.h>
#include "Crypto/SignalProtocol.h"

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
                        {InstanceMethod("GeneratePreKeyBundle", &SignalProto_Native::GeneratePreKeyBundle)
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

    Napi::Value GeneratePreKeyBundle(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();
        x3DHPreKeyBundle preKeyBundle = this->proto.generatePreKeyBundle();
        Napi::Object x3DHPreKeyBundleObj = Napi::Object::New(env);
        x3DHPreKeyBundleObj.Set(Napi::String::New(env, "msg"), "heyheyeasy");
        return x3DHPreKeyBundleObj;
    }
};