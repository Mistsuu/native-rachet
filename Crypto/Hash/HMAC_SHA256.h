#pragma once

#include "../../Utils/Utils.h"
#include "SHA256.h"
#include "HMAC.h"

class HMAC_SHA256: public HMAC
{
public:
    Buffer digest(Buffer key, Buffer msg)
    {
        return Buffer();
    }
};
