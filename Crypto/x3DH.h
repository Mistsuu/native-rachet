#pragma once

#include "../Utils/Type.h"
#include "../Utils/Calc.h"
#include "../Utils/Random.h"
#include "../Utils/Const.h"
#include "../Utils/Buffer.h"
#include "DiffieHellman.h"

class x3DHPreKeyBundle
{
    KeyPair identityKey;
    KeyPair signedPreKey;
    KeyPair signature;
    KeyPair oneTimePreKeys[ONETIME_PREKEYS_BATCH_SIZE];
};
