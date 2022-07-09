#pragma once

#include <iostream>

class InvalidPointException : public std::exception 
{
private:
    std::string message;
public:
    InvalidPointException(std::string exceptionMessage) : message(exceptionMessage) {}
    std::string what() {
        return message;
    }
};

class MathErrorException : public std::exception 
{
private:
    std::string message;
public:
    MathErrorException(std::string exceptionMessage) : message(exceptionMessage) {}
    std::string what() {
        return message;
    }
};

class BufferErrorException : public std::exception 
{
private:
    std::string message;
public:
    BufferErrorException(std::string exceptionMessage) : message(exceptionMessage) {}
    std::string what() {
        return message;
    }
};

class InvalidKeyPairException : public std::exception 
{
private:
    std::string message;
public:
    InvalidKeyPairException(std::string exceptionMessage) : message(exceptionMessage) {}
    std::string what() {
        return message;
    }
};

class NotImplementedException : public std::exception 
{
private:
    std::string message;
public:
    NotImplementedException(std::string exceptionMessage) : message(exceptionMessage) {}
    std::string what() {
        return message;
    }
};

class KeyErrorException : public std::exception 
{
private:
    std::string message;
public:
    KeyErrorException(std::string exceptionMessage) : message(exceptionMessage) {}
    std::string what() {
        return message;
    }
};

class InvalidConstructorCallException : public std::exception 
{
private:
    std::string message;
public:
    InvalidConstructorCallException(std::string exceptionMessage) : message(exceptionMessage) {}
    std::string what() {
        return message;
    }
};

class VerifySignatureErrorException : public std::exception 
{
private:
    std::string message;
public:
    VerifySignatureErrorException(std::string exceptionMessage) : message(exceptionMessage) {}
    std::string what() {
        return message;
    }
};

class DeserializeErrorException : public std::exception 
{
private:
    std::string message;
public:
    DeserializeErrorException(std::string exceptionMessage) : message(exceptionMessage) {}
    std::string what() {
        return message;
    }
};

class CryptFailException : public std::exception 
{
private:
    std::string message;
public:
    CryptFailException(std::string exceptionMessage) : message(exceptionMessage) {}
    std::string what() {
        return message;
    }
};
