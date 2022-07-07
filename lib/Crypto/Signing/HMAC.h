#pragma once

#include "Utils/Utils.h"

class HMAC
{
    Buffer padKey(Buffer key)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! HMAC.h: padKey(Buffer key) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return Buffer();
    }

    Buffer padKey(string key)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! HMAC.h: padKey(string key) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return Buffer();
    }


public:
    uint blockSize;
    uint outputSize;


    Buffer digest(Buffer key, Buffer msg)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! HMAC.h: digest(Buffer key, Buffer msg) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return Buffer();
    }

    Buffer digest(string key, Buffer msg)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! HMAC.h: digest(string key, Buffer msg) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return Buffer();
    }

    Buffer digest(Buffer key, string msg)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! HMAC.h: digest(Buffer key, string msg) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return Buffer();
    }

    Buffer digest(string key, string msg)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! HMAC.h: digest(string key, string msg) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return Buffer();
    }



    string hexDigest(Buffer key, Buffer msg)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! HMAC.h: hexDigest(Buffer key, Buffer msg) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return "";
    }

    string hexDigest(string key, Buffer msg)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! HMAC.h: hexDigest(string key, Buffer msg) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return "";
    }

    string hexDigest(Buffer key, string msg)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! HMAC.h: hexDigest(Buffer key, string msg) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return "";
    }

    string hexDigest(string key, string msg)
    {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error! HMAC.h: hexDigest(string key, string msg) is not implemented." << std::endl;
        throw NotImplementedException(errorStream.str());
        return "";
    }
};