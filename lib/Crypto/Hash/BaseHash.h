#pragma once

#include "Utils/Utils.h"

class BaseHash
{
protected:
    BaseHash()
    {
        // This is put as protected to prevent it from being called
        // publicly.
    }

public:
    Buffer digest()
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! BaseHash.h: digest() is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return Buffer();
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
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! BaseHash.h: update(Buffer input) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
    }

    inline void update(std::string input)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! BaseHash.h: update(std::string input) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
    }

    inline void update(const char* input, uint size)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! BaseHash.h: update(const char* input, uint size) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
    }

    inline void update(char chr)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! BaseHash.h: update(char chr) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
    }
};