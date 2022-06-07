#pragma once

#include "../../Utils/Utils.h"

class Hash
{
public:
    void digest()
    {
        cerr << "[ ! ] Error! Hash.h: digest() is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    void hexdigest()
    {
        cerr << "[ ! ] Error! Hash.h: hexDigest() is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }


    inline void update(Buffer& input)
    {
        cerr << "[ ! ] Error! Hash.h: update(Buffer& input) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    inline void update(string& input)
    {
        cerr << "[ ! ] Error! Hash.h: update(string& input) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    inline void update(const char* input, uint size)
    {
        cerr << "[ ! ] Error! Hash.h: update(const char* input, uint size) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    inline void update(char chr)
    {
        cerr << "[ ! ] Error! Hash.h: update(char chr) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }
};