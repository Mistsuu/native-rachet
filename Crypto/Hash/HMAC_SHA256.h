#pragma once

#include "../../Utils/Utils.h"
#include "SHA256.h"
#include "HMAC.h"

class HMAC_SHA256: public HMAC
{
    Buffer padKey(Buffer key)
    {
        // Return hash(key) if longer
        if (key.len() > this->blockSize)
            return SHA256Hash(key).digest();

        // Return key + '\x00\x00...' if shorter
        Buffer paddedKey(key);
        if (key.len() < this->blockSize)
            paddedKey += Buffer('\x00') * (this->blockSize - key.len());

        // (imply) if do nothing, returns the same
        return paddedKey;
    }

public:
    HMAC_SHA256()
    {
        this->blockSize  = 64;
        this->outputSize = 32;
    }

    Buffer digest(Buffer key, Buffer msg)
    {
        Buffer paddedKey = padKey(key);
        Buffer oKeyPad   = paddedKey ^ (Buffer('\x5c') * this->blockSize);
        Buffer iKeyPad   = paddedKey ^ (Buffer('\x36') * this->blockSize);
        return SHA256Hash(oKeyPad + SHA256Hash(iKeyPad + msg).digest()).digest();
    }

    inline string hexDigest(Buffer key, Buffer msg)
    {
        return this->digest(key, msg).toHex();
    }
};
