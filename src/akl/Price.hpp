/*
 * Price.hpp
 *
 *  Created on: Jan 14, 2025
 *      Author: rocky
 */

#ifndef SRC_AKL_PRICE_HPP_
#define SRC_AKL_PRICE_HPP_

#pragma once

#include <akl/Decimal.hpp>
#include <akl/PricingDefines.hpp>

namespace akl
{
// Price is now defined using the compile-time precision
using Price = Decimal<PRICE_PRECISION>;

// If you need to work with multiple exchanges in the same binary,
// you can define exchange-specific price types
// using CMEPrice = Decimal<9>;
// using CFEPrice = Decimal<0>;

// // Optional: A templated price type that can be used for generic code
// template<int EXCHANGE_TYPE>
// struct ExchangePrice;

// // Specializations for different exchanges
// template<>
// struct ExchangePrice<0> {  // 0 could represent CME
//     using Type = Decimal<9>;
// };

// template<>
// struct ExchangePrice<1> {  // 1 could represent CFE
//     using Type = Decimal<0>;
// };

// Usage: ExchangePrice<EXCHANGE_TYPE>::Type myPrice;
} // namespace akl


#endif /* SRC_AKL_PRICE_HPP_ */
