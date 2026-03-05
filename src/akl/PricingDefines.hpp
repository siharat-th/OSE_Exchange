#ifndef SRC_AKL_PRICEDEFINES_HPP
#define SRC_AKL_PRICEDEFINES_HPP_

#pragma once

#define CME_PRICE_PRECISION 9
#define CFE_PRICE_PRECISION 9  // Changed from 2 to 9 for unified precision

// IMPORTANT: Internal price precision is now always 9 decimal places
// This allows price comparison across exchanges (CFE and CME)
// Wire protocol conversion is handled by CFEConverter and CMEConverter
#define PRICE_PRECISION 9  // Unified precision for all exchanges

// Legacy compiler flag support (deprecated - always uses 9 now)
#ifndef PRICE_PRECISION
#define PRICE_PRECISION 9
#endif

#endif