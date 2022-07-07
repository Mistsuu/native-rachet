#pragma once

#include "HMAC_SHA256.h"
#include "Utils/Utils.h"

Buffer HKDF(int length, Buffer inputKeyMaterial, Buffer salt, Buffer info)
{   
    HMAC_SHA256 HMAC_SHA256Obj;

    if (salt.len() == 0)
        salt = Buffer('\x00') * HMAC_SHA256Obj.outputSize;
        
    Buffer prk = HMAC_SHA256Obj.digest(salt, inputKeyMaterial);
    Buffer tmp = Buffer();
    Buffer outputKeyMaterial = Buffer();
    for (int i = 0; i < ceildiv(length, HMAC_SHA256Obj.outputSize); ++i) {
        tmp = HMAC_SHA256Obj.digest(prk, tmp + info + Buffer((char)(i+1)));
        outputKeyMaterial += tmp;
    }

    return outputKeyMaterial[{length}];
}