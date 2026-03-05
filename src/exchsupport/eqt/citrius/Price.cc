#include "Price.hh"

#include <cstdint>
#include <iomanip>
#include <limits>

namespace LimeBrokerage
{
    const Price Price::INVALID_PRICE((int32_t)0, PriceExponent(0));

    const std::int64_t Price::powers_of_ten[] = {
        1ull,
        10ull,
        100ull,
        1000ull,
        10000ull,
        100000ull,
        1000000ull,
        10000000ull,
        100000000ull,
        1000000000ull,
        10000000000ull,
        100000000000ull,
        1000000000000ull,
        10000000000000ull,
        100000000000000ull,
        1000000000000000ull,
        10000000000000000ull
    };

    Price::Price()
        : mantissa_(0), exponent_(0), priceVal_ (0)
    {
    }

    Price::Price(int32_t mantissa, int8_t exponent)
        : mantissa_(mantissa), exponent_(exponent), priceVal_ ((std::int64_t)mantissa_ * powers_of_ten[NORMALIZED_EXPONENT + exponent_])
    {
    }

    Price::Price(int64_t mantissa, PriceExponent exponent)
        : mantissa_(mantissa)
    {
        std::int64_t max = std::numeric_limits<std::int32_t>::max();
        std::int64_t min = std::numeric_limits<std::int32_t>::min();

        while ((mantissa > max) || (mantissa < min)) 
        {
            mantissa /= 10;
            exponent++;
        }
        mantissa_ = mantissa;
        priceVal_ = mantissa * powers_of_ten[NORMALIZED_EXPONENT + exponent.get()];
        exponent_ = exponent.get();
    }

    Price Price::createPriceFromFractionanlRep(int64_t numerator, int32_t denominator)
    {
        int64_t intPart = numerator / denominator;
        int32_t decPart = 0;
        int32_t decPartLen = 0;

        int32_t remainder = numerator - intPart * denominator;
        int i;
        for (i = 0; i < 6; i++) {
            remainder *= 10;
            int32_t q = remainder / denominator;
            remainder -= q * denominator;
            decPart = decPart * 10 + q;
            decPartLen++;
        }
        return Price(intPart * powers_of_ten[decPartLen] + decPart, PriceExponent(-decPartLen));
    }

    Price::~Price()
    {
    }

    Price::Price(const Price& price)
    {
        mantissa_ = price.mantissa_;
        exponent_ = price.exponent_;
        priceVal_ = price.priceVal_;
    }

    const Price& Price::operator=(const Price& price)
    {
        if (this != &price) {
            mantissa_ = price.mantissa_;
            exponent_ = price.exponent_;
            priceVal_ = price.priceVal_;
        }
        return *this;
    }

    double Price::toDouble() const
    {
        int32_t maxexponent_ = sizeof(powers_of_ten)/sizeof(double);
        if (exponent_ < -1 * maxexponent_ || exponent_ > maxexponent_) {
            //Invalid price conversion
            return -1;
        }
        if (exponent_ < 0) {
            return (double)mantissa_ / powers_of_ten[-1 * exponent_];
        }
        return (double)mantissa_ * powers_of_ten[exponent_];
    }

    int64_t Price::toInt() const
    {
        static int32_t max_exponent = sizeof(powers_of_ten)/sizeof(int64_t);
        int exponent = NORMALIZED_EXPONENT + exponent_;
        if (exponent < -1 * max_exponent || exponent > max_exponent) {
            //Invalid price conversion
            return -1;
        }
        return (int64_t)(mantissa_ * powers_of_ten[exponent]);
    }

    double Price::toDouble(int32_t mantissa, int8_t exponent)
    {
        static int32_t max_exponent = sizeof(powers_of_ten)/sizeof(int64_t);
        if (exponent < -1 * max_exponent || exponent > max_exponent) {
            //Fixme: error handling Invalid price conversion
            return -1;
        }
        if (exponent < 0) {
            return (double)mantissa / powers_of_ten[-1 * exponent];
        }
        return (double)mantissa * powers_of_ten[exponent];
    }

    int64_t Price::toInt(int32_t mantissa, int8_t exp)
    {
        static int32_t max_exponent = sizeof(powers_of_ten)/sizeof(int64_t);
        int exponent = NORMALIZED_EXPONENT + exp;
        if (exponent < -1 * max_exponent || exponent > max_exponent) {
            //Invalid price conversion
            return -1;
        }
        return ((int64_t)mantissa * powers_of_ten[exponent]);
    }

    Price::operator double () const
    {
        return toDouble();
    }

    Price::operator int64_t () const
    {
        return toInt();
    }

    bool operator==(const Price& p1, const Price& p2)
    {
        return (p1.priceVal_ == p2.priceVal_);
    }

    bool operator!=(const Price& p1, const Price& p2)
    {
        return (p1.priceVal_ != p2.priceVal_);
    }

    bool operator>=(const Price& p1, const Price& p2)
    {
        return (p1.priceVal_ >= p2.priceVal_);
    }

    bool operator>(const Price& p1, const Price& p2)
    {
        return (p1.priceVal_ > p2.priceVal_);
    }

    bool operator<=(const Price& p1, const Price& p2)
    {
        return (p1.priceVal_ <= p2.priceVal_);
    }

    bool operator<(const Price& p1, const Price& p2)
    {
        return (p1.priceVal_ < p2.priceVal_);
    }

} /* namespace LimeBrokerage */

