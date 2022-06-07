#pragma once

#include "../../Utils/Utils.h"
#include <openssl/sha.h>

using namespace std;

class SHA512Hash
{
private:
    SHA512_CTX context;

public:
    SHA512Hash()
    {
        SHA512_Init(&this->context);
    }


    SHA512Hash(Buffer& initialData)
    {
        SHA512_Init(&this->context);
        SHA512_Update(&this->context, initialData.data(), initialData.len());
    }

    SHA512Hash(string& initialData)
    {
        SHA512_Init(&this->context);
        SHA512_Update(&this->context, initialData.c_str(), initialData.size());
    }

    SHA512Hash(const char* initialData, uint size)
    {
        SHA512_Init(&this->context);
        SHA512_Update(&this->context, initialData, size);
    }

    SHA512Hash(char chr)
    {
        SHA512_Init(&this->context);
        SHA512_Update(&this->context, &chr, 1);
    }


    inline Buffer digest()
    {
        Buffer digest(SHA512_DIGEST_LENGTH);
        SHA512_Final(digest.data(), &this->context);
        return digest;
    }

    inline string hexdigest()
    {
        Buffer      digest(SHA512_DIGEST_LENGTH);
        string      hexDigest = "";
        const char* digits = "0123456789abcdef";
        
        SHA512_Final(digest.data(), &this->context);
        for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
            hexDigest += digits[digest[i] >>   4];
            hexDigest += digits[digest[i] &  0xf];
        }

        return hexDigest;
    }


    inline void update(Buffer& input)
    {
        SHA512_Update(&this->context, input.data(), input.len());
    }

    inline void update(string& input)
    {
        SHA512_Update(&this->context, input.c_str(), input.length());
    }

    inline void update(const char* input, uint size)
    {
        SHA512_Update(&this->context, input, size);
    }

    inline void update(char chr)
    {
        SHA512_Update(&this->context, &chr, 1);
    }


    ~SHA512Hash()
    {
    }
};