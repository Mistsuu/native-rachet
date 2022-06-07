#include <iostream>
#include <gmpxx.h>
#include "Type.h"
#include "Calc.h"

using namespace std;

int main(void)
{
    Int x, n;
    cout << "x: "; cin >> x;
    cout << "n: "; cin >> n;
    cout << "sqrt(x) = " << isqrt(x) << endl;
    cout << "nthroot(x) = " << inth_root(x, n) << endl;

    return 0;
}