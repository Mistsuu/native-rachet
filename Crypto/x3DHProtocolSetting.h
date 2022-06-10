#pragma once

#include "../Utils/Utils.h"

// ------------------------ kdf settings ------------------------
#define KDF_INFO Buffer("mistsuuu_x3dhhh_ahhhhhh!!!")

// ------------------------ curve settings ------------------------
#define USING_CURVE25519
// #define USING_CURVE448

// ------------------------ hash settings ------------------------
#define USING_SHA512
// #define USING_SHA256
// #define USING_ELLIGATOR2

// ------------------------ key batch settings ---------------------------
#define ONETIME_PREKEYS_BATCH_SIZE 100



//
//  -- TODO: implement a virtual class that implements these Curve.
// 
#ifdef USING_CURVE25519             // Implemented.
    #include "Curve/Curve25519.h"
    typedef Curve25519 ProtocolCurve;
    #define F (Buffer('\xff') * 32)
#elif USING_CURVE448                // Not implemented yet.
    #include "Curve/Curve448.h"
    typedef Curve448 ProtocolCurve;
    #define F (Buffer('\xff') * 57)
#endif


//
//  -- TODO: implement a virtual class that handles HashClasses.
//  
#ifdef USING_SHA512
    #include "Hash/SHA512.h"
    typedef SHA512Hash ProtocolHash;
#elif  USING_SHA256
    #include "Hash/SHA256.h"
    typedef SHA256 ProtocolHash;
#elif  USING_ELLIGATOR2
    #include "Hash/Elligator2.h"
    typedef Elligator2 ProtocolHash;
#endif