#include <iostream>
#include <gmpxx.h>
#include "Type.h"
#include "Calc.h"

using namespace std;

int main(void)
{
    Int p, e, n;

    cout << "p: "; cin >> p;
    cout << "e: "; cin >> e;
    cout << "n: "; cin >> n;
    cout << "pow(p, e, n) = " << pow(p, e, n) << endl;

    return 0;
}