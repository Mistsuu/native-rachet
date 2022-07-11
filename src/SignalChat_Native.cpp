#include "SignalProto_Native.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    SignalProto_Native::Init(env, exports);
    return exports;
}

NODE_API_MODULE(SignalChat_Native, InitAll)
