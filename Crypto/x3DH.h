#pragma once

#include "../Utils/Utils.h"
#include "Crypto.h"

class x3DHPreKeyBundle
{
    KeyPair identityKey;                                 // Random key, generate one time.
    KeyPair signedPreKey;                                // Random key, generate one time.
    Buffer  signature;                                   // Generated from identity key and signedPreKey.
    KeyPair oneTimePreKeys[ONETIME_PREKEYS_BATCH_SIZE];  // Random key, use one time, generate if none.
};

class x3DHEncryptKeyBundleA
{
    KeyPair identityKey;
    KeyPair ephemeralKey;
};

class x3DHEncryptKeyBundleB
{
    KeyPair identityKey;
    KeyPair signedPreKey;
    KeyPair oneTimePreKey;
};

