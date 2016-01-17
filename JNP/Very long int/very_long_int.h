#ifndef VERY_LONG_INT_H
#define VERY_LONG_INT_H

#include <string>
#include <vector>
#include <utility>
#include <bitset>
#include <vector>
#include <functional>

class VeryLongInt;

/* Type used to store bitvectors */
using storage_t = unsigned char;

/* Type used to store pop count of bitvectors and number of binary digits */
using bit_count_t = uint64_t;

/* Type used to perform bit shift operations */
using bitshift_t = uint64_t;

/* Pair returned by division and modulo operators */
using vli_pair_t = std::pair<VeryLongInt, VeryLongInt>;

class VeryLongInt {
   private:
    /* Base for representing numbers internally */
    static const unsigned int REPR_BASE = 256;

    bool isNaN = false;
    std::vector<storage_t> contents;

    /* This constructor cannot be redeclared, we use it only privately (to
     * create Zero() / NaN() objects */
    explicit VeryLongInt(bool);
    /*-- Helper methods --*/
    static bool compare_bit_vectors(const VeryLongInt &, const VeryLongInt &,
                                    std::function<bool(storage_t, storage_t)>);
    static storage_t getLeftShiftMask(storage_t shift);
    static storage_t getRightShiftMask(storage_t shift);
    static unsigned int countLeadingZeros(storage_t x);
    static bool bitwiseEquals(storage_t e1, storage_t e2);
    static bool bitwiseGreater(storage_t e1, storage_t e2);
    static void divideStringByTwo(std::string &str);
    static bool isInteger(const std::string &str);
    static void trimLeadingZeros(std::string &str);
    static void trimLeadingZeros(std::vector<storage_t> &contents);
    vli_pair_t &help_pair();

   public:
    /* An implementation of a nice algorithm to find quotient and
     * remainder*/
    void division(VeryLongInt &, const VeryLongInt &);

    VeryLongInt();

    /* Integer constructors
     * Note: We return NaN for a negative number supplied in the constructor
     */
    VeryLongInt(unsigned long long);
    VeryLongInt(unsigned long number)
        : VeryLongInt((unsigned long long)number){};
    VeryLongInt(unsigned int number)
        : VeryLongInt((unsigned long long)number){};
    VeryLongInt(long long number);
    VeryLongInt(long number) : VeryLongInt((long long)number){};
    VeryLongInt(int number) : VeryLongInt((long long)number){};

    /* Copy constructor */
    VeryLongInt(const VeryLongInt &i) = default;

    /* Move constructor */
    VeryLongInt(VeryLongInt &&i) = default;

    /* Deleted constructors */
    explicit VeryLongInt(unsigned char) = delete;
    explicit VeryLongInt(char) = delete;
    explicit VeryLongInt(signed char) = delete;
    explicit VeryLongInt(char16_t) = delete;
    explicit VeryLongInt(char32_t) = delete;
    explicit VeryLongInt(wchar_t) = delete;

    explicit VeryLongInt(std::string);
    explicit VeryLongInt(const char *);

    /* Assignment operators */

    /* Copy assignment operator */
    VeryLongInt &operator=(const VeryLongInt &) = default;
    /* Move assignment operator */
    VeryLongInt &operator=(VeryLongInt &&) = default;

    friend const VeryLongInt operator+(const VeryLongInt &,
                                       const VeryLongInt &);
    friend const VeryLongInt operator-(const VeryLongInt &,
                                       const VeryLongInt &);
    friend const VeryLongInt operator*(const VeryLongInt &,
                                       const VeryLongInt &);
    friend const VeryLongInt operator/(const VeryLongInt &,
                                       const VeryLongInt &);
    friend const VeryLongInt operator%(const VeryLongInt &,
                                       const VeryLongInt &);

    /* Compound assignment operators */
    VeryLongInt &operator+=(const VeryLongInt &);
    VeryLongInt &operator-=(const VeryLongInt &);
    VeryLongInt &operator*=(const VeryLongInt &);
    VeryLongInt &operator/=(const VeryLongInt &);
    VeryLongInt &operator%=(const VeryLongInt &);

    /* Bitwise operators */
    VeryLongInt &operator>>=(bitshift_t);
    VeryLongInt &operator<<=(bitshift_t);

    friend const VeryLongInt operator<<(const VeryLongInt &, bitshift_t);
    friend const VeryLongInt operator>>(const VeryLongInt &, bitshift_t);

    /* Comparison operators */
    friend bool operator==(const VeryLongInt &, const VeryLongInt &);
    friend bool operator!=(const VeryLongInt &, const VeryLongInt &);
    friend bool operator>(const VeryLongInt &, const VeryLongInt &);
    friend bool operator<(const VeryLongInt &, const VeryLongInt &);
    friend bool operator>=(const VeryLongInt &, const VeryLongInt &);
    friend bool operator<=(const VeryLongInt &, const VeryLongInt &);

    /* ostream operator */
    friend std::ostream &operator<<(std::ostream &, const VeryLongInt &);

    /* Implicit bool conversion
     * Returns false for NaN and zero, true otherwise */
    explicit operator bool() const { return !isNaN && contents[0] != 0; }

    bool isValid() const;

    bit_count_t numberOfBinaryDigits() const;

    /* Returns a VeryLongInt object representing zero */
    friend const VeryLongInt &Zero();

    /* Returns a VeryLongInt object representing NaN */
    friend const VeryLongInt &NaN();
};

const VeryLongInt &Zero();
const VeryLongInt &NaN();

#endif /* VERY_LONG_INT_H */
