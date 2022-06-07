#pragma once

#include "../../Utils/Utils.h"

class HMAC
{
    void padKey(Buffer key)
    {
        cerr << "[ ! ] Error! HMAC.h: padKey(Buffer key) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    void padKey(string key)
    {
        cerr << "[ ! ] Error! HMAC.h: padKey(string key) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }


public:
    uint blockSize;
    uint outputSize;


    void digest(Buffer key, Buffer msg)
    {
        cerr << "[ ! ] Error! HMAC.h: digest(Buffer key, Buffer msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    void digest(string key, Buffer msg)
    {
        cerr << "[ ! ] Error! HMAC.h: digest(string key, Buffer msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    void digest(Buffer key, string msg)
    {
        cerr << "[ ! ] Error! HMAC.h: digest(Buffer key, string msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    void digest(string key, string msg)
    {
        cerr << "[ ! ] Error! HMAC.h: digest(string key, string msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }



    void hexDigest(Buffer key, Buffer msg)
    {
        cerr << "[ ! ] Error! HMAC.h: hexDigest(Buffer key, Buffer msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    void hexDigest(string key, Buffer msg)
    {
        cerr << "[ ! ] Error! HMAC.h: hexDigest(string key, Buffer msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    void hexDigest(Buffer key, string msg)
    {
        cerr << "[ ! ] Error! HMAC.h: hexDigest(Buffer key, string msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }

    void hexDigest(string key, string msg)
    {
        cerr << "[ ! ] Error! HMAC.h: hexDigest(string key, string msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
    }
};