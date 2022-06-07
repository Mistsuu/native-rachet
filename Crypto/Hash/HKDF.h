#pragma once

#include "HMAC_SHA256.h"
#include "../../Utils/Utils.h"

Buffer HKDF(int length, Buffer ikm, Buffer salt, Buffer info)
{   
    HMAC_SHA256 HMAC_SHA256Obj;

    if (salt.len() == 0)
        salt = Buffer('\x00') * HMAC_SHA256Obj.outputSize;
        
    Buffer prk = HMAC_SHA256Obj.digest(salt, ikm);
    Buffer t   = Buffer();
    Buffer okm = Buffer();
    for (int i = 0; i < ceildiv(length, HMAC_SHA256Obj.outputSize); ++i) {
        t = HMAC_SHA256Obj.digest(prk, t + info + Buffer((char)(i+1)));
        okm += t;
    }

    return okm[{0, length}];
}