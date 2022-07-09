#pragma once

#include "Utils/Utils.h"
#include "BaseCrypt.h"

#define AES_KEYLENGTH 256
class AES256_CBCCrypt: public BaseCrypt
{
public:
    Buffer pad(Buffer plaintext)
    {
        int padLength = (AES_BLOCK_SIZE - (plaintext.len() % AES_BLOCK_SIZE));
        return plaintext + Buffer((char)padLength) * padLength;
    }

    Buffer unpad(Buffer plaintext)
    {
        for (int i = 1; i <= 16; ++i)
            if (plaintext[{-i}] == Buffer((char)i) * i)
                return plaintext[{0, -i}];
        return plaintext;
    }

    Buffer encrypt(Buffer IV, Buffer key, Buffer plaintext)
    {
        if (IV.len() != AES_BLOCK_SIZE) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! AES256_CBCCrypt.h: encrypt(): IV.length is not " << AES_BLOCK_SIZE << "! Got: " << IV.len() << std::endl;
            errorStream << "[ ! ]      IV (in hex): " << IV.toHex() << endl;
            throw CryptFailException(errorStream.str());
            return Buffer();
        }

        if (key.len() != AES_KEYLENGTH / 8) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! AES256_CBCCrypt.h: encrypt(): key.length is not " << AES_KEYLENGTH / 8 << "! Got: " << key.len() << std::endl;
            errorStream << "[ ! ]      key (in hex): " << key.toHex() << endl;
            throw CryptFailException(errorStream.str());
            return Buffer();
        }

        AES_KEY encryptKey;
        AES_set_encrypt_key(key.data(), AES_KEYLENGTH, &encryptKey);

        Buffer _IV = IV;
        Buffer paddedPlaintext = this->pad(plaintext);
        Buffer ciphertext = paddedPlaintext.len();
        AES_cbc_encrypt(paddedPlaintext.data(), ciphertext.data(), paddedPlaintext.len(), &encryptKey, _IV.data(), AES_ENCRYPT);

        return ciphertext;
    }

    Buffer decrypt(Buffer IV, Buffer key, Buffer ciphertext)
    {
        if (IV.len() != AES_BLOCK_SIZE) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! AES256_CBCCrypt.h: decrypt(): IV.length is not " << AES_BLOCK_SIZE << "! Got: " << IV.len() << std::endl;
            errorStream << "[ ! ]      IV (in hex): " << IV.toHex() << endl;
            throw CryptFailException(errorStream.str());
            return Buffer();
        }

        if (key.len() != AES_KEYLENGTH / 8) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! AES256_CBCCrypt.h: decrypt(): key.length is not " << AES_KEYLENGTH / 8 << "! Got: " << key.len() << std::endl;
            errorStream << "[ ! ]      key (in hex): " << key.toHex() << endl;
            throw CryptFailException(errorStream.str());
            return Buffer();
        }

        if (ciphertext.len() % AES_BLOCK_SIZE != 0) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error! AES256_CBCCrypt.h: decrypt(): ciphertext.length does not divisible by " << AES_BLOCK_SIZE << "! Got: " << ciphertext.len() << std::endl;
            errorStream << "[ ! ]      ciphertext (in hex): " << ciphertext.toHex() << endl;
            throw CryptFailException(errorStream.str());
            return Buffer();
        }

        AES_KEY decryptKey;
        AES_set_decrypt_key(key.data(), AES_KEYLENGTH, &decryptKey);

        Buffer _IV = IV;
        Buffer plaintext = Buffer(ciphertext.len());
        AES_cbc_encrypt(ciphertext.data(), plaintext.data(), ciphertext.len(), &decryptKey, _IV.data(), AES_DECRYPT);

        return this->unpad(plaintext);
    }
};