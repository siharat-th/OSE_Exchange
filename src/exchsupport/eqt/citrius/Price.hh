
#ifndef _LIME_PRICE_H_
#define _LIME_PRICE_H_

#include <iostream>
#include <iomanip>
#include "limetypes.hh"

#include "./PriceExponent.hh"

namespace LimeBrokerage
{
    /**
     * A container for price information.
     */
    class Price {
    public:
        static const Price INVALID_PRICE;
        static const int NORMALIZED_EXPONENT = 7;
        static const int64_t powers_of_ten[];

        /**
         * Initialize price to zero.
         */
        Price();

        /**
         * Initialize price using mantissa and exponent values.
         */
        Price(int32_t mantissa, int8_t exponent);

        //Round mantissa to fit 32 bit integer, some precision might be lost.
        Price(int64_t mantissa, PriceExponent exponent);

        //Create price by converting fractional representation into mantissa/exponent representation.
        static Price createPriceFromFractionanlRep(int64_t numerator, int32_t denominator);

        virtual ~Price();

        Price(const Price&);

        const Price & operator=(const Price&);

        /**
         * Get the mantissa value for this price.
         */
        uint32_t getMantissa() const { return mantissa_; }

        /**
         * Get the exponent value for this price.
         */
        int8_t getExponent() const { return exponent_; }

        static double toDouble(int32_t mantissa, int8_t exponent); 

        static int64_t toInt(int32_t mantissa, int8_t exponent);

        /**
         * Get the price as a double.
         *
         * If conversion fails, price value will be -1.0.
         */
        double toDouble() const;
 
        /**
         * Get the price as a uint32_t.
         *
         * If conversion fails, price value will be (uint64_t)-1.
         */
        int64_t toInt() const;

        operator double() const;

        operator int64_t() const;

        /**
         * Convert price to double for insertion into output stream.
         */
        template <typename S> friend S& operator<<(S& s, const Price& b);
        friend bool operator==(const Price& p1, const Price& p2);
        friend bool operator!=(const Price&, const Price&);
        friend bool operator>=(const Price&, const Price&);
        friend bool operator>(const Price&, const Price&);
        friend bool operator<=(const Price&, const Price&);
        friend bool operator<(const Price&, const Price&);

    private:
        int32_t mantissa_;
        int8_t exponent_;
        int64_t priceVal_;
    };  

    template <typename S> S& operator<<(S& s, const Price& b)
    {
        double price = (double) b;
        s << std::left << std::setprecision(6) << std::setw(8) << std::showpoint << (double)b;
        return s;
    }

} /* namespace LimeBrokerage */

#endif /* _LIME_PRICE_H_ */

