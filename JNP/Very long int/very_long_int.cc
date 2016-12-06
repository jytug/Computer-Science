#include "very_long_int.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <functional>

/*
 * Assumptions:
 *   1. We accept strings with trailing zeros in constructors.
 *   2. Numbers are stored in bit vectors (unsigned char), MSB to LSB
 *   3. Bitwise operations applied to NaNs have no effect (no compile time
 *   errors, etc.)
 *   4. We assume that VeryLongInt % 0 should return NaN
 */

/*-- Helper methods --*/

/*-- Methods that operate on bit sequences --*/

/* Compares two bit vectors (of type storage_t) using comparator function
 * supplied */
bool VeryLongInt::compare_bit_vectors(
    const VeryLongInt &val1, const VeryLongInt &val2,
    std::function<bool(storage_t, storage_t)> comparator) {
    return std::lexicographical_compare(
        val1.contents.begin(), val1.contents.end(), val2.contents.begin(),
        val2.contents.end(), comparator);
}

/* Returns shift mask for a given first 'shift' bits set, i.e.
 * 3 -> 11100000 */
storage_t VeryLongInt::getLeftShiftMask(storage_t shift) {
    static storage_t masks[9] = {0x0,  0x80, 0xc0, 0xe0, 0xf0,
                                 0xf8, 0xfc, 0xfe, 0xff};
    return masks[shift];
}

/* Returns shift mask for a given last 'shift' bits set, i.e.
 * 3 -> 00000111 */
storage_t VeryLongInt::getRightShiftMask(storage_t shift) {
    return ~getLeftShiftMask(8 - shift);
}

/* Returns number of (binary) leading zeros */
unsigned int VeryLongInt::countLeadingZeros(storage_t x) {
    unsigned int y;
    unsigned int n;

    n = 8;
    y = x >> 4;
    if (y != 0) {
        n = n - 4;
        x = y;
    }
    y = x >> 2;
    if (y != 0) {
        n = n - 2;
        x = y;
    }
    y = x >> 1;
    if (y != 0) return n - 2;
    return n - x;
}

/* Comparators */

bool VeryLongInt::bitwiseEquals(storage_t e1, storage_t e2) { return e1 == e2; }

bool VeryLongInt::bitwiseGreater(storage_t e1, storage_t e2) { return e1 > e2; }

/* String helper functions */

/* Divides a string representing unsigned integer by 2 */
void VeryLongInt::divideStringByTwo(std::string &str) {
    unsigned long next_additive = 0;
    for (size_t i = 0; i < str.size(); i++) {
        unsigned long additive = next_additive;
        unsigned long digit = std::stoul(str.substr(i, 1));
        next_additive = (digit % 2 == 0 ? 0 : 5);
        digit /= 2;
        digit += additive;
        str.erase(i, 1);
        str.insert(i, std::to_string(digit));
    }
    if (str != "0" && str[0] == '0') str.erase(0, 1);
}

/* Checks if string contains a valid number */
bool VeryLongInt::isInteger(const std::string &str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

/* Removes leading zeros from a string */
void VeryLongInt::trimLeadingZeros(std::string &str) {
    size_t first_nonzero = str.find_first_not_of('0');
    if (first_nonzero == std::string::npos) first_nonzero = str.size() - 1;
    str.erase(0, first_nonzero);
}

/* Removes leading binary segments from the contents of a VeryLongInt number */
void VeryLongInt::trimLeadingZeros(std::vector<storage_t> &contents) {
    if (contents.size() != 0) {
        auto first_nonzero = std::find_if(contents.begin(), contents.end(),
                                          [](storage_t i) { return i != 0; });
        if (first_nonzero == contents.end()) first_nonzero--;

        contents.erase(contents.begin(), first_nonzero);
    }
}

/* Help_pair allows us to not copy big objects in recursive procedure
 * VeryLongInt::division - its values are set to 0 every time after the
 * operators /=, %=, /, % are called */
vli_pair_t &VeryLongInt::help_pair() {
    static VeryLongInt a, b;
    static vli_pair_t p(a, b);
    return p;
}

/* VeryLongInt::division is an implementation of a recursive algorithm for
 * finding both the quotient and remainder from integer division */
void VeryLongInt::division(VeryLongInt &x, const VeryLongInt &y) {
    /* We asume both *this and x are valid numbers - this will be checked in
     * the
     * operators /=, %= */
    if (x != Zero()) {
        bool x_odd = ((x.contents[x.contents.size() - 1] % 2) == 1);
        x >>= 1;
        division(x, y);
        x <<= 1;
        help_pair().first *= 2;
        help_pair().second *= 2;
        if (x_odd) {
            x += 1;
            help_pair().second += 1;
        }
        if (help_pair().second >= y) {
            help_pair().second -= y;
            help_pair().first += 1;
        }
    } else {
        static VeryLongInt a, b;
        help_pair().first = a;
        help_pair().second = b;
    }
}

/* Constructors */

/* Private bool constructor */
VeryLongInt::VeryLongInt(bool isNaN)
    : isNaN(isNaN), contents(std::vector<storage_t>(1, 0)) {}

/* Default constructor */
VeryLongInt::VeryLongInt()
    : isNaN(false), contents(std::vector<storage_t>(1, 0)) {}

VeryLongInt::VeryLongInt(unsigned long long i) {
    if (i == 0) {
        (*this).contents = {0};
        return;
    }
    while (i != 0) {
        contents.push_back(i % REPR_BASE);
        i /= REPR_BASE;
    }
    std::reverse(contents.begin(), contents.end());
}

VeryLongInt::VeryLongInt(long long i) {
    if (i < 0)
        (*this).isNaN = true;
    else
        (*this) = VeryLongInt((unsigned long long)i);
}

VeryLongInt::VeryLongInt(const char *str) {
    /* Check the pointer */
    if (str != nullptr)
        *this = VeryLongInt(std::string(str));
    else
        (*this).isNaN = true;
}

/* std::string constructor
 * It takes a copy of the string, so that we could apply
 * divideStringByTwo() function on it */
VeryLongInt::VeryLongInt(std::string str) {
    contents = std::vector<storage_t>();
    /* Currently added segment */
    std::bitset<8> current;
    if (isInteger(str)) {
        trimLeadingZeros(str);
        int i = 0; /* Position of the current binary digit */
        while (str != "0") {
            std::string digit = str.substr(str.size() - 1, 1);
            storage_t binary_digit = std::stoul(digit) % 2;

            /* Set bit if neccessary */
            if (binary_digit) current.set(i % 8);

            i++;

            /* Add every unsigned char if ready to be added */
            if (i % 8 == 0) {
                contents.push_back(
                    static_cast<unsigned char>(current.to_ulong()));
                current = std::bitset<8>();
            }

            divideStringByTwo(str);
        }

        /* There may be still some binary digits that should be added
         * or we may have received "0" in the constructor */
        if (i % 8 != 0 || !contents.size())
            contents.push_back(static_cast<unsigned char>(current.to_ulong()));

        std::reverse(contents.begin(), contents.end());
    } else {
        isNaN = true;
    }
}

/* Returns a VeryLongInt object representing zero */
const VeryLongInt &Zero() {
    static VeryLongInt val(false);
    return val;
}

/* Returns a VeryLongInt object representing NaN */
const VeryLongInt &NaN() {
    const static VeryLongInt val(true);
    return val;
}

/* Returns false for NaNs, true otherwise */
bool VeryLongInt::isValid() const { return !isNaN; }

bit_count_t VeryLongInt::numberOfBinaryDigits() const {
    /* Implementation defined: should return 1 for zero and 0 for NaNs */
    if ((*this).isNaN)
        return 0;
    else if ((*this) == Zero())
        return 1;
    else if (contents.size() == 1 && contents[0] == 0)
        return !isNaN;
    /* In our implementation, there may be some unused bits on the left of
     * the
     * first bit vector, which should be subtracted from the number of
     * binary
     * digits */
    else
        return 8 * contents.size() - countLeadingZeros(contents[0]);
}

/* Comparison operators */

bool operator==(const VeryLongInt &val1, const VeryLongInt &val2) {
    /*  NaNs */
    if (!val1.isValid() || !val2.isValid()) return false;
    /* Different bit pattern lengths */
    if (val1.contents.size() != val2.contents.size()) return false;
    return val1.contents == val2.contents;
}

bool operator!=(const VeryLongInt &val1, const VeryLongInt &val2) {
    if (!val1.isValid() || !val2.isValid()) return false;
    return !(val1 == val2);
}

bool operator>(const VeryLongInt &val1, const VeryLongInt &val2) {
    if (!val1.isValid() || !val2.isValid()) return false;
    if (val1.contents.size() == val2.contents.size()) {
        return VeryLongInt::compare_bit_vectors(val1, val2,
                                                VeryLongInt::bitwiseGreater);
    }
    return val1.contents.size() > val2.contents.size();
}

bool operator<(const VeryLongInt &val1, const VeryLongInt &val2) {
    if (!val1.isValid() || !val2.isValid()) return false;
    if (val1.contents.size() == val2.contents.size()) {
        /* Swap the values to be compared */
        return VeryLongInt::compare_bit_vectors(val2, val1,
                                                VeryLongInt::bitwiseGreater);
    }

    return val1.contents.size() < val2.contents.size();
}

bool operator>=(const VeryLongInt &val1, const VeryLongInt &val2) {
    if (!val1.isValid() || !val2.isValid()) return false;
    if (val1.contents.size() == val2.contents.size()) {
        bool compare = VeryLongInt::compare_bit_vectors(
            val1, val2, VeryLongInt::bitwiseGreater);
        return compare || val1 == val2;
    }
    return val1.contents.size() > val2.contents.size();
}

bool operator<=(const VeryLongInt &val1, const VeryLongInt &val2) {
    if (!val1.isValid() || !val2.isValid()) return false;
    if (val1.contents.size() == val2.contents.size()) {
        /* Swap the values to be compared */
        bool compare = VeryLongInt::compare_bit_vectors(
            val2, val1, VeryLongInt::bitwiseGreater);
        return compare || val1 == val2;
    }
    return val1.contents.size() < val2.contents.size();
}

/* Compound assignment operators */

VeryLongInt &VeryLongInt::operator+=(const VeryLongInt &val) {
    if (!((*this).isValid() && val.isValid())) {
        this->isNaN = true;
        return (*this);
    }

    if (val > (*this)) {
        size_t prev_size = (*this).contents.size();
        (*this).contents.resize(val.contents.size(), 0);
        std::rotate((*this).contents.begin(),
                    (*this).contents.begin() + prev_size,
                    (*this).contents.end());
    }

    /* At this point, res_val.contents has size >= val2.contents.size() and
     * there might be leading zeros */
    auto it1 = (*this).contents.rbegin();
    auto it2 = val.contents.rbegin();
    /* Reverse iterators - the last digit is least significant */
    uint32_t carry = 0;

    for (; it1 != (*this).contents.rend(); it1++) {
        uint32_t digit1 = *it1, digit2;
        if (it2 != val.contents.rend()) {
            /* (*this).contents might be longer than val; to avoid
             * copy-pasting,
             * we fictionally enlarge val with additional leading
             * zeros */
            digit2 = *it2;
            it2++;
        } else {
            digit2 = 0;
        }
        uint32_t new_digit = digit1 + digit2 + carry;
        carry = new_digit / REPR_BASE;
        new_digit %= REPR_BASE;
        *it1 = new_digit;
    }
    if (carry != 0) {
        (*this).contents.insert((*this).contents.begin(), 1);
    }
    VeryLongInt::trimLeadingZeros((*this).contents);
    return *this;
}

VeryLongInt &VeryLongInt::operator-=(const VeryLongInt &val2) {
    if (!((*this).isValid() && val2.isValid()) || val2 > (*this)) {
        (*this).isNaN = true;
        return (*this);
    }
    /* At this point we are assured that (*this) >= val2, hence
     * (*this).contents.size() >= val2.contents.size() */
    auto it1 = (*this).contents.rbegin();
    auto it2 = val2.contents.rbegin();
    uint32_t carry = 0;
    for (; it1 != (*this).contents.rend(); it1++) {
        uint32_t digit1 = *it1, digit2;
        if (it2 != val2.contents.rend()) {
            digit2 = *it2;
            it2++;
        } else {
            digit2 = 0;
        }
        uint32_t new_digit;
        if (digit2 + carry > digit1) {
            new_digit = REPR_BASE - (digit2 + carry - digit1);
            carry = 1;
        } else {
            new_digit = digit1 - (digit2 + carry);
            carry = 0;
        }
        *it1 = new_digit;
    }
    trimLeadingZeros((*this).contents);
    return (*this);
}

VeryLongInt &VeryLongInt::operator*=(const VeryLongInt &val2) {
    if (!((*this).isValid() && val2.isValid())) {
        (*this).isNaN = true;
        return (*this);
    }

    size_t new_size = (*this).contents.size() + val2.contents.size();

    /* Observe that if a positive integer n has length k (in base b), then
     * b^{k-1} <= n <= b^k, hence the length of a product of two positive
     * integers of length k1, k2 has length between k1 + k2 - 1 and k1 + k2
     */
    std::vector<storage_t> product = std::vector<storage_t>(new_size, 0);
    uint32_t carry = 0;
    /* It's more convenient using int iterators instead of
     * std::vector<T>::iterator, since we can add them */
    for (int i = (*this).contents.size() - 1; i >= 0; i--) {
        for (int j = val2.contents.size() - 1; j >= 0; j--) {
            uint32_t digit1 = (*this).contents[i], digit2 = val2.contents[j],
                     new_digit = digit1 * digit2 + carry +
                                 product[new_size - (i + j + 2)];
            product[new_size - (i + j + 2)] = new_digit % REPR_BASE;
            carry = new_digit / REPR_BASE;
        }
        product[new_size - i - 1] = carry;
        carry = 0;
    }

    std::reverse(product.begin(), product.end());
    trimLeadingZeros(product);
    (*this).contents = product;

    return (*this);
}

/* Uses VeryLongInt::division() method to return div operator result */
VeryLongInt &VeryLongInt::operator/=(const VeryLongInt &val2) {
    if (!((*this).isValid() && val2.isValid()) || val2 == Zero()) {
        this->isNaN = true;
        this->contents.clear();
        return (*this);
    }
    VeryLongInt::division((*this), val2);
    (*this) = help_pair().first;
    help_pair().first = 0;
    help_pair().second = 0;
    return (*this);
}

/* Uses VeryLongInt::division() method to return mod operator result */
VeryLongInt &VeryLongInt::operator%=(const VeryLongInt &val2) {
    if (!(*this).isValid() || !val2.isValid() || val2 == Zero()) {
        this->isNaN = true;
        this->contents.clear();
        return (*this);
    }

    VeryLongInt::division((*this), val2);
    (*this) = help_pair().second;
    help_pair().first = 0;
    help_pair().second = 0;
    return (*this);
}

/* Binary operators */

const VeryLongInt operator+(const VeryLongInt &val1, const VeryLongInt &val2) {
    VeryLongInt result(val1);
    return result += val2;
}

const VeryLongInt operator-(const VeryLongInt &val1, const VeryLongInt &val2) {
    VeryLongInt ret_val = val1;
    return ret_val -= val2;
}

const VeryLongInt operator*(const VeryLongInt &val1, const VeryLongInt &val2) {
    VeryLongInt ret_val = val1;
    return ret_val *= val2;
}

const VeryLongInt operator/(const VeryLongInt &val1, const VeryLongInt &val2) {
    VeryLongInt ret_val = val1;
    return ret_val /= val2;
}

const VeryLongInt operator%(const VeryLongInt &val1, const VeryLongInt &val2) {
    VeryLongInt ret_val = val1;
    ret_val %= val2;
    return ret_val;
}

/* Bitwise operators */

VeryLongInt &VeryLongInt::operator>>=(bitshift_t shift) {
    VeryLongInt &number = *this;
    if (number.isValid()) {
        auto &segments = number.contents;

        if (number.contents.size() == 1) {
            number.contents[0] >>= shift;
        } else if (number.numberOfBinaryDigits() < shift) {
            number.contents = {0};
        } else {
            while (shift) {
                storage_t previous = *(segments.begin());
                uint8_t current_shift = (shift < 8) ? shift : 8;
                storage_t right_mask = getRightShiftMask(current_shift);

                for (auto rit = segments.begin() + 1; rit != segments.end();
                     ++rit) {
                    storage_t &current = (*rit);
                    storage_t tmp = (previous & right_mask)
                                    << (8 - current_shift);
                    previous = current;

                    current = (current >> current_shift) | tmp;
                }
                /* Shift the MSB segment */
                *(segments.begin()) >>= current_shift;

                shift -= current_shift;
            }

            /* Removed unused segments */
            trimLeadingZeros(segments);
        }
    }

    return number;
}

VeryLongInt &VeryLongInt::operator<<=(bitshift_t shift) {
    if (shift != 0 && (*this).isValid() && (*this) != Zero()) {
        auto &segments = (*this).contents;

        /* Find trailing zeros in first byte (MSB segment) */
        unsigned int leading_zeros = countLeadingZeros((*this).contents[0]);
        /* Make space for new bits if needed */
        if (leading_zeros < shift) {
            unsigned long long segments_needed =
                (shift - leading_zeros) / 8U +
                ((shift - leading_zeros) % 8U != 0);
            unsigned char zero = 0;
            segments.insert(segments.begin(), segments_needed, zero);
        }

        while (shift) {
            storage_t previous = *(segments.rbegin());
            unsigned int current_shift = (shift < 8) ? shift : 8;
            storage_t left_mask = getLeftShiftMask(current_shift);

            for (auto rit = segments.rbegin() + 1; rit != segments.rend();
                 ++rit) {
                storage_t &current = (*rit);
                storage_t tmp = (previous & left_mask) >> (8 - current_shift);
                previous = current;

                current = (current << current_shift) | tmp;
            }
            /* Shift the LSB segment */
            *(segments.rbegin()) <<= current_shift;

            shift -= current_shift;
        }
    }
    return *this;
}

const VeryLongInt operator<<(const VeryLongInt &val1, bitshift_t shift) {
    VeryLongInt ret_val = val1;
    ret_val <<= shift;
    return ret_val;
}

const VeryLongInt operator>>(const VeryLongInt &val1, bitshift_t shift) {
    VeryLongInt ret_val = val1;
    ret_val >>= shift;
    return ret_val;
}

/* Stream operators */
std::ostream &operator<<(std::ostream &os, const VeryLongInt &input_number) {
    /* Convert to base 10 */
    static const VeryLongInt BASE = VeryLongInt(10ULL);

    if (input_number.isNaN) {
        os << "NaN";
    } else if (input_number == Zero()) {
        os << "0";
    } else {
        /* We make a copy to divide the number until it equals 0 */
        VeryLongInt number = input_number;
        VeryLongInt mod;

        std::vector<char> digits;

        while (number) {
            mod = number % BASE;
            digits.push_back(mod.contents[0] + '0');
            number /= BASE;
        }

        for (auto rit = digits.rbegin(); rit != digits.rend(); rit++)
            os << *rit;
    }
    return os;
}
