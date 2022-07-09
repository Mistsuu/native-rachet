#pragma once

// --- ERROR CODES ---
#define INVALID_POINT_ERROR_CODE             -1
#define MATH_ERROR_CODE                      -2
#define BUFFER_ERROR_CODE                    -3
#define INVALID_KEYPAIR_ERROR_CODE           -4
#define NOT_IMPLEMENTED_ERROR_CODE           -5
#define EMPTY_KEY_ERROR_CODE                 -6
#define INVALID_CONSTRUCTOR_CALL_CODE        -7
#define VERIFY_SIGNATURE_ERROR_CODE          -8
#define DESERIALIZE_BUFFER_WRONG_FORMAT_CODE -9

// --- KEY RELATED ---
#define PRIVATE_KEY_NULL                     -1

// --- POINT RELATED ---
#define EMPTY_X_COORDINATE_EDWARDS           -1
#define EMPTY_COORDINATE_MONGOMERY           -1

// --- FOR PRIME CHECKING ---
#define _PRIME_CHECK_THRESHOLD               40