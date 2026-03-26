//============================================================================
// Name        	: PriceConverters.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 26, 2022 2:39:50 PM
//============================================================================

#ifndef SRC_AKL_PRICECONVERTERS_HPP_
#define SRC_AKL_PRICECONVERTERS_HPP_

#include <stdint.h>
#include <math.h>

#include <akl/Price.hpp>

/**
 * @brief The PriceConverters class provides utility functions for converting prices.
 */
class PriceConverters {
public:
	static constexpr int64_t int_pow(int base, int exp)
	{
		return (exp == 0) ? 1 : base * int_pow(base, exp - 1);
	}

	inline static int64_t ten_to_power = int_pow(10, PRICE_PRECISION);
	inline static double reciprocal = 1.0 / ten_to_power;
	/**
  * @brief Converts a mantissa value to an integer price.
  * @param mantissa The mantissa value to convert.
  * @return The converted integer price.
  */
	inline static int32_t FromMantissa9(int64_t mantissa)
	{
		int64_t px = mantissa * reciprocal;
		int32_t res = (int32_t)px;
		return res;
	}

	inline static float FromMantissa9Float(int64_t mantissa)
	{
		float px = mantissa * reciprocal;
		return px;
	}

	inline static akl::Price FromMantissa9ToPrice(int64_t mantissa)
	{
		// Assuming mantissa represents the price in a fixed-point format with 9 decimal places
		double unshifted = static_cast<double>(mantissa) * reciprocal;
		return akl::Price::FromUnshifted(unshifted);
	}

	/**
  * @brief Converts an integer price to a mantissa value.
  * @param price The integer price to convert.
  * @return The converted mantissa value.
  */
	inline static int64_t ToMantissa(int32_t price)
	{
     return (price * ten_to_power);
	}

	inline static int64_t ToMantissaFloat(float price)
	{
	 return (int64_t)(price * ten_to_power);
	}

	inline static int64_t ToMantissaFromPrice(const akl::Price& price)
	{
	    return static_cast<int64_t>(price.AsUnshifted() * ten_to_power);
	}

	static constexpr int64_t MAGIC_NUMBER = 2305843009LL;
	static constexpr int32_t SHIFT = 30;

	inline static int32_t FromMantissa(int64_t mantissa)
	{
		//int exponent = DEFAULT_PRICE_EXPONENT;
		//switch (exponent)
		//{
		//case 2:
		//	return FromMantissa2(mantissa);
		//case 4:
		//	return FromMantissa4(mantissa);
		//case 9:
		//	return FromMantissa9(mantissa);
		//default:
		//	return FromMantissa9(mantissa);
		//}
		return FromMantissa9(mantissa);
	}

};

/**
 * @brief CFE-specific price converter
 * CFE BOE3 protocol uses 4 implied decimal places in DOLLARS
 * Example: $13.05 = 130500 in wire format
 * Internal representation: cents × 10^9 (9 decimal places in cents)
 */
class CFEConverter {
public:
	// CFE wire format: 4 implied decimal places (in dollars)
	static constexpr int CFE_DECIMALS = 4;

	// Internal format: 9 decimal places (in cents)
	static constexpr int INTERNAL_DECIMALS = 9;

	// Scale factor: Convert from cents×10^9 to dollars×10^4
	// Formula: cents×10^9 → dollars×10^4 = divide by 10^7 (since 1 dollar = 100 cents)
	static constexpr int64_t SCALE_FACTOR = 10000000LL; // 10^7

	/**
	 * @brief Convert from CFE wire format (4 decimals in dollars) to internal Price (9 decimals in cents)
	 * @param wire_mantissa Price from CFE protocol with 4 decimals (e.g., 130500 = $13.05)
	 * @return Internal Price object with 9 decimal precision (in cents)
	 *
	 * Example: 130500 (wire: $13.05) → 1305000000000 (internal: 1305 cents × 10^9)
	 * Calculation: 130500 × 10^7 = 1305000000000
	 */
	inline static akl::Price FromWire(int64_t wire_mantissa) {
		// CFE sends: 130500 (4 decimals: $13.05)
		// Internal needs: 1305000000000 (9 decimals: 1305 cents)
		// Multiply by 10^7 to convert dollars×10^4 to cents×10^9
		int64_t shifted = wire_mantissa * SCALE_FACTOR;
		return akl::Price::FromShifted(shifted);
	}

	/**
	 * @brief Convert from internal Price (9 decimals in cents) to CFE wire format (4 decimals in dollars)
	 * @param price Internal Price object with 9 decimal precision (in cents)
	 * @return Wire format mantissa for CFE protocol (4 decimals in dollars)
	 *
	 * Example: 1305000000000 (internal: 1305 cents × 10^9) → 130500 (wire: $13.05)
	 * Calculation: 1305000000000 / 10^7 = 130500
	 */
	inline static int64_t ToWire(const akl::Price& price) {
		// Internal has: 1305000000000 (9 decimals: 1305 cents)
		// CFE needs: 130500 (4 decimals: $13.05)
		// Divide by 10^7 to convert cents×10^9 to dollars×10^4
		int64_t shifted = price.AsShifted();
		return shifted / SCALE_FACTOR;
	}
};

/**
 * @brief EQT/Lime-specific price converter
 * Lime Trading API uses ScaledPrice = dollars * 10,000 (4 decimal places)
 * priceScalingFactor = 10,000
 * Internal representation: cents × 10^9 (9 decimal places in cents)
 */
class EQTConverter {
public:
	// Lime wire format: 4 implied decimal places (in dollars)
	static constexpr int LIME_DECIMALS = 4;

	// Internal format: 9 decimal places (in cents)
	static constexpr int INTERNAL_DECIMALS = 9;

	// Scale factor: Convert from cents×10^9 to dollars×10^4
	// Formula: cents×10^9 → dollars×10^4 = divide by 10^7 (since 1 dollar = 100 cents)
	static constexpr int64_t SCALE_FACTOR = 10000000LL; // 10^7

	/**
	 * @brief Convert from Lime wire format (4 decimals in dollars) to internal Price (9 decimals in cents)
	 * @param limeScaledPrice Price from Lime API with 4 decimals (e.g., 130500 = $13.05)
	 * @return Internal Price object with 9 decimal precision (in cents)
	 *
	 * Example: 130500 (wire: $13.05) → 1305000000000 (internal: 1305 cents × 10^9)
	 * Calculation: 130500 × 10^7 = 1305000000000
	 */
	inline static akl::Price FromWire(int64_t limeScaledPrice) {
		// Lime sends: 130500 (4 decimals: $13.05)
		// Internal needs: 1305000000000 (9 decimals: 1305 cents)
		// Multiply by 10^7 to convert dollars×10^4 to cents×10^9
		int64_t shifted = limeScaledPrice * SCALE_FACTOR;
		return akl::Price::FromShifted(shifted);
	}

	/**
	 * @brief Convert from internal Price (9 decimals in cents) to Lime wire format (4 decimals in dollars)
	 * @param price Internal Price object with 9 decimal precision (in cents)
	 * @return Wire format ScaledPrice for Lime API (4 decimals in dollars)
	 *
	 * Example: 1305000000000 (internal: 1305 cents × 10^9) → 130500 (wire: $13.05)
	 * Calculation: 1305000000000 / 10^7 = 130500
	 */
	inline static int64_t ToWire(const akl::Price& price) {
		// Internal has: 1305000000000 (9 decimals: 1305 cents)
		// Lime needs: 130500 (4 decimals: $13.05)
		// Divide by 10^7 to convert cents×10^9 to dollars×10^4
		int64_t shifted = price.AsShifted();
		return shifted / SCALE_FACTOR;
	}
};

/**
 * @brief OSE/OMnet-specific price converter
 * OMnet uses dec_in_premium=4 (premium_i = price × 10,000)
 * Internal representation: Decimal<9> = premium_i × Multiplier(10^9)
 * UI applies pricefactor=0.0001 to display actual JPY price
 *
 * Example: premium_i=1230400 → internal=1230400000000000 → UI displays 123.0400 JPY
 */
class OSEConverter {
public:
	static constexpr int64_t SCALE_FACTOR = akl::Price::Multiplier; // 10^9

	/**
	 * @brief Convert from OMnet wire format to internal Price
	 * @param wire_price OMnet premium_i (e.g., 1230400)
	 * @return Internal Price (1230400000000000)
	 */
	inline static akl::Price FromWire(int64_t wire_price) {
		return akl::Price::FromShifted(wire_price * SCALE_FACTOR);
	}

	/**
	 * @brief Convert from internal Price to OMnet wire format
	 * @param price Internal Price
	 * @return OMnet premium_i value
	 */
	inline static int64_t ToWire(const akl::Price& price) {
		return price.AsShifted() / SCALE_FACTOR;
	}
};

#endif /* SRC_EXCHSUPPORT_CME_PRICECONVERTERS_HPP_ */
