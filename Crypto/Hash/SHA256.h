#pragma once

#include "../../Utils/Utils.h"
#include "Hash.h"
#include <openssl/sha.h>

using namespace std;

class SHA256Hash: public Hash
{
private:
    SHA256_CTX context;

public:
    SHA256Hash()
    {
        SHA256_Init(&this->context);
    }


    SHA256Hash(Buffer& initialData)
    {
        SHA256_Init(&this->context);
        SHA256_Update(&this->context, initialData.data(), initialData.len());
    }

    SHA256Hash(string& initialData)
    {
        SHA256_Init(&this->context);
        SHA256_Update(&this->context, initialData.c_str(), initialData.length());
    }

    SHA256Hash(const char* initialData, uint size)
    {
        SHA256_Init(&this->context);
        SHA256_Update(&this->context, initialData, size);
    }

    SHA256Hash(char chr)
    {
        SHA256_Init(&this->context);
        SHA256_Update(&this->context, &chr, 1);
    }


    inline Buffer digest()
    {
        Buffer digest(SHA256_DIGEST_LENGTH);
        SHA256_Final(digest.data(), &this->context);
        return digest;
    }

    inline string hexdigest()
    {
        Buffer      digest(SHA256_DIGEST_LENGTH);
        string      hexDigest = "";
        const char* digits = "0123456789abcdef";
        
        SHA256_Final(digest.data(), &this->context);
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            hexDigest += digits[digest[i] >>   4];
            hexDigest += digits[digest[i] &  0xf];
        }

        return hexDigest;
    }


    inline void update(Buffer& input)
    {
        SHA256_Update(&this->context, input.data(), input.len());
    }

    inline void update(string& input)
    {
        SHA256_Update(&this->context, input.c_str(), input.size());
    }

    inline void update(const char* input, uint size)
    {
        SHA256_Update(&this->context, input, size);
    }

    inline void update(char chr)
    {
        SHA256_Update(&this->context, &chr, 1);
    }


    ~SHA256Hash()
    {
    }
};