#pragma once

#include <Orders/OrderEnumsV2.hpp>
#include <exchsupport/cme/mdp3/Mdp3Messages.hpp>
#include <optional>
#include <charconv>
#include <limits>

namespace akl::stacker
{

inline constexpr KTN::ORD::KOrderSide::Enum CmeSideToKtnSide(const KTN::Mdp3::EnumAggressorSide side)
{
	switch (side)
	{
		case KTN::Mdp3::EnumAggressorSide::Buy:
			return KTN::ORD::KOrderSide::Enum::BUY;
		case KTN::Mdp3::EnumAggressorSide::Sell:
			return KTN::ORD::KOrderSide::Enum::SELL;
		default:
			return KTN::ORD::KOrderSide::Enum::UNKNOWN;
	}
}

inline constexpr KTN::Mdp3::EnumAggressorSide KtnSideToCmeSide(const KTN::ORD::KOrderSide::Enum side)
{
	switch (side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return KTN::Mdp3::EnumAggressorSide::Buy;
		case KTN::ORD::KOrderSide::Enum::SELL:
			return KTN::Mdp3::EnumAggressorSide::Sell;
		default:
			return KTN::Mdp3::EnumAggressorSide::None;
	}
}

inline constexpr KTN::ORD::KOrderSide::Enum AklSideToKtnSide(const int8_t side)
{
	switch (side)
	{
		case 0:
			return KTN::ORD::KOrderSide::Enum::BUY;
		case 1:
			return KTN::ORD::KOrderSide::Enum::SELL;
		default:
			return KTN::ORD::KOrderSide::Enum::UNKNOWN;
	}
}

inline constexpr std::optional<int8_t> KtnSideToAklSide(const KTN::ORD::KOrderSide::Enum side)
{
	switch (side)
	{
		case KTN::ORD::KOrderSide::Enum::BUY:
			return 0;
		case KTN::ORD::KOrderSide::Enum::SELL:
			return 1;
		default:
			return std::nullopt;
	}
}

inline uint64_t stringToUint64(const std::string &str)
{
	uint64_t result;
	const char *begin = str.data();
	const char *end = begin + str.size();
	std::from_chars_result ret = std::from_chars(begin, end, result);
	if (ret.ec == std::errc())
	{
		return result;
	}
	else
	{
		return 0;
	}
}

inline std::string uint64ToString(uint64_t n)
{
	char buffer[21];
	std::to_chars_result ret = std::to_chars(buffer, buffer + sizeof(buffer), n);
	return std::string(buffer, ret.ptr - buffer);
}

inline std::optional<int32_t> stringToInt32(const std::string &str)
{
    int32_t result;
    const char *begin = str.data();
    const char *end = begin + str.size();
    std::from_chars_result ret = std::from_chars(begin, end, result);
    if (ret.ec == std::errc())
    {
        return result;
    }
    else
    {
        return std::nullopt;
    }
}

inline bool isPriceValid(const int64_t price)
{
	return price != std::numeric_limits<int64_t>::max() && price != std::numeric_limits<int64_t>::min();
}

}