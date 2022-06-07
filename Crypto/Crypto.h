#pragma once
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#define USING_CURVE25519
// #define USING_CURVE448

#define USING_SHA512
// #define USING_SHA256
// #define USING_ELLIGATOR2

//
//  -- TODO: implement a virtual class that implements these Curve.
// 
#ifdef USING_CURVE25519             // Implemented.
    #include "Curve/Curve25519.h"
    typedef Curve25519 Curve;
#elif USING_CURVE448                // Not implemented yet.
    #include "Curve/Curve448.h"
    typedef Curve448 Curve;
#endif


//
//  -- TODO: implement a virtual class that handles HashClasses.
//  
#ifdef USING_SHA512
    #include "Hash/SHA512.h"
    typedef SHA512Hash HashClass;
#elif  USING_SHA256
    #include "Hash/SHA256.h"
    typedef SHA256 HashClass;
#elif  USING_ELLIGATOR2
    #include "Hash/Elligator2.h"
    typedef Elligator2 HashClass;
#endif

#pragma GCC diagnostic pop