#pragma once

#include "Utils/Utils.h"
#include "BaseHash.h"

class SHA512Hash: public BaseHash
{
private:
    SHA512_CTX context;

public:
    SHA512Hash()
    {
        SHA512_Init(&this->context);
    }


    SHA512Hash(Buffer initialData)
    {
        SHA512_Init(&this->context);
        SHA512_Update(&this->context, initialData.data(), initialData.len());
    }

    SHA512Hash(string initialData)
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
        return this->digest().toHex();
    }

    inline Int intdigest(bool bigEndian=true)
    {
        return this->digest().toInt(bigEndian);
    }


    inline void update(Buffer input)
    {
        SHA512_Update(&this->context, input.data(), input.len());
    }

    inline void update(std::string input)
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