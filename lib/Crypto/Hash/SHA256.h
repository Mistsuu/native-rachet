#pragma once

#include "Utils/Utils.h"
#include "BaseHash.h"

class SHA256Hash: public BaseHash
{
private:
    SHA256_CTX context;

public:
    SHA256Hash()
    {
        SHA256_Init(&this->context);
    }


    SHA256Hash(Buffer initialData)
    {
        SHA256_Init(&this->context);
        SHA256_Update(&this->context, initialData.data(), initialData.len());
    }

    SHA256Hash(string initialData)
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
        return this->digest().toHex();
    }

    inline Int intdigest(bool bigEndian=true)
    {
        return this->digest().toInt(bigEndian);
    }


    inline void update(Buffer input)
    {
        SHA256_Update(&this->context, input.data(), input.len());
    }

    inline void update(std::string input)
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