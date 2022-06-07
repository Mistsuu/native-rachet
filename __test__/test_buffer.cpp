#include "Buffer.h"
#include "Calc.h"
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    Int c; c = "18275418927178429719871849174912";
    Buffer b = intToBuffer(c, false);
    Int d; d = bufferToInt(b, true);
    cout << b.toHex() << endl;
    cout << d << endl;
}