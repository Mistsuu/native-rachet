#include <iostream>
#include <gmpxx.h>
#include "Random.h"
#include "Type.h"
#include "Calc.h"

using namespace std;

int main(int argc, char** argv)
{
    Int e = 65537;
    Int p = getrandprime(1024); cout << "[ i ] p: " << p << endl;
    Int q = getrandprime(1024); cout << "[ i ] q: " << q << endl;
    Int n = p*q;
    Int d = pow(e, -1, (p-1)*(q-1));

    Int m = randbelow(n);
    cout << "[ i ] Input plaintext: " << m << endl;

    Int c = pow(m, e, n);
    cout << "[ i ] Output ciphertext: " << c << endl;
    cout << "[ i ] Decrypted: " << pow(c, d, n) << endl;

    return 0;
}