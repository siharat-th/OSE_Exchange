#pragma once

#include <optional>
#include <string>
#include <vector>
#include <charconv>

#include <Orders/OrderEnumsV2.hpp>
#include <akl/Price.hpp>
#include <akl/Quantity.hpp>
#include <AlgoSupport/algo_structs.hpp>
#include <KT01/Core/Log.hpp>

#include <rapidjson/document.h>

namespace akl::stacker
{

class PriceConverter;

inline std::optional<Price> stringToPrice(const std::string &str)
{
	int64_t result;
	const char *begin = str.data();
	const char *end = begin + str.size();
	std::from_chars_result ret = std::from_chars(begin, end, result);
	if (ret.ec == std::errc())
	{
		return Price::FromShifted(result);
	}
	else
	{
		return std::nullopt;
	}
}

inline bool parsePriceParam(const rapidjson::GenericValue<rapidjson::UTF8<>> &doc, const char *paramName, std::optional<Price> &paramValue)
{
	if (doc.HasMember(paramName) && doc[paramName].IsString() && strlen(doc[paramName].GetString()) > 0)
	{
		const std::optional<Price> temp = stringToPrice(doc[paramName].GetString());
		if (temp)
		{
			const bool result = paramValue != *temp;
			paramValue = *temp;
			return result;
		}
		else
		{
			LOGWARN("Failed to parse {}: {}", paramName, doc[paramName].GetString());
			const bool result = paramValue != std::nullopt;
			paramValue = std::nullopt;
			return result;
		}
	}
	else if (doc.HasMember(paramName) && doc[paramName].IsString() &&
			 strlen(doc[paramName].GetString()) == 0)
	{
		const bool result = paramValue != std::nullopt;
		paramValue = std::nullopt;
		return result;
	}
	else
	{
		return false;
	}
}

inline std::string priceToString(const std::optional<Price> &px)
{
	if (px)
	{
		char buffer[21];
		std::to_chars_result ret = std::to_chars(buffer, buffer + sizeof(buffer), px->AsShifted());
		return std::string(buffer, ret.ptr - buffer);
	}
	else
	{
		return "";
	}
}

inline void setPriceParam(rapidjson::Value &doc, const char *paramName, const std::optional<Price> &paramValue, rapidjson::Document::AllocatorType &allocator)
{
	if (paramValue)
	{
		doc.AddMember(rapidjson::GenericValue<rapidjson::UTF8<>>::StringRefType(paramName), rapidjson::Value(priceToString(paramValue).c_str(), allocator), allocator);
	}
	else
	{
		doc.AddMember(rapidjson::GenericValue<rapidjson::UTF8<>>::StringRefType(paramName), rapidjson::Value("", allocator), allocator);
	}
}

struct StackSideSettings
{
	Quantity qty{5};
	int length{1};
	std::optional<Price> insidePrice{std::nullopt};
	std::optional<Price> outsidePrice{std::nullopt};

	bool operator==(const StackSideSettings &other) const
	{
		return qty == other.qty && length == other.length && insidePrice == other.insidePrice &&
			   outsidePrice == other.outsidePrice;
	}

	bool operator!=(const StackSideSettings &other) const
	{
		return !(*this == other);
	}
};

struct QuoteStackSideSettings : public StackSideSettings
{
	std::optional<Price> altManualAdjPrice{std::nullopt};
	Quantity lean{100};
	Quantity quickAddLean{100};
	int quickAddTime{0};
	int manualAdj{0};
	int altManualAdj{0};
	bool quickAddEnabled{false};
	bool noAggressing{false};

	bool operator==(const QuoteStackSideSettings &other) const
	{
		return static_cast<const StackSideSettings &>(*this) == static_cast<const StackSideSettings &>(other) &&
			   altManualAdjPrice == other.altManualAdjPrice && lean == other.lean &&
			   quickAddLean == other.quickAddLean && quickAddTime == other.quickAddTime &&
			   manualAdj == other.manualAdj && altManualAdj == other.altManualAdj &&
			   quickAddEnabled == other.quickAddEnabled && noAggressing == other.noAggressing;
	}

	bool operator!=(const QuoteStackSideSettings &other) const
	{
		return !(*this == other);
	}
};

struct QuoteSideSettings
{
	std::string prorataProfile{""};
	std::string alternateProrataProfile{""};
	std::string dynamicProfile{""};
	QuoteStackSideSettings stackSideSettings[2];
	Quantity payup{100};
	Quantity winnerPayup{100};
	Quantity addHedgeQty{0}; // TODO remove this?
	int maxPayupTicks{0};
	int timeoutCover{0};
	int alternateProrataProfileDelay{0};
	bool prorataEnabled{false};
	bool addHedgeEnabled{false};
	bool dynamicProfileEnabled{false};

	bool operator==(const QuoteSideSettings &other) const
	{
		return prorataProfile == other.prorataProfile && alternateProrataProfile == other.alternateProrataProfile &&
			   dynamicProfile == other.dynamicProfile && stackSideSettings[0] == other.stackSideSettings[0] &&
			   stackSideSettings[1] == other.stackSideSettings[1] && payup == other.payup &&
			   winnerPayup == other.winnerPayup && addHedgeQty == other.addHedgeQty &&
			   maxPayupTicks == other.maxPayupTicks && timeoutCover == other.timeoutCover &&
			   alternateProrataProfileDelay == other.alternateProrataProfileDelay &&
			   prorataEnabled == other.prorataEnabled && addHedgeEnabled == other.addHedgeEnabled &&
			   dynamicProfileEnabled == other.dynamicProfileEnabled;
	}

	bool operator!=(const QuoteSideSettings &other) const
	{
		return !(*this == other);
	}
};

struct InstrumentRiskSettings
{
	Quantity maxPosition { 0 };
	Quantity maxOrderQty { 0 };

	bool operator==(const InstrumentRiskSettings &other) const
	{
		return maxPosition == other.maxPosition && maxOrderQty == other.maxOrderQty;
	}

	bool operator!=(const InstrumentRiskSettings &other) const
	{
		return !(*this == other);
	}
};

class RateLimitSettings
{
  public:
	int limit{50};
	int window{1000};

	bool operator==(const RateLimitSettings &other) const
	{
		return limit == other.limit && window == other.window;
	}

	bool operator!=(const RateLimitSettings &other) const
	{
		return !(*this == other);
	}
};

// Added for leg information
struct LegInfo
{
	int legId{0};
	std::string exchange{""};
	std::string symbol{""};
	std::string prodType{""};
	bool inverse{false};
	double priceMult{0.0};
	double spreadMult{0.0};
	bool activeQuoting{false};

	bool operator==(const LegInfo &other) const
	{
		return legId == other.legId && exchange == other.exchange && symbol == other.symbol &&
			   prodType == other.prodType && inverse == other.inverse && priceMult == other.priceMult &&
			   spreadMult == other.spreadMult && activeQuoting == other.activeQuoting;
	}

	bool operator!=(const LegInfo &other) const
	{
		return !(*this == other);
	}
};

struct QuoteInstrumentSettings
{
	static constexpr int MAX_REQUOTE_COUNT{2};

	InstrumentRiskSettings riskSettings;
	RateLimitSettings rateLimitSettings;
	QuoteSideSettings bidSettings;
	QuoteSideSettings askSettings;
	std::string prorataGroup{""};
	double prorataAllocation{0.0};
	int quoteTimeout{50};
	int prorataQuoteTimeout{1000};
	int leanPayupIncrement{1};
	int prorataRequoteCount{0};
	int requoteCount{0};
	int requoteDelay{5000};
	int timeInForce{0};
	bool newOrders{false};
	bool moveWithCancelNew{false};
	bool requoteOnMove{false};
	bool dynamicStack{false};
	bool noPayup{false};		   // TODO remove?
	bool quoteInPreopen{false}; // TODO rename?
	bool preventAlgoCancelWhenWorkingOrders{false};
	bool adoptFromUnknownStrategies{false};
	bool hedgeOnLargeTrade{false};
	bool hedgeOnLargeFill{false};
	bool cancelUnadoptedOrders{false};
	bool linkLeanPayup{false};
	bool secondStackEnabled{false};
	bool showProrata{true};
	bool showAltManualAdj{false};

	// Additional fields from JSON
	int requote{0};
	int clearPosition{0};
	int adopt{0};
	double randomLotSizeMult{0.0};
	int algoCancelAction{0};
	int strategyStatus{1};
	int instance{3};
	int disconnectAction{0};

	template <KTN::ORD::KOrderSide::Enum SIDE>
	const QuoteSideSettings &GetSideSettings() const;

	bool operator==(const QuoteInstrumentSettings &other) const
	{
		return riskSettings == other.riskSettings && rateLimitSettings == other.rateLimitSettings &&
			   bidSettings == other.bidSettings && askSettings == other.askSettings &&
			   prorataGroup == other.prorataGroup && prorataAllocation == other.prorataAllocation &&
			   quoteTimeout == other.quoteTimeout && prorataQuoteTimeout == other.prorataQuoteTimeout &&
			   leanPayupIncrement == other.leanPayupIncrement && prorataRequoteCount == other.prorataRequoteCount &&
			   requoteCount == other.requoteCount && requoteDelay == other.requoteDelay && timeInForce == other.timeInForce &&
			   newOrders == other.newOrders && moveWithCancelNew == other.moveWithCancelNew &&
			   requoteOnMove == other.requoteOnMove && dynamicStack == other.dynamicStack && noPayup == other.noPayup &&
			   quoteInPreopen == other.quoteInPreopen &&
			   preventAlgoCancelWhenWorkingOrders == other.preventAlgoCancelWhenWorkingOrders &&
			   adoptFromUnknownStrategies == other.adoptFromUnknownStrategies &&
			   hedgeOnLargeTrade == other.hedgeOnLargeTrade && hedgeOnLargeFill == other.hedgeOnLargeFill &&
			   cancelUnadoptedOrders == other.cancelUnadoptedOrders && linkLeanPayup == other.linkLeanPayup &&
			   secondStackEnabled == other.secondStackEnabled && showProrata == other.showProrata &&
			   showAltManualAdj == other.showAltManualAdj && requote == other.requote &&
			   clearPosition == other.clearPosition && adopt == other.adopt &&
			   randomLotSizeMult == other.randomLotSizeMult && algoCancelAction == other.algoCancelAction &&
			   strategyStatus == other.strategyStatus && instance == other.instance &&
			   disconnectAction == other.disconnectAction;
	}

	bool operator!=(const QuoteInstrumentSettings &other) const
	{
		return !(*this == other);
	}
};

struct StopSettings
{
  public:
	int stopQuantities[5]{1, 5, 10, 50, 100};
	int stopTriggers[5]{1, 5, 10, 50, 100};
	int stopLean[5]{1, 5, 10, 50, 100};
	int middleClickQuantity{0};
	int middleClickTrigger{0};
	int stopQuantityMax{100};
	int flipDelay{1000};
	int triggerQuoteDelay{1000};
	bool deleteOnFlipDefault{false};
	bool bidLinkEnabled{false};
	bool askLinkEnabled{false};
	bool showLeanColumn{false};
	bool showTradeColumn{false};

	// template<Side SIDE>
	// bool LinkEnabled() const;

	// template<Side SIDE>
	// void DisableLink();

	// bool DisableLink();

	bool operator==(const StopSettings &other) const
	{
		for (int i = 0; i < 5; ++i)
		{
			if (stopQuantities[i] != other.stopQuantities[i])
				return false;
			if (stopTriggers[i] != other.stopTriggers[i])
				return false;
			if (stopLean[i] != other.stopLean[i])
				return false;
		}

		return middleClickQuantity == other.middleClickQuantity && middleClickTrigger == other.middleClickTrigger &&
			   stopQuantityMax == other.stopQuantityMax && flipDelay == other.flipDelay &&
			   triggerQuoteDelay == other.triggerQuoteDelay && deleteOnFlipDefault == other.deleteOnFlipDefault &&
			   bidLinkEnabled == other.bidLinkEnabled && askLinkEnabled == other.askLinkEnabled &&
			   showLeanColumn == other.showLeanColumn && showTradeColumn == other.showTradeColumn;
	}

	bool operator!=(const StopSettings &other) const
	{
		return !(*this == other);
	}
};

struct SpreaderPricingSettings
{
	int addTicks[3] { 3, 5, 7 };
	int removeTicks[3] { 2, 4, 6 };
	Quantity lockQty[3] { Quantity(100'000), Quantity(100'000), Quantity(100'000) };
	bool bidEnabled[3] { false, false, false };
	bool askEnabled[3] { false, false, false };
	Quantity resetQty { Quantity(0) };
	int tickVariance { 0 };

	template<KTN::ORD::KOrderSide::Enum SIDE>
	bool SideEnabled(const int index) const
	{
		if constexpr (SIDE == KTN::ORD::KOrderSide::Enum::BUY)
		{
			return bidEnabled[index];
		}
		else
		{
			return askEnabled[index];
		}
	}
};

struct AklStackerParams
{
	struct ParseResult
	{
		bool priceBandsChanged { false };
	};

	static AklStackerParams parse(const std::string &jsonStr, const PriceConverter &priceConverter);
	ParseResult update(const std::string &jsonStr, const PriceConverter &priceConverter);
	std::string toJson() const;

	algometa meta;

	QuoteInstrumentSettings quoteInstrumentSettings;
	StopSettings stopSettings;
	SpreaderPricingSettings spreaderPricingSettings;

	// Additional fields for complete JSON structure
	std::string topic{"ALGOPARAMS,"};
	std::string source{""};
	std::string strategy{""};
	std::string uniqueId{""};
	std::vector<LegInfo> legs;

	bool reset{false};

	bool operator==(const AklStackerParams &other) const
	{
		if (quoteInstrumentSettings != other.quoteInstrumentSettings)
			return false;
		if (stopSettings != other.stopSettings)
			return false;
		if (topic != other.topic)
			return false;
		if (source != other.source)
			return false;
		if (strategy != other.strategy)
			return false;
		if (uniqueId != other.uniqueId)
			return false;

		// Check legs
		if (legs.size() != other.legs.size())
			return false;
		for (size_t i = 0; i < legs.size(); ++i)
		{
			if (legs[i] != other.legs[i])
				return false;
		}

		return true;
	}

	bool operator!=(const AklStackerParams &other) const
	{
		return !(*this == other);
	}
};

template<typename T, typename U = T>
bool SafeParse(const rapidjson::GenericValue<rapidjson::UTF8<>> &doc, const char *paramName, T &paramValue)
{
	if (doc.HasMember(paramName))
	{
		if (doc[paramName].Is<U>())
		{
			paramValue = T(doc[paramName].Get<U>());
			return true;
		}
		else
		{
			LOGWARN("AlgoParams has type mismatch for param {}", paramName);
			return false;
		}
	}
	else
	{
		LOGWARN("AlgoParams is missing param {}", paramName);
		return false;
	}
}

} // namespace akl::stacker