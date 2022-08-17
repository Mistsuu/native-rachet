#pragma once

#include "Const.h"
#include "Type.h"
#include "Error.h"

using namespace std;

class Buffer
{
private:
    u_char* head       = NULL;
    uint    size       = 0;
    uint    actualSize = 0;

    // ----------------------------- MEM MANAGEMENT HERE -----------------------------
    //                  (could be experimented to have more fun :))
    void reallocate(uint size)
    {   
        // Size is 0, why care...?
        if (!size)
            return;

        // Allocate needed
        if (!this->head) {
            this->head       = (u_char*)malloc(size);
            this->actualSize = size;
        } else if (this->actualSize >= size) {
            return;
        } else if (this->actualSize  < size) {
            this->head       = (u_char*)realloc(this->head, size);
            this->actualSize = size;
        }

        // If after this, head still returns NULL, exit the program immediately!
        if (!this->head) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Critical Error: Buffer.h: reallocate(): Allocation failed." << std::endl;
            throw BufferErrorException(errorStream.str());
        }
    }


public:
    void __debug__()
    {
        cout << "------------------- < debug buffer> -----------------------" << std::endl;
        if (this->head) {
            cout << "[ i ] Data (0->size):       ";
            for (int i = 0; i < (int)this->size; ++i) 
                cout << setw(2) << setfill('0') << hex << (int)this->head[i];
            cout << std::endl;

            cout << "[ i ] Data (0->actualSize): ";
            for (int i = 0; i < (int)this->actualSize; ++i) 
                cout << setw(2) << setfill('0') << hex << (int)this->head[i];
            cout << std::endl;
        }
        cout << "[ i ] Size:        " << dec << this->size << std::endl;
        cout << "[ i ] Actual size: " << this->actualSize << std::endl;
        cout << "------------------- </debug buffer> -----------------------" << std::endl;
    }

    // ----------------------------- CONSTRUCTORS() -----------------------------
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

    Buffer(Buffer const &buf)
    {
        this->head       = NULL;
        this->size       = 0;
        this->actualSize = 0;
        this->set(buf);
    }

    Buffer(string const &str)
    {
        this->head       = NULL;
        this->size       = 0;
        this->actualSize = 0;
        this->set(str);
    }

    Buffer(char* str, uint size)
    {
        this->head       = NULL;
        this->size       = 0;
        this->actualSize = 0;
        this->set(str, size);
    }

    Buffer(char chr)
    {
        this->head       = (u_char*)malloc(1);
        this->size       = 1;
        this->actualSize = 1;
        this->set(chr);
    }


    // ----------------------------- PUBLIC MEM MANAGER -----------------------------
    void cleanMemory()
    {
        if (this->head) {
            free(this->head);
            this->head = NULL;
        }
        this->size       = 0;
        this->actualSize = 0;
    }

    void zeroAll()
    {
        if (this->head)
            memset(this->head, 0, this->size);
    }

    void randomAll()
    {
        if (this->head && this->size) {
            FILE* urandomFilePtr = fopen("/dev/urandom", "rb");
            if (fread(this->head, 1, this->size, urandomFilePtr) < this->size) {
                std::stringstream errorStream;
                errorStream << "[ ! ] Error! Buffer.h: randomAll(): Generate random buffer with size " << this->size << " failed." << std::endl;
                throw BufferErrorException(errorStream.str());
            }
            fclose(urandomFilePtr);
        }
    }

    inline void resize(uint size)
    {
        this->reallocate(size);
        this->size = size;
    }

    

    // ----------------------------- TEMPLATES FOR EQ= & CONSTRUCTOR WITH ARGS() -----------------------------
    void set(Buffer const &buf)
    {
        this->resize(buf.size);
        if (buf.size)
            memcpy(this->head, buf.head, buf.size);
    }

    void set(string const &str)
    {
        this->resize(str.size());
        if (str.size())
            memcpy(this->head, str.c_str(), str.size());
    }

    void set(char* str, uint size)
    {
        this->resize(size);
        if (size)
            memcpy(this->head, str, size);
    }

    void set(char chr)
    {
        this->resize(1);
        this->head[0] = chr;
    }


    // ----------------------------- LADD+= -----------------------------
    void operator+= (Buffer const &buf)
    {
        if (!buf.size)
            return;
        
        uint newSize = this->size + buf.size;
        this->reallocate(newSize);

        memcpy(&this->head[this->size], buf.head, buf.size);
        this->size = newSize;
    }

    void operator+= (string const &str)
    {
        if (!str.size())
            return;

        uint newSize = this->size + str.size();
        this->reallocate(newSize);

        memcpy(&this->head[this->size], str.c_str(), str.size());
        this->size = newSize;
    }

    void operator+= (char chr)
    {
        this->reallocate(this->size + 1);
        this->head[this->size++] = chr;
    }

    // ----------------------------- LADD+ -----------------------------
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

    // ----------------------------- EQ== -----------------------------
    bool operator== (Buffer const &buf)
    {
        return this->size == buf.size
            && this->head
            && memcmp(this->head, buf.head, this->size) == 0;
    }

    bool operator== (string const &str)
    {
        return this->size == str.size() 
            && this->head 
            && memcmp(this->head, str.c_str(), this->size) == 0;
    }

    bool operator== (char chr)
    {
        return this->size == 1 
            && this->head 
            && this->head[0] == chr;
    }


    // ----------------------------- IEQ!= -----------------------------
    bool operator!= (Buffer const &buf)
    {
        return this->size != buf.size ||
              (this->head
            && memcmp(this->head, buf.head, this->size) != 0);
    }

    bool operator!= (string const &str)
    {
        return this->size != str.size() ||
              (this->head
            && memcmp(this->head, str.c_str(), this->size) != 0);
    }

    bool operator!= (char chr)
    {
        return this->size != 1 ||
              (this->head 
            && this->head[0] != chr);
    }


    // ----------------------------- LE<= -----------------------------
    bool operator<= (Buffer const &buf)
    {
        if (this->size < buf.size)
            return true;
        if (this->size > buf.size)
            return false;
        return this->head
            && memcmp(this->head, buf.head, this->size) <= 0;
    }

    bool operator<= (string const &str)
    {
        if (this->size < str.size())
            return true;
        if (this->size > str.size())
            return false;
        return this->head 
            && memcmp(this->head, str.c_str(), this->size) <= 0;
    }

    bool operator<= (char chr)
    {
        if (!this->size)
            return true;
        if (this->size > 1)
            return false;
        return this->head 
            && this->head[0] <= chr;
    }


    // ----------------------------- LT< -----------------------------
    bool operator< (Buffer const &buf)
    {
        if (this->size < buf.size)
            return true;
        if (this->size > buf.size)
            return false;
        return this->head
            && memcmp(this->head, buf.head, this->size) < 0;
    }

    bool operator< (string const &str)
    {
        if (this->size < str.size())
            return true;
        if (this->size > str.size())
            return false;
        return this->head 
            && memcmp(this->head, str.c_str(), this->size) < 0;
    }

    bool operator< (char chr)
    {
        if (!this->size)
            return true;
        if (this->size > 1)
            return false;
        return this->head 
            && this->head[0] < chr;
    }


    // ----------------------------- GT> -----------------------------
    bool operator> (Buffer const &buf)
    {
        if (this->size < buf.size)
            return false;
        if (this->size > buf.size)
            return true;
        return this->head
            && memcmp(this->head, buf.head, this->size) > 0;
    }

    bool operator> (string const &str)
    {
        if (this->size < str.size())
            return false;
        if (this->size > str.size())
            return true;
        return this->head 
            && memcmp(this->head, str.c_str(), this->size) > 0;
    }

    bool operator> (char chr)
    {
        if (!this->size)
            return false;
        if (this->size > 1)
            return true;
        return this->head 
            && this->head[0] > chr;
    }


    // ----------------------------- GE>= -----------------------------
    bool operator>= (Buffer const &buf)
    {
        if (this->size < buf.size)
            return false;
        if (this->size > buf.size)
            return true;
        return this->head
            && memcmp(this->head, buf.head, this->size) >= 0;
    }

    bool operator>= (string const &str)
    {
        if (this->size < str.size())
            return false;
        if (this->size > str.size())
            return true;
        return this->head 
            && memcmp(this->head, str.c_str(), this->size) >= 0;
    }

    bool operator>= (char chr)
    {
        if (!this->size)
            return false;
        if (this->size > 1)
            return true;
        return this->head 
            && this->head[0] >= chr;
    }


    // ----------------------------- ASSIGN= -----------------------------
    void operator= (Buffer const &buf)
    {
        this->set(buf);
    }

    void operator= (string const &str)
    {
        this->set(str);
    }

    void operator= (char chr)
    {
        this->set(chr);
    }


    // ----------------------------- MUL* -----------------------------
    Buffer operator* (int noDup)
    {
        if (noDup < 0) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error: Buffer.h: operator*(): Trying to multiply buffer with negative number!" << std::endl;
            throw BufferErrorException(errorStream.str());
        }
        if (!noDup || !this->size)
            return Buffer();
        if (noDup == 1)
            return Buffer(*this);

        Buffer dupBuffer(this->size * noDup);
        for (int i = 0; i < noDup; ++i)
            memcpy(&dupBuffer.head[this->size * i], this->head, this->size);
        return dupBuffer;
    }


    // ----------------------------- MUL*= -----------------------------
    void operator*= (int noDup)
    {
        if (noDup < 0) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error: Buffer.h: operator*=(): Trying to multiply buffer with negative number!" << std::endl;
            throw BufferErrorException(errorStream.str());
        }

        if (!this->size || noDup == 1)
            return;

        if (!noDup) {
            this->size = 0;
            return;
        }

        this->reallocate(this->size * noDup);
        for (int i = 1; i < noDup; ++i)
            memcpy(&this->head[this->size * i], this->head, this->size);
        this->size *= noDup;
    }


    // ----------------------------- XOR^ -----------------------------
    Buffer operator^ (Buffer const &buf)
    {
        if (this->size != buf.size) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Critical Error: Buffer.h: operator^(): The item should be in equal lengths." << std::endl;
            throw BufferErrorException(errorStream.str());
        }

        Buffer newBuffer(this->size);
        for (int i = 0; i < (int)this->size; ++i)
            newBuffer.head[i] = this->head[i] ^ buf.head[i];
        return newBuffer;
    }

    Buffer operator^ (string const &str)
    {
        if (this->size != str.size()) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Critical Error: Buffer.h: operator^(): The item should be in equal lengths." << std::endl;
            throw BufferErrorException(errorStream.str());
        }

        Buffer newBuffer(this->size);
        for (int i = 0; i < (int)this->size; ++i)
            newBuffer.head[i] = this->head[i] ^ str.c_str()[i];
        return newBuffer;
    }

    Buffer operator^ (char chr)
    {
        if (this->size != 1) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Critical Error: Buffer.h: operator^(): The item should be in equal lengths." << std::endl;
            throw BufferErrorException(errorStream.str());
        }

        return Buffer(this->head[0] ^ chr);
    }


    // ----------------------------- XOR^= -----------------------------
    void operator^= (Buffer const &buf)
    {
        if (this->size != buf.size) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Critical Error: Buffer.h: operator^(): The item should be in equal lengths." << std::endl;
            throw BufferErrorException(errorStream.str());
        }

        for (int i = 0; i < (int)this->size; ++i)
            this->head[i] ^= buf.head[i];
    }

    void operator^= (string const &str)
    {
        if (this->size != str.size()) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Critical Error: Buffer.h: operator^(): The item should be in equal lengths." << std::endl;
            throw BufferErrorException(errorStream.str());
        }

        for (int i = 0; i < (int)this->size; ++i)
            this->head[i] ^= str[i];
    }

    void operator^= (char chr)
    {
        if (this->size != 1) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Critical Error: Buffer.h: operator^(): The item should be in equal lengths." << std::endl;
            throw BufferErrorException(errorStream.str());
        }

        this->head[0] ^= chr;
    }


    // ----------------------------- EXTRACT UTILS -----------------------------
    inline int filterAndConvertIndex(int index)
    {
        if (index >= (int)this->size || index < -((int)(this->size))) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error: Buffer.h: operator[]: Accessing index " << index << " is not allowed, since size = " << this->size << "." << std::endl;
            throw BufferErrorException(errorStream.str());
        }

        // If index is negative, convert it to its non-negative counterpart.
        if (index < 0)
            index += (int)this->size;
        return index;
    }

    inline u_char& operator[] (int index)
    {
        index = filterAndConvertIndex(index);
        return this->head[index];
    }

    /*
        operator[] with list as input:
            Mimic Python's [:] notation
    */
    inline Buffer operator[] (std::initializer_list<int> range)
    {
        int  lIndex;
        int  rIndex;
        int  step;
        auto pRange = range.begin();

        if (range.size() == 1) {
            rIndex = *(pRange++);

            if (rIndex < 0) {
                rIndex = filterAndConvertIndex(rIndex-1)+1;
                return Buffer((char*)&this->head[rIndex], this->size - rIndex);
            }
            
            if (rIndex > 0) {
                rIndex = filterAndConvertIndex(rIndex-1)+1;
                return Buffer((char*)this->head, rIndex);
            }

            return Buffer();

        } else if (range.size() == 2) {
            lIndex = *(pRange++);
            rIndex = *(pRange++);

            lIndex = filterAndConvertIndex(lIndex);
            if (rIndex) 
                rIndex = filterAndConvertIndex(rIndex-1)+1;
            else        
                rIndex = this->size;
            
            if (lIndex < rIndex)
                return Buffer((char*)&this->head[lIndex], rIndex-lIndex);
            return Buffer();

        } else if (range.size() == 3) {
            lIndex = *(pRange++);
            rIndex = *(pRange++);
            step   = *(pRange++);

            // Filter indices
            lIndex = filterAndConvertIndex(lIndex);
            if (rIndex)
                rIndex = filterAndConvertIndex(rIndex-1)+1;
            else        
                rIndex = this->size;

            // Return empty buffer if lIndex >= rIndex
            if (lIndex >= rIndex) 
                return Buffer();

            // Check if valid step
            if (step <= 0) {
                std::stringstream errorStream;
                errorStream << "[ ! ] Error: Buffer.h: operator[]: step should be a positive number." << std::endl;
                throw BufferErrorException(errorStream.str());
            }

            // Do some memory copying without not sure if this is
            // optimised enough...
            Buffer newBuffer((rIndex-lIndex)/step + bool((rIndex-lIndex)%step));
            for (int i=lIndex, j=0;  i < rIndex;  i+=step, j++)
                newBuffer.head[j] = this->head[i];
            return newBuffer;
        }

        std::stringstream errorStream;
        errorStream << "[ ! ] Error: Buffer.h: operator[]: NotImplementedError" << std::endl;
        throw NotImplementedException(errorStream.str());
    }
    
    u_char* data()
    {
        return this->head;
    }

    uint len()
    {
        return this->size;
    }


    // ----------------------------- HEX UTILS -----------------------------
    string toHex()
    {
        static const char* hexDigits = "0123456789abcdef";
        int    byte;
        string hexResult;
        for (int i = 0; i < (int)this->size; ++i) {
            byte       = (int)this->head[i];
            hexResult += hexDigits[byte >>   4];
            hexResult += hexDigits[byte &  0xf];
        }
        return hexResult;
    }

    static Buffer fromHex(string hexString)
    {
        // Sanity check...
        if (hexString.length() % 2 != 0) {
            std::stringstream errorStream;
            errorStream << "[ ! ] Error: Buffer.h: fromHex(): Cannot convert hexstring \"" << hexString << "\" to buffer! Invalid length: " << hexString.length() << std::endl;
            throw BufferErrorException(errorStream.str());
        }

        // Set size.
        int numBytes = hexString.length() / 2;
        Buffer newBuffer(numBytes);

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
                    std::stringstream errorStream;
                    errorStream << "[ ! ] Error: Buffer.h: fromHex(): Cannot convert hexstring \"" << hexString << "\" to buffer! Invalid hex character at position " << i*2+j << ": " << hexString[i*2+j] << std::endl;
                    throw BufferErrorException(errorStream.str());
                }
            }

            newBuffer.head[i] = byte;
        }

        return newBuffer;
    }


    // ----------------------------- NUM UTILS -----------------------------
    static Buffer fromInt(Int num, uint size, bool bigEndian=true)
    {
        uint   numBytes = mpz_sizeinbase(num.get_mpz_t(), 256);
        Buffer outputBuffer(max(numBytes, size));
        outputBuffer.zeroAll();

        if (numBytes >= size || !bigEndian) {
            mpz_export(
                outputBuffer.data(), 
                NULL, 
                1, 
                1, 
                (bigEndian ? 1:-1), 
                0, 
                num.get_mpz_t()
            );
        } else {
            mpz_export(
                &(outputBuffer.data()[size - numBytes]), 
                NULL, 
                1, 
                1, 
                (bigEndian ? 1:-1), 
                0, 
                num.get_mpz_t()
            );
        }

        return outputBuffer;
    }

    Int toInt(bool bigEndian=true)
    {
        if (!this->size)
            return Int(0);

        Int result;
        mpz_import(
            result.get_mpz_t(),
            this->size,
            1,
            1,
            (bigEndian ? 1:-1),
            0,
            this->head
        );
        return result;
    }

    
    // ----------------------------- SMART POINTER -----------------------------
    ~Buffer()
    {
        this->cleanMemory();
    }
};

// ----------------------------- OUTPUT -----------------------------
std::ostream& operator<<(std::ostream &os, Buffer buf)
{
    u_char* buf_ptr = buf.data();
    uint    buf_len = buf.len();
    for (int i = 0; i < (int)buf_len; ++i)
        os << buf_ptr[i];
    return os;
}


// ----------------------------- LADD+ -----------------------------
Buffer operator+(string const &str, Buffer buf)
{
    Buffer  newBuffer((uint)str.size() + buf.len());
    u_char* bufferPtr = newBuffer.data();
    if (str.size())
        memcpy(bufferPtr, str.c_str(), str.size());
    if (buf.len())
        memcpy(&bufferPtr[str.size()], buf.data(), buf.len());
    return newBuffer;
}

Buffer operator+(char chr, Buffer buf)
{
    Buffer  newBuffer(1 + buf.len());
    u_char* bufferPtr = newBuffer.data();
    bufferPtr[0] = chr;
    if (buf.len())
        memcpy(&bufferPtr[1], buf.data(), buf.len());
    return newBuffer;
}


// ----------------------------- LMUL* -----------------------------
Buffer operator* (int noDup, Buffer buf)
{
    if (noDup < 0) {
        std::stringstream errorStream;
        errorStream << "[ ! ] Error: Buffer.h: operator*(): Trying to multiply buffer with negative number!" << std::endl;
        throw BufferErrorException(errorStream.str());
    }
    if (!noDup || !buf.len())
        return Buffer();
    if (noDup == 1)
        return Buffer(buf);

    Buffer  dupBuffer(buf.len() * noDup);
    u_char* dupBufHead = dupBuffer.data();
    for (int i = 0; i < noDup; ++i)
        memcpy(&dupBufHead[buf.len() * i], buf.data(), buf.len());
    return dupBuffer;
}


// ----------------------------- LGT> -----------------------------
bool operator> (string const &str, Buffer buf)
{
    if (str.size() > buf.len())
        return true;
    if (str.size() < buf.len())
        return false;
    return buf.data()
        && memcmp(str.c_str(), buf.data(), buf.len()) > 0;
}

bool operator> (char chr, Buffer buf)
{
    if (!buf.len())
        return true;
    if (buf.len() > 1)
        return false;
    return buf.data() 
        && chr > buf.data()[0];
}


// ----------------------------- LGE>= -----------------------------
bool operator>= (string const &str, Buffer buf)
{
    if (str.size() > buf.len())
        return true;
    if (str.size() < buf.len())
        return false;
    return buf.data()
        && memcmp(str.c_str(), buf.data(), buf.len()) >= 0;
}

bool operator>= (char chr, Buffer buf)
{
    if (!buf.len())
        return true;
    if (buf.len() > 1)
        return false;
    return buf.data() 
        && chr >= buf.data()[0];
}


// ----------------------------- LLT< -----------------------------
bool operator< (string const &str, Buffer buf)
{
    if (str.size() > buf.len())
        return false;
    if (str.size() < buf.len())
        return true;
    return buf.data()
        && memcmp(str.c_str(), buf.data(), buf.len()) < 0;
}

bool operator< (char chr, Buffer buf)
{
    if (!buf.len())
        return false;
    if (buf.len() > 1)
        return true;
    return buf.data() 
        && chr < buf.data()[0];
}


// ----------------------------- LLE<= -----------------------------
bool operator<= (string const &str, Buffer buf)
{
    if (str.size() > buf.len())
        return false;
    if (str.size() < buf.len())
        return true;
    return buf.data()
        && memcmp(str.c_str(), buf.data(), buf.len()) <= 0;
}

bool operator<= (char chr, Buffer buf)
{
    if (!buf.len())
        return false;
    if (buf.len() > 1)
        return true;
    return buf.data() 
        && chr <= buf.data()[0];
}


// ----------------------------- LEQ== -----------------------------
bool operator== (string const &str, Buffer buf)
{
    return buf.len() == str.size() 
        && buf.data()
        && memcmp(buf.data(), str.c_str(), buf.len()) == 0;
}

bool operator== (char chr, Buffer buf)
{
    return buf.len() == 1
        && buf.data()
        && buf.data()[0] == chr;
}


// ----------------------------- LIEQ!= -----------------------------
bool operator!= (string const &str, Buffer buf)
{
    return buf.len() != str.size() ||
          (buf.data()
        && memcmp(buf.data(), str.c_str(), buf.len()) != 0);
}

bool operator!= (char chr, Buffer buf)
{
    return buf.len() != 1 ||
          (buf.data()
        && buf.data()[0] != chr);
}


// ----------------------------- XOR^ -----------------------------
Buffer operator^ (string const &str, Buffer buf)
{
    if (buf.len() != str.size()) {
        std::stringstream errorStream;
        errorStream << "[ ! ] Critical Error: Buffer.h: operator^(): The item should be in equal lengths." << std::endl;
        throw BufferErrorException(errorStream.str());
    }

    Buffer newBuffer;
    for (int i = 0; i < (int)buf.len(); ++i)
        newBuffer[i] = str[i] ^ buf[i];
    return newBuffer;
}

Buffer operator^ (char chr, Buffer buf)
{
    if (buf.len() != 1) {
        std::stringstream errorStream;
        errorStream << "[ ! ] Critical Error: Buffer.h: operator^(): The item should be in equal lengths." << std::endl;
        throw BufferErrorException(errorStream.str());
    }

    return Buffer(buf[0] ^ chr);
}


// ----------------------------- CONVERSIONS -----------------------------
Int bytesToInt(u_char* bufferInt, int size, bool bigEndian=true)
{
    Int result;
    mpz_import(
        result.get_mpz_t(),
        size,
        1,
        1,
        (bigEndian ? 1:-1),
        0,
        bufferInt
    );
    return result;
}