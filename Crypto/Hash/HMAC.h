#pragma once

#include "../../Utils/Utils.h"

class HMAC
{
    Buffer padKey(Buffer key)
    {
        cerr << "[ ! ] Error! HMAC.h: padKey(Buffer key) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return Buffer();
    }

    Buffer padKey(string key)
    {
        cerr << "[ ! ] Error! HMAC.h: padKey(string key) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return Buffer();
    }


public:
    uint blockSize;
    uint outputSize;


    Buffer digest(Buffer key, Buffer msg)
    {
        cerr << "[ ! ] Error! HMAC.h: digest(Buffer key, Buffer msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return Buffer();
    }

    Buffer digest(string key, Buffer msg)
    {
        cerr << "[ ! ] Error! HMAC.h: digest(string key, Buffer msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return Buffer();
    }

    Buffer digest(Buffer key, string msg)
    {
        cerr << "[ ! ] Error! HMAC.h: digest(Buffer key, string msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return Buffer();
    }

    Buffer digest(string key, string msg)
    {
        cerr << "[ ! ] Error! HMAC.h: digest(string key, string msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return Buffer();
    }



    string hexDigest(Buffer key, Buffer msg)
    {
        cerr << "[ ! ] Error! HMAC.h: hexDigest(Buffer key, Buffer msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return "";
    }

    string hexDigest(string key, Buffer msg)
    {
        cerr << "[ ! ] Error! HMAC.h: hexDigest(string key, Buffer msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return "";
    }

    string hexDigest(Buffer key, string msg)
    {
        cerr << "[ ! ] Error! HMAC.h: hexDigest(Buffer key, string msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return "";
    }

    string hexDigest(string key, string msg)
    {
        cerr << "[ ! ] Error! HMAC.h: hexDigest(string key, string msg) is not implemented." << endl;
        exit(NOT_IMPLEMENTED_ERROR_CODE);
        return "";
    }
};