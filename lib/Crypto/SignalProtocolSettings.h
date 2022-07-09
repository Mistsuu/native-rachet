#pragma once

#include "Utils/Utils.h"

// ------------------------ kdf settings ------------------------
#define KDF_INFO_SIGN              Buffer("mistsuuu_x3dhhh_ahhhhhh!!!")
#define KDF_INFO_UPDATE_ROOTKEY    Buffer("su_is_going_to_be_update_root!!!")
#define KDF_INFO_GET_ENCRYPTKEY    Buffer("are_you_ready_to_be_encrypted?")
#define HMAC_INPUT_UPDATE_CHAINKEY Buffer('\x01')
#define HMAC_INPUT_GET_MESSAGEKEY  Buffer('\x02')

// ------------------------ curve settings ------------------------
#define USING_CURVE25519
// #define USING_CURVE448

// ------------------------ hash settings ------------------------
// #define USING_SHA512
#define USING_SHA256

// ------------------------ encrypt settings ------------------------
#define USING_AES256

// ------------------------ key batch settings ---------------------------
#define ONETIME_PREKEYS_BATCH_SIZE 100
#define MAX_SKIP_MESSAGE_KEYS 100

#if defined(USING_CURVE25519)
    #include "Curve/Curve25519.h"
    typedef Curve25519 ProtocolCurve;
    #define F (Buffer('\xff') * 32)
#elif defined(USING_CURVE448)
    #include "Curve/Curve448.h"
    typedef Curve448 ProtocolCurve;
    #define F (Buffer('\xff') * 57)
#endif


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


#if defined(USING_AES256)
    #include "Crypt/AES256_CBC.h"
    typedef AES256_CBCCrypt ProtocolCrypt;
#endif