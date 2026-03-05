/*
 * Decimal.hpp
 *
 *  Created on: Jan 14, 2025
 *      Author: rocky
 */

 #ifndef SRC_AKL_DECIMAL_HPP_
 #define SRC_AKL_DECIMAL_HPP_
 
 #pragma once
 
 #include <cstdint>
 #include <cmath>
 #include <limits>
 #include <ostream>
 
 namespace akl
 {
	 template<int PLACES>
	 class Decimal;
 }
 
 // Decimal operators for Decimal-to-Decimal operations
 template<int PLACES>
 inline akl::Decimal<PLACES> operator+(const akl::Decimal<PLACES>& lhs, const akl::Decimal<PLACES>& rhs) noexcept;
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator-(const akl::Decimal<PLACES>& lhs, const akl::Decimal<PLACES>& rhs) noexcept;
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator*(const akl::Decimal<PLACES>& lhs, const int rhs) noexcept;
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator*(const int lhs, const akl::Decimal<PLACES>& rhs) noexcept;
 
 template<int PLACES>
 inline int operator/(const akl::Decimal<PLACES>& lhs, const akl::Decimal<PLACES>& rhs) noexcept;
 
 template<int PLACES>
 inline std::ostream& operator<<(std::ostream& os, const akl::Decimal<PLACES>& q) noexcept;
 
 // Overloads for operations with double operands
 template<int PLACES>
 inline akl::Decimal<PLACES> operator+(const akl::Decimal<PLACES>& lhs, double rhs) noexcept {
	 return lhs + akl::Decimal<PLACES>::FromUnshifted(rhs);
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator+(double lhs, const akl::Decimal<PLACES>& rhs) noexcept {
	 return akl::Decimal<PLACES>::FromUnshifted(lhs) + rhs;
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator-(const akl::Decimal<PLACES>& lhs, double rhs) noexcept {
	 return lhs - akl::Decimal<PLACES>::FromUnshifted(rhs);
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator-(double lhs, const akl::Decimal<PLACES>& rhs) noexcept {
	 return akl::Decimal<PLACES>::FromUnshifted(lhs) - rhs;
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator*(const akl::Decimal<PLACES>& lhs, double rhs) noexcept {
	 return akl::Decimal<PLACES>::FromUnshifted(lhs.AsUnshifted() * rhs);
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator*(double lhs, const akl::Decimal<PLACES>& rhs) noexcept {
	 return akl::Decimal<PLACES>::FromUnshifted(lhs * rhs.AsUnshifted());
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator/(const akl::Decimal<PLACES>& lhs, double rhs) noexcept {
	 return akl::Decimal<PLACES>::FromUnshifted(lhs.AsUnshifted() / rhs);
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator/(double lhs, const akl::Decimal<PLACES>& rhs) noexcept {
	 return akl::Decimal<PLACES>::FromUnshifted(lhs / rhs.AsUnshifted());
 }
 
 // Overloads for operations with int operands
 template<int PLACES>
 inline akl::Decimal<PLACES> operator+(const akl::Decimal<PLACES>& lhs, int rhs) noexcept {
	 return akl::Decimal<PLACES>::FromShifted(lhs.AsShifted() + rhs * static_cast<int64_t>(akl::Decimal<PLACES>::Multiplier));
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator+(int lhs, const akl::Decimal<PLACES>& rhs) noexcept {
	 return akl::Decimal<PLACES>::FromShifted(static_cast<int64_t>(lhs) * akl::Decimal<PLACES>::Multiplier + rhs.AsShifted());
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator-(const akl::Decimal<PLACES>& lhs, int rhs) noexcept {
	 return akl::Decimal<PLACES>::FromShifted(lhs.AsShifted() - rhs * static_cast<int64_t>(akl::Decimal<PLACES>::Multiplier));
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator-(int lhs, const akl::Decimal<PLACES>& rhs) noexcept {
	 return akl::Decimal<PLACES>::FromShifted(static_cast<int64_t>(lhs) * akl::Decimal<PLACES>::Multiplier - rhs.AsShifted());
 }
 
 // Overloads for operations with unsigned int operands (e.g. uint32_t)
 template<int PLACES>
 inline akl::Decimal<PLACES> operator*(const akl::Decimal<PLACES>& lhs, unsigned int rhs) noexcept {
	 return akl::Decimal<PLACES>::FromShifted(lhs.AsShifted() * static_cast<int64_t>(rhs));
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator*(unsigned int lhs, const akl::Decimal<PLACES>& rhs) noexcept {
	 return akl::Decimal<PLACES>::FromShifted(static_cast<int64_t>(lhs) * rhs.AsShifted());
 }
 
 namespace akl
 {
 
 template<int PLACES>
 class Decimal
 {
 private:
	 using ThisType = Decimal<PLACES>;
	 using ShiftedType = int64_t;
	 using UnshiftedType = double;
 
	 template<int N>
	 static constexpr double calculateMultiplier() {
		 static_assert(N >= 0);
		 if constexpr (N > 0)
			 return 10 * calculateMultiplier<N - 1>();
		 else // N == 0
			 return 1;
	 }
 
 public:
	 inline constexpr Decimal() noexcept : shifted(0) { }
 
	 static constexpr double Multiplier = calculateMultiplier<PLACES>();
 
	 static inline constexpr ThisType FromShifted(ShiftedType shifted) { return ThisType { shifted }; }
	 static inline constexpr ThisType FromUnshifted(UnshiftedType unshifted) {
		 return ThisType { static_cast<ShiftedType>(unshifted * Multiplier + (unshifted >= 0.0 ? 0.5 : -0.5)) };
	 }
	 static inline constexpr ThisType Min() { return FromShifted(std::numeric_limits<ShiftedType>::min()); }
	 static inline constexpr ThisType Max() { return FromShifted(std::numeric_limits<ShiftedType>::max()); }
	 
	 inline ShiftedType AsShifted() const noexcept { return shifted; }
	 inline UnshiftedType AsUnshifted() const noexcept { return static_cast<double>(shifted) / Multiplier; }
 
	 inline bool operator==(const ThisType& rhs) const noexcept { return shifted == rhs.shifted; }
	 inline bool operator!=(const ThisType& rhs) const noexcept { return shifted != rhs.shifted; }
	 inline bool operator<(const ThisType& rhs) const noexcept { return shifted < rhs.shifted; }
	 inline bool operator<=(const ThisType& rhs) const noexcept { return shifted <= rhs.shifted; }
	 inline bool operator>(const ThisType& rhs) const noexcept { return shifted > rhs.shifted; }
	 inline bool operator>=(const ThisType& rhs) const noexcept { return shifted >= rhs.shifted; }
 
	 inline ThisType& operator+=(const ThisType& rhs) noexcept { shifted += rhs.shifted; return *this; }
	 inline ThisType& operator-=(const ThisType& rhs) noexcept { shifted -= rhs.shifted; return *this; }
 
	 inline ThisType operator-() const noexcept { return Decimal(-shifted); }
 
	 // Convenience: operator += for double
	 inline ThisType& operator+=(const double rhs) noexcept {
		 shifted += static_cast<ShiftedType>(rhs * Multiplier + (rhs >= 0.0 ? 0.5 : -0.5));
		 return *this;
	 }
 
	 // Conversion operator to double
	 inline operator double() const noexcept {
		 return AsUnshifted();
	 }
 
	 // Assignment from int
	 Decimal& operator=(int value) noexcept {
		 shifted = static_cast<ShiftedType>(value) * Multiplier;
		 return *this;
	 }
 
	 // Assignment from double
	 Decimal& operator=(double value) noexcept {
		 shifted = static_cast<ShiftedType>(value * Multiplier + (value >= 0.0 ? 0.5 : -0.5));
		 return *this;
	 }
 
 private:
	 explicit inline constexpr Decimal(const ShiftedType shifted) noexcept : shifted(shifted) { }
	 ShiftedType shifted;
 };
 
 } // namespace akl
 
 // Definitions for non-member operators using the Decimal type
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator+(const akl::Decimal<PLACES>& lhs, const akl::Decimal<PLACES>& rhs) noexcept {
	 return akl::Decimal<PLACES>::FromShifted(lhs.AsShifted() + rhs.AsShifted());
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator-(const akl::Decimal<PLACES>& lhs, const akl::Decimal<PLACES>& rhs) noexcept {
	 return akl::Decimal<PLACES>::FromShifted(lhs.AsShifted() - rhs.AsShifted());
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator*(const akl::Decimal<PLACES>& lhs, const int rhs) noexcept {
	 return akl::Decimal<PLACES>::FromShifted(lhs.AsShifted() * rhs);
 }
 
 template<int PLACES>
 inline akl::Decimal<PLACES> operator*(const int lhs, const akl::Decimal<PLACES>& rhs) noexcept {
	 return akl::Decimal<PLACES>::FromShifted(lhs * rhs.AsShifted());
 }
 
 template<int PLACES>
 inline int operator/(const akl::Decimal<PLACES>& lhs, const akl::Decimal<PLACES>& rhs) noexcept {
	 return static_cast<int>(lhs.AsShifted() / rhs.AsShifted());
 }
 
 template<int PLACES>
 inline std::ostream& operator<<(std::ostream& os, const akl::Decimal<PLACES>& q) noexcept {
	 // Output the shifted integer value; you can later extend this to show formatted output.
	 os << q.AsShifted();
	 return os;
 }
 
 #endif /* SRC_AKL_DECIMAL_HPP_ */
 