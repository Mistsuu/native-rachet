#pragma once

#include "Utils/Utils.h"

class BaseCrypt
{
protected:
    BaseCrypt()
    {
        // This is put as protected to prevent it from being called
        // publicly.
    }

public:
    Buffer encrypt(Buffer IV, Buffer key, Buffer plaintext)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! BaseCrypt.h: encrypt() is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return Buffer();
    }

    Buffer encryptHex(Buffer IV, Buffer key, Buffer plaintext)
    {
        return this->encrypt(IV, key, plaintext).toHex();
    }

    Buffer decrypt(Buffer IV, Buffer key, Buffer ciphertext)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! BaseCrypt.h: decrypt() is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return Buffer();
    }

    Buffer decryptHex(Buffer IV, Buffer key, Buffer ciphertext)
    {
        return this->decrypt(IV, key, ciphertext).toHex();
    }
};