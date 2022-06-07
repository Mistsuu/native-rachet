#pragma once

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include "Const.h"

using namespace std;

class Buffer
{
private:
    u_char* head           = NULL;
    uint    size           = 0;
    uint    actualSize     = 0;

    void reallocate(uint size)
    {
        if (this->size > size) {
            cerr << "[ ! ] Critical Error: Buffer.h: reallocate(): Forcing new size < actual size." << endl;
            exit(BUFFER_ERROR_CODE);
        } else if (!this->head) {
            this->head       = (u_char*)malloc(size);
            this->actualSize = size;
        } else if (this->actualSize < size) {
            this->head       = (u_char*)realloc(this->head, size);
            this->actualSize = size;
        }

        // If after this, allocate returns NULL, exit the program immediately!
        if (!this->head) {
            cerr << "[ ! ] Critical Error: Buffer.h: reallocate(): Allocation failed." << endl;
            exit(BUFFER_ERROR_CODE);
        }
    }


public:
    void __debug__()
    {
        cout << "------------------- < debug buffer> -----------------------" << endl;
        if (this->head) {
            cout << "[ i ] Data (0->size):       ";
            for (int i = 0; i < this->size; ++i) 
                cout << setw(2) << setfill('0') << hex << (int)this->head[i];
            cout << endl;

            cout << "[ i ] Data (0->actualSize): ";
            for (int i = 0; i < this->actualSize; ++i) 
                cout << setw(2) << setfill('0') << hex << (int)this->head[i];
            cout << endl;
        }
        cout << "[ i ] Size:        " << dec << this->size << endl;
        cout << "[ i ] Actual size: " << this->actualSize << endl;
        cout << "------------------- </debug buffer> -----------------------" << endl;

    }


    Buffer()
    {
        this->head       = NULL;
        this->size       = 0;
        this->actualSize = 0;
    }

    Buffer(uint size)
    {
        if (size) {
            this->head       = (u_char*)malloc(size);
            this->size       = size;
            this->actualSize = size;
        } else {
            this->head       = NULL;
            this->size       = 0;
            this->actualSize = 0;
        }
    }

    Buffer(int size)
    {
        if (size > 0) {
            this->head       = (u_char*)malloc(size);
            this->size       = (uint)size;
            this->actualSize = (uint)size;
        } else {
            this->head       = NULL;
            this->size       = 0;
            this->actualSize = 0;
        }
    }


    void cleanSelf()
    {
        if (this->head) {
            free(this->head);
            this->head       = NULL;
            this->size       = 0;
            this->actualSize = 0;
        }
    }


    void copyFrom(Buffer const &buf)
    {
        if (!buf.size)
            return this->cleanSelf();

        this->reallocate(buf.size);
        this->size = buf.size;
        memcpy(this->head, buf.head, buf.size);
    }

    void copyFrom(string const &str)
    {
        if (!str.size())
            return this->cleanSelf();

        this->reallocate(str.size());
        this->size = str.size();
        memcpy(this->head, str.c_str(), str.size());
    }

    void copyFrom(char chr)
    {
        this->reallocate(1);
        this->size = 1;
        this->head[0] = chr;
    }


    void operator+= (Buffer const &buf)
    {
        if (!buf.size)
            return;
        
        uint newSize = this->size + buf.size;
        if (newSize > this->actualSize)
            this->reallocate(newSize);

        memcpy(&this->head[this->size], buf.head, buf.size);
        this->size = newSize;
    }

    void operator+= (string const &str)
    {
        if (!str.size())
            return;

        uint newSize = this->size + str.size();
        if (newSize > this->actualSize)
            this->reallocate(newSize);

        memcpy(&this->head[this->size], str.c_str(), str.size());
        this->size = newSize;
    }

    void operator+= (char chr)
    {
        if (this->size + 1 > this->actualSize)
            this->reallocate(this->size + 1);

        this->head[this->size] = chr;
        this->size++;
    }


    Buffer operator+ (Buffer const &buf)
    {
        Buffer newBuffer(this->size + buf.size);
        if (this->size)
            memcpy(newBuffer.head, this->head, this->size);
        if (buf.size)
            memcpy(&newBuffer.head[this->size], buf.head, buf.size);
        return newBuffer;
    }

    Buffer operator+ (string const &str)
    {
        Buffer newBuffer(this->size + (uint)str.size());
        if (this->size)
            memcpy(newBuffer.head, this->head, this->size);
        if (str.size())
            memcpy(&newBuffer.head[this->size], str.c_str(), str.size());
        return newBuffer;
    }

    Buffer operator+ (char chr)
    {
        Buffer newBuffer(this->size + 1);
        if (this->size)
            memcpy(newBuffer.head, this->head, this->size);
        newBuffer.head[this->size] = chr;
        return newBuffer;
    }


    Buffer(Buffer const &buf)
    {
        this->head       = NULL;
        this->size       = 0;
        this->actualSize = 0;
        this->copyFrom(buf);
    }

    Buffer(string const &str)
    {
        this->head       = NULL;
        this->size       = 0;
        this->actualSize = 0;
        this->copyFrom(str);
    }

    Buffer(char chr)
    {
        this->head       = NULL;
        this->size       = 0;
        this->actualSize = 0;
        this->copyFrom(chr);
    }


    void operator= (Buffer const &buf)
    {
        this->copyFrom(buf);
    }

    void operator= (string const &str)
    {
        this->copyFrom(str);
    }

    void operator= (char chr)
    {
        this->copyFrom(chr);
    }


    inline u_char& operator[] (uint index)
    {
        if (index >= this->size) {
            cerr << "[ ! ] Error: Buffer.h: operator[]: Accessing index " << index << " is not allowed, since size = " << this->size << "." << endl;
            exit(BUFFER_ERROR_CODE);
        }

        return this->head[index];
    }

    u_char* data()
    {
        return this->head;
    }

    uint len()
    {
        return this->size;
    }


    string toHex()
    {
        static const char* digits = "0123456789abcdef";
        int    byte;
        string hexResult;
        for (int i = 0; i < this->size; ++i) {
            byte       = (int)this->head[i];
            hexResult += digits[byte >>   4];
            hexResult += digits[byte &  0xf];
        }
        return hexResult;
    }

    void fromHex(string hexString)
    {
        // Sanity check...
        if (hexString.length() % 2 != 0) {
            cerr << "[ ! ] Error: Buffer.h: fromHex(): Cannot convert hexstring \"" << hexString << "\" to buffer! Invalid length: " << hexString.length() << endl;
            exit(BUFFER_ERROR_CODE);
        }

        // Set size.
        int numBytes = hexString.length() / 2;
        this->reallocate(numBytes);
        this->size = numBytes;

        // Loop through hex string...
        unsigned char byte;
        for (int i = 0; i < numBytes; ++i) {
            byte = 0;

            for (int j = 0; j < 2; ++j)
            {
                byte <<= 4;

                if      ('0' <= hexString[i*2+j] && hexString[i*2+j] <= '9') 
                    byte += hexString[i*2+j] - '0';
                else if ('A' <= hexString[i*2+j] && hexString[i*2+j] <= 'F')
                    byte += hexString[i*2+j] - 'A' + 10;
                else if ('a' <= hexString[i*2+j] && hexString[i*2+j] <= 'f')
                    byte += hexString[i*2+j] - 'a' + 10;
                else 
                {
                    cerr << "[ ! ] Error: Buffer.h: fromHex(): Cannot convert hexstring \"" << hexString << "\" to buffer! Invalid hex character at position " << i*2+j << ": " << hexString[i*2+j] << endl;
                    exit(BUFFER_ERROR_CODE);
                }
            }

            this->head[i] = byte;
        }
    }


    ~Buffer()
    {
        this->cleanSelf();
    }
};


std::ostream& operator<<(std::ostream &os, Buffer &buf)
{
    u_char* buf_ptr = buf.data();
    uint    buf_len = buf.len();
    for (int i = 0; i < buf_len; ++i)
        os << buf_ptr[i];
    return os;
}


Buffer operator+(std::string &str, Buffer &buf)
{
    Buffer  newBuffer((uint)str.size() + buf.len());
    u_char* bufferPtr = newBuffer.data();
    if (str.size())
        memcpy(bufferPtr, str.c_str(), str.size());
    if (buf.len())
        memcpy(&bufferPtr[str.size()], buf.data(), buf.len());
    return newBuffer;
}

Buffer operator+(const char* str, Buffer &buf)
{
    uint    strLen    = strlen(str);
    Buffer  newBuffer(strLen + buf.len());
    u_char* bufferPtr = newBuffer.data();
    if (strLen)
        memcpy(bufferPtr, str, strLen);
    if (buf.len())
        memcpy(&bufferPtr[strLen], buf.data(), buf.len());
    return newBuffer;
}

Buffer operator+(char chr, Buffer &buf)
{
    Buffer  newBuffer(1 + buf.len());
    u_char* bufferPtr = newBuffer.data();
    bufferPtr[0] = chr;
    if (buf.len())
        memcpy(&bufferPtr[1], buf.data(), buf.len());
    return newBuffer;
}