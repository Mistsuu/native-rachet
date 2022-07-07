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
        std::cerr << "[ ! ] Error! BaseHash.h: digest() is not implemented." << std::endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return Buffer();
    }

    inline string hexdigest()
    {
        std::cerr << "[ ! ] Error! BaseHash.h: hexdigest() is not implemented." << std::endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return string();
    }

    inline Int intdigest(bool bigEndian=true)
    {
        std::cerr << "[ ! ] Error! BaseHash.h: intdigest() is not implemented." << std::endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return Int();
    }


    inline void update(Buffer input)
    {
        std::cerr << "[ ! ] Error! BaseHash.h: update(Buffer input) is not implemented." << std::endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    inline void update(std::string input)
    {
        std::cerr << "[ ! ] Error! BaseHash.h: update(std::string input) is not implemented." << std::endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    inline void update(const char* input, uint size)
    {
        std::cerr << "[ ! ] Error! BaseHash.h: update(const char* input, uint size) is not implemented." << std::endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    inline void update(char chr)
    {
        std::cerr << "[ ! ] Error! BaseHash.h: update(char chr) is not implemented." << std::endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }
};