#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include "../BigInteger/BigInteger.h"

using namespace std;

int main(int argc, char *argv[])
{
    for (int i = 0; i <= 19; i++)
    {
        string name1 = "test_" + intToStringWithLeadingZeros(i) + ".inp";
        string name2 = "test_" + intToStringWithLeadingZeros(i) + ".out";
        runTestCase(name1, name2);
    }
    return 1;
}