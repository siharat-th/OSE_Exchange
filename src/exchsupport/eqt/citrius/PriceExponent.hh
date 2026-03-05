#pragma once

#include <cstdint>


namespace LimeBrokerage
{

    class PriceExponent
    {
    public:

        using value_type = std::int32_t;

        PriceExponent():value_(0){}

        explicit PriceExponent
        (
            value_type value
        ):
            value_(value)
        {
        }

        PriceExponent(PriceExponent const & other):value_(other.value_){}

        PriceExponent operator -()
        {
            return PriceExponent(-value_);
        }
        
        PriceExponent & operator ++ ()
        {
            ++value_;
            return *this;
        }

        PriceExponent operator ++ (int)
        {
            PriceExponent result(*this);
            ++value_;
            return result;
        }

        PriceExponent & operator -- ()
        {
            --value_;
            return *this;
        }

        PriceExponent operator -- (int)
        {
            PriceExponent result(*this);
            --value_;
            return result;
        }

        value_type get() const
        {
            return value_;
        }
        
    private:

        value_type  value_;
    };

}