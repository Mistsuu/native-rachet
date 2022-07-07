#pragma once

#include "Utils/Utils.h"
#include "PointMongomery.h"

// ============================================== KEY PAIR ==============================================

class KeyPair 
{
public:
    PointMongomery publicKey;
    Int            privateKey;
};