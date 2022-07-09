#pragma once

#include "Utils/Utils.h"

// ------------------------ kdf settings ------------------------
#define KDF_INFO Buffer("mistsuuu_x3dhhh_ahhhhhh!!!")

// ------------------------ curve settings ------------------------
#define USING_CURVE25519
// #define USING_CURVE448

// ------------------------ hash settings ------------------------
// #define USING_SHA512
#define USING_SHA256

// ------------------------ key batch settings ---------------------------
#define ONETIME_PREKEYS_BATCH_SIZE 100



//
//  -- TODO: implement a virtual class that implements these Curve.
// 
#if defined(USING_CURVE25519)
    #include "Curve/Curve25519.h"
    typedef Curve25519 ProtocolCurve;
    #define F (Buffer('\xff') * 32)
#elif defined(USING_CURVE448)
    #include "Curve/Curve448.h"
    typedef Curve448 ProtocolCurve;
    #define F (Buffer('\xff') * 57)
#endif


//
//  -- TODO: implement a virtual class that handles HashClasses.
//  
#if defined(USING_SHA512)
    #include "Hash/SHA512.h"
    #include "Signing/HMAC_SHA512.h"
    typedef SHA512Hash ProtocolHash;
    typedef HMAC_SHA512 ProtocolHMAC;
#elif defined(USING_SHA256)
    #include "Hash/SHA256.h"
    #include "Signing/HMAC_SHA256.h"
    typedef SHA256Hash ProtocolHash;
    typedef HMAC_SHA256 ProtocolHMAC;
#endif