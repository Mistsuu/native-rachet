#pragma once

#include <iostream>
#include <exception>

class InvalidPointException : public std::exception 
{
private:
    std::string message;
public:
    InvalidPointException(std::string exceptionMessage) : message("\n" + exceptionMessage) {}
    virtual char const *what() const noexcept { 
        return message.c_str(); 
    }
};

class MathErrorException : public std::exception 
{
private:
    std::string message;
public:
    MathErrorException(std::string exceptionMessage) : message("\n" + exceptionMessage) {}
    virtual char const *what() const noexcept { 
        return message.c_str(); 
    }
};

class BufferErrorException : public std::exception 
{
private:
    std::string message;
public:
    BufferErrorException(std::string exceptionMessage) : message("\n" + exceptionMessage) {}
    virtual char const *what() const noexcept { 
        return message.c_str(); 
    }
};

class InvalidKeyPairException : public std::exception 
{
private:
    std::string message;
public:
    InvalidKeyPairException(std::string exceptionMessage) : message("\n" + exceptionMessage) {}
    virtual char const *what() const noexcept { 
        return message.c_str(); 
    }
};

class NotImplementedException : public std::exception 
{
private:
    std::string message;
public:
    NotImplementedException(std::string exceptionMessage) : message("\n" + exceptionMessage) {}
    virtual char const *what() const noexcept { 
        return message.c_str(); 
    }
};

class KeyErrorException : public std::exception 
{
private:
    std::string message;
public:
    KeyErrorException(std::string exceptionMessage) : message("\n" + exceptionMessage) {}
    virtual char const *what() const noexcept { 
        return message.c_str(); 
    }
};

class InvalidConstructorCallException : public std::exception 
{
private:
    std::string message;
public:
    InvalidConstructorCallException(std::string exceptionMessage) : message("\n" + exceptionMessage) {}
    virtual char const *what() const noexcept { 
        return message.c_str(); 
    }
};

class VerifySignatureErrorException : public std::exception 
{
private:
    std::string message;
public:
    VerifySignatureErrorException(std::string exceptionMessage) : message("\n" + exceptionMessage) {}
    virtual char const *what() const noexcept { 
        return message.c_str(); 
    }
};

class DeserializeErrorException : public std::exception 
{
private:
    std::string message;
public:
    DeserializeErrorException(std::string exceptionMessage) : message("\n" + exceptionMessage) {}
    virtual char const *what() const noexcept { 
        return message.c_str(); 
    }
};

class CryptFailException : public std::exception 
{
private:
    std::string message;
public:
    CryptFailException(std::string exceptionMessage) : message("\n" + exceptionMessage) {}
    virtual char const *what() const noexcept { 
        return message.c_str(); 
    }
};