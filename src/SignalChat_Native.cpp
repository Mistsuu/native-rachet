#include <napi.h>
#include <stdexcept>
#include "Crypto/SignalProtocol.h"

// Napi::String Method(const Napi::CallbackInfo& info) {
//   Napi::Env env = info.Env();
//   return Napi::String::New(env, "world");
// }

Napi::String HelloFunc(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, "seems cool!");
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "hello"),
              Napi::Function::New(env, HelloFunc));
  return exports;
}

NODE_API_MODULE(SignalChat_Native, Init)
 