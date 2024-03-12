#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
int _count = 0;
using namespace std;

#define MAX 64 //(64x32=2048 bit)

class BigInteger // BigInteger 64byte
{
private:
    const static unsigned long long base = (1ULL << 32);
    unsigned int number[MAX] = {0};
    int sign = 1;
    int max_size = 1;

public:
    BigInteger(long long int a)
    {
        long long int tmp = abs(a);
        number[0] = unsigned(int(tmp));
        number[1] = tmp >> 32;
        sign = a < 0 ? -1 : 1;
        RecomputeSize();
    }
    void RecomputeSize()
    {
        max_size = 1;
        for (int i = MAX - 1; i >= 0; i--)
        {
            if (number[i] != 0)
            {
                max_size = i + 1;
                break;
            }
        }
        _count++;
    }
    BigInteger(string hexString)
    {
        int length = hexString.size();
        int current = length;
        for (int i = 0; i < MAX; i++)
        {

            int start = length - 8 * (i + 1);
            int num = 8;
            if (start <= 0 && current > 0)
            {
                num = current;
                start = 0;
            }
            if (current > 0)
            {
                string tmp = hexString.substr(start, num);
                stringstream ss;
                ss << std::hex << tmp;
                ss >> number[i];
            }
            else
                number[i] = 0;
            current -= 8;
        }
        RecomputeSize();
    }
    BigInteger(const BigInteger &other)
    {
        memcpy(&number, &other.number, sizeof(number));
        sign = other.sign;
        RecomputeSize();
    }
    BigInteger()
    {
    }
    void operator=(const BigInteger &other)
    {
        memcpy(&number, &other.number, sizeof(number));
        sign = other.sign;
        max_size = other.max_size;
        RecomputeSize();
    }

    BigInteger operator<<(int shift) const
    {
        int fullShifts = shift / 32;
        int savebit = 0;
        int remainingShift = shift % 32;
        int reverse = (32 - remainingShift);
        BigInteger result(0);
        for (int i = 0; i < MAX; i++)
        {
            if (i < fullShifts)
            {
                result.number[i] = 0;
            }
            if (i + fullShifts < MAX)
            {
                result.number[i + fullShifts] = number[i] << remainingShift | savebit;
            }
            savebit = (unsigned long long int)number[i] >> reverse;
        }
        result.RecomputeSize();
        return result;
    }

    bool operator>=(const BigInteger &a) const
    {
        if (sign == -1 && a.sign == 1)
            return false;
        if (sign == 1 && a.sign == -1)
            return true;
        if (max_size != a.max_size)
        {
            return (max_size > a.max_size) == (sign == 1);
        }
        for (int i = max_size - 1; i >= 0; i--)
        {
            if (number[i] > a.number[i])
            {
                return (sign == 1);
            }
            else if (number[i] < a.number[i])
            {
                return (sign == -1);
            }
        }

        return true;
    }

    bool operator<=(const BigInteger &a) const
    {
        if (sign == -1 && a.sign == 1)
            return true;
        if (sign == 1 && a.sign == -1)
            return false;
        if (max_size != a.max_size)
        {
            return (max_size < a.max_size) == (sign == 1);
        }
        for (int i = max_size - 1; i >= 0; i--)
        {
            if (number[i] < a.number[i])
            {
                return (sign != -1);
            }
            else if (number[i] > a.number[i])
            {
                return (sign != 1);
            }
        }
        return true;
    }
    bool operator>(const BigInteger &a) const
    {
        if (sign == -1 && a.sign == 1)
            return false;
        if (sign == 1 && a.sign == -1)
            return true;
        if (max_size != a.max_size)
        {
            return (max_size > a.max_size) == (sign == 1);
        }
        for (int i = max_size - 1; i >= 0; i--)
        {
            if (number[i] > a.number[i])
            {
                return (sign == 1);
            }
            else if (number[i] < a.number[i])
            {
                return (sign == -1);
            }
        }
        return false;
    }

    static BigInteger Abs(const BigInteger &a)
    {
        BigInteger x(a);
        x.sign = 1;
        return x;
    }
    bool operator==(const BigInteger &a) const
    {
        if (sign != a.sign || a.max_size != max_size)
            return false;

        for (int i = max_size - 1; i >= 0; i--)
        {
            if (number[i] != a.number[i])
            {
                return false;
            }
        }

        return true;
    }

    string ToString() const
    {
        if (*this == 0)
        {
            return "0";
        }
        string result("");
        BigInteger numCopy = Abs(*this);
        while (numCopy > 0)
        {
            BigInteger remainder = numCopy % 10;
            result = char(remainder.ToInteger() + '0') + result;
            numCopy = numCopy / 10;
        }

        return ((sign == -1) ? "-" : "") + result;
    }

    BigInteger operator+(const BigInteger &other) const
    {
        if (sign == other.sign)
        {
            BigInteger result(0);

            unsigned long long carry = 0;
            for (int i = 0; i < max(max_size, other.max_size) + 1; i++)
            {
                long long int sum = static_cast<long long int>(number[i]) +
                                    static_cast<long long int>(other.number[i]) + carry;
                result.number[i] = static_cast<unsigned int>(sum);
                carry = sum >> 32;
            }

            result.sign = sign;
            return result;
        }
        else
        {
            return sign == 1 ? *this - (-other) : other - (-*this);
        }
    }

    friend BigInteger operator-(const BigInteger &a)
    {
        BigInteger result(a);
        result.sign = a.sign * -1;
        return result;
    }
    BigInteger operator-(const BigInteger &other) const
    {
        if (sign == other.sign && sign == 1)
        {
            if (!(*this >= other))
            {
                return -(other - *this);
            }

            BigInteger result(*this);
            int carry = 0;
            for (int i = 0; i < (max(max_size, other.max_size)); i++)
            {
                long long int diff = static_cast<long long int>(result.number[i]) -
                                     static_cast<long long int>(other.number[i]) - carry;
                if (diff < 0)
                {
                    diff += base;
                    carry = 1;
                }
                else
                {
                    carry = 0;
                }
                result.number[i] = static_cast<unsigned int>(diff);
            }
            if (carry)
            {
                result.sign = -1;
            }

            return result;
        }
        else if (sign == -1 && other.sign == 1)
        {
            BigInteger x = -((-*this) + other);
            return x;
        }
        else if (sign == 1 && other.sign == -1)
        {
            return (*this + (-other));
        }
        else if (sign == -1 && other.sign == -1)
        {
            return (-other) - (-*this);
        }
        return 0;
    }
    BigInteger operator%(unsigned long long a) const
    {
        unsigned long long remainder = 0;
        for (int i = max_size - 1; i >= 0; i--)
        {
            remainder = (remainder * base + number[i]) % a;
        }
        return remainder;
    }
    BigInteger operator%(const BigInteger &a) const
    {
        BigInteger dividend = Abs(*this);
        BigInteger divisor = Abs(a);
        while (dividend >= divisor)
        {
            int shift = max((dividend.max_size - divisor.max_size - 1) * 32, 0);
            int mid = 32;
            while (mid != 0)
            {
                if (dividend >= (divisor << (shift + mid)))
                {
                    shift += mid;
                }
                mid /= 2;
            }
            dividend = dividend - (divisor << shift);
        }
        return dividend;
    }
    BigInteger operator/(const BigInteger &a) const
    {
        BigInteger dividend = Abs(*this);
        BigInteger divisor = Abs(a);
        BigInteger quotient(0);
        BigInteger one(1);
        while (dividend >= divisor)
        {
            int shift = max((dividend.max_size - divisor.max_size - 1) * 32, 0);
            int mid = 32;

            while (mid != 0)
            {
                if (dividend >= (divisor << (shift + mid)))
                {
                    shift += mid;
                }
                mid /= 2;
            }
            dividend = dividend - (divisor << shift);
            quotient = quotient + (one << shift);
        }
        quotient.sign = sign * a.sign;
        return quotient;
    }

    static BigInteger Random(const BigInteger &n)
    {
        BigInteger result(0);
        for (int i = 0; i < n.max_size; i++)
        {
            result.number[i] = (rand() + rand() * 0xFFFFFF);
        }
        if (result <= 1)
            result = 2;
        if (result >= (n - 2))
            result = (n - 2);
        return result;
    }

    unsigned long long ToInteger()
    {
        return number[0] + number[1] * base;
    }
    friend ostream &operator<<(ostream &os, const BigInteger &dt)
    {
        os << dt.ToString();
        return os;
    }
    BigInteger operator*(const BigInteger &a) const
    {
        int resultSign = sign * a.sign;

        BigInteger result(0);

        for (int i = 0; i < max_size; i++)
        {
            unsigned long long carry = 0;
            for (int j = 0; j < MAX - i; j++)
            {
                unsigned long long product = static_cast<unsigned long long>(number[i]) * static_cast<unsigned long long>(a.number[j]) + result.number[i + j] + carry;
                result.number[i + j] = static_cast<unsigned int>(product & 0xFFFFFFFF);
                carry = product >> 32;
            }
        }

        result.sign = resultSign;

        return result;
    }

    void operator>>=(int shift)
    {
        if (shift == 0)
            return;
        int fullShifts = shift / 32;
        int remainingShift = shift % 32;
        for (int i = 0; i < MAX; i++)
        {
            if (i + fullShifts + 1 < MAX)
            {
                number[i] = (number[i + fullShifts] >> remainingShift) | (number[fullShifts + i + 1] << (32 - remainingShift));
            }
            else
            {
                number[i] = 0;
            }
        }
        RecomputeSize();
    }
};

BigInteger powMod(BigInteger a, BigInteger b, BigInteger m)
{
    BigInteger result = 1;
    a = a % m;
    while (b > 0)
    {
        if (b % 2 == 1)
        {
            result = (result * a) % m;
        }
        b >>= 1;
        a = (a * a) % m;
    }

    return result;
}
// Hàm kiểm tra Miller-Rabin với một số nguyên a và số nguyên n
bool millerRabinTest(BigInteger n, BigInteger a)
{

    BigInteger m = n - 1;
    int k = 0;
    while (m % 2 == 0)
    {
        m >>= 1;
        k++;
    }

    BigInteger x = powMod(a, m, n);

    if (x == 1 || x == n - 1)
    {
        return false;
    }

    for (int i = 0; i < k; i++)
    {
        x = (x * x) % n;

        if (x == n - 1)
        {
            return false;
        }
    }

    return true;
}
bool isPrimeMillerRabin(BigInteger n, int t)
{
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;

    if (n % 2 == 0)
        return false;
    for (int i = 0; i < t; i++)
    {
        BigInteger a = BigInteger::Random(n);
        if (millerRabinTest(n, a))
            return false;
    }

    return true;
}
void runTestCase(string pathInput, string pathOuput)
{
    ifstream inputFile(pathInput);
    ofstream outputFile(pathOuput);
    stringstream ss;

    if (!inputFile.is_open() || !outputFile.is_open())
    {
        cerr << "could not open file input or output" << endl;
        exit(0);
    }

    string line;
    getline(inputFile, line);
    reverse(line.begin(), line.end());
    BigInteger N(line);
    // cout << "N: " << N << endl;
    // clock_t start = clock();
    bool isPrime = isPrimeMillerRabin(N, 5);
    // clock_t end = clock();
    // isPrime ? cout << "N is Prime" << endl : cout << "N is not Prime" << endl;
    isPrime ? outputFile << 1 << endl : outputFile << 0 << endl;
    // double time = (end - start) / (double)CLOCKS_PER_SEC;
    // cout << "Time: " << fixed << time << "s" << endl;
    inputFile.close();
    outputFile.close();
}

string intToStringWithLeadingZeros(int num, int width = 2)
{
    ostringstream oss;
    oss << setw(width) << setfill('0') << num;
    return oss.str();
}
