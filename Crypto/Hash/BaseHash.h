#pragma once

#include "../../Utils/Utils.h"

class BaseHash
{
protected:
    BaseHash()
    {
        // This is put as protected to prevent it from being called
        // publicly.
    }

public:
    void digest()
    {
        cerr << "[ ! ] Error! BaseHash.h: digest() is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    void hexdigest()
    {
        cerr << "[ ! ] Error! BaseHash.h: hexDigest() is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }


    inline void update(Buffer& input)
    {
        cerr << "[ ! ] Error! BaseHash.h: update(Buffer& input) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    inline void update(string& input)
    {
        cerr << "[ ! ] Error! BaseHash.h: update(string& input) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    inline void update(const char* input, uint size)
    {
        cerr << "[ ! ] Error! BaseHash.h: update(const char* input, uint size) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    inline void update(char chr)
    {
        cerr << "[ ! ] Error! BaseHash.h: update(char chr) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }
};