#include "AklStackerParams.hpp"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <Algos/AklStacker/PriceConverter.hpp>

namespace akl::stacker
{

template <>
const QuoteSideSettings &QuoteInstrumentSettings::GetSideSettings<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return bidSettings;
}

template <>
const QuoteSideSettings &QuoteInstrumentSettings::GetSideSettings<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return askSettings;
}

AklStackerParams AklStackerParams::parse(const std::string &jsonStr, const PriceConverter &priceConverter)
{
	AklStackerParams params;

	rapidjson::Document document;
	rapidjson::ParseResult result = document.Parse(jsonStr.c_str());

	if (!result)
	{
		std::string error = std::string("JSON parse error: ") + rapidjson::GetParseError_En(result.Code()) +
							" at offset " + std::to_string(result.Offset());
		throw std::runtime_error(error);
	}

	if (!document.IsArray() || document.Empty())
	{
		// LogMe("ALGO JSON OBJECT MUST BE POPULATED AND AN ARRAY!!!!", LOG_RED);
		// LogMe(json, LOG_RED);
		return params;
	}


	for (rapidjson::GenericValue<rapidjson::UTF8<>>::ConstValueIterator itr = document.Begin(); itr != document.End(); ++itr)
	{
		if (!(*itr).HasMember("GroupName") || !(*itr).HasMember("Org") || !(*itr).HasMember("User"))
		{
			cout << "ERROR: ALGO MUST CONTAIN USER/GROUPNAME/ORG!" << endl;
			return params;
		}
		params.meta.userinfo.groupname = (*itr)["GroupName"].GetString();
		params.meta.userinfo.org = (*itr)["Org"].GetString();
		params.meta.userinfo.user = (*itr)["User"].GetString();

		params.meta.Enabled = (*itr)["Enabled"].GetInt();

		params.meta.UniqueId = (*itr)["UniqueId"].GetString();
		params.meta.Strategy = (*itr)["Strategy"].GetString();
		params.meta.TemplateName = (*itr)["TemplateName"].GetString();
		params.meta.Source = (*itr)["Source"].GetString();
		params.meta.Tag = (*itr)["Tag"].GetString();

		break;
	}

	params.update(jsonStr, priceConverter);
	return params;
}

AklStackerParams::ParseResult AklStackerParams::update(const std::string &jsonStr, const PriceConverter &priceConverter)
{
	ParseResult pr;

	rapidjson::Document document;
	rapidjson::ParseResult result = document.Parse(jsonStr.c_str());

	if (!result)
	{
		std::string error = std::string("JSON parse error: ") + rapidjson::GetParseError_En(result.Code()) +
							" at offset " + std::to_string(result.Offset());
		throw std::runtime_error(error);
	}

	if (!document.IsArray() || document.Empty())
	{
		// LogMe("ALGO JSON OBJECT MUST BE POPULATED AND AN ARRAY!!!!", LOG_RED);
		// LogMe(json, LOG_RED);
		return pr;
	}

	const auto &doc = document[0];

	if (doc.HasMember("ResetPosition"))
	{
		reset = doc["ResetPosition"].GetBool();
		std::cout << "****SETTING RESET TO " << reset << std::endl;
	}

	if (doc.HasMember("Enabled"))
	{
		meta.Enabled = doc["Enabled"].GetInt();
	}

	// Parse metadata fields - only update if present in JSON
	if (doc.HasMember("topic"))
		topic = doc["topic"].GetString();
	if (doc.HasMember("source"))
		source = doc["source"].GetString();
	if (doc.HasMember("Strategy"))
		strategy = doc["Strategy"].GetString();
	if (doc.HasMember("UniqueId"))
		uniqueId = doc["UniqueId"].GetString();

	// Parse common settings - only update if present in JSON
	if (doc.HasMember("ProrataGroup"))
		quoteInstrumentSettings.prorataGroup = doc["ProrataGroup"].GetString();
	if (doc.HasMember("ProrataAllocation"))
		quoteInstrumentSettings.prorataAllocation = doc["ProrataAllocation"].GetDouble();
	if (doc.HasMember("LeanPayupIncrement"))
		quoteInstrumentSettings.leanPayupIncrement = doc["LeanPayupIncrement"].GetInt();
	if (doc.HasMember("ProrataRequoteCount"))
		quoteInstrumentSettings.prorataRequoteCount = doc["ProrataRequoteCount"].GetInt();
	if (doc.HasMember("NoPayup"))
		quoteInstrumentSettings.noPayup = doc["NoPayup"].GetBool();
	if (doc.HasMember("QuoteInPreopen"))
		quoteInstrumentSettings.quoteInPreopen = doc["QuoteInPreopen"].GetBool();
	if (doc.HasMember("PreventAlgoCancelWhenWorkingOrders"))
		quoteInstrumentSettings.preventAlgoCancelWhenWorkingOrders =
			doc["PreventAlgoCancelWhenWorkingOrders"].GetBool();
	if (doc.HasMember("AdoptFromUnknownStrategies"))
		quoteInstrumentSettings.adoptFromUnknownStrategies = doc["AdoptFromUnknownStrategies"].GetBool();
	if (doc.HasMember("HedgeOnLargeTrade"))
		quoteInstrumentSettings.hedgeOnLargeTrade = doc["HedgeOnLargeTrade"].GetBool();
	if (doc.HasMember("HedgeOnLargeFill"))
		quoteInstrumentSettings.hedgeOnLargeFill = doc["HedgeOnLargeFill"].GetBool();
	if (doc.HasMember("CancelUnadoptedOrders"))
		quoteInstrumentSettings.cancelUnadoptedOrders = doc["CancelUnadoptedOrders"].GetBool();
	if (doc.HasMember("LinkLeanPayup"))
		quoteInstrumentSettings.linkLeanPayup = doc["LinkLeanPayup"].GetBool();
	if (doc.HasMember("SecondStackEnabled"))
		quoteInstrumentSettings.secondStackEnabled = doc["SecondStackEnabled"].GetBool();
	if (doc.HasMember("ShowProrata"))
		quoteInstrumentSettings.showProrata = doc["ShowProrata"].GetBool();
	if (doc.HasMember("ShowAltManualAdj"))
		quoteInstrumentSettings.showAltManualAdj = doc["ShowAltManualAdj"].GetBool();
	if (doc.HasMember("NewOrders"))
		quoteInstrumentSettings.newOrders = doc["NewOrders"].GetBool();
	if (doc.HasMember("MoveWithCancelNew"))
		quoteInstrumentSettings.moveWithCancelNew = doc["MoveWithCancelNew"].GetBool();
	if (doc.HasMember("RequoteOnMove"))
		quoteInstrumentSettings.requoteOnMove = doc["RequoteOnMove"].GetBool();
	if (doc.HasMember("DynamicStack"))
		quoteInstrumentSettings.dynamicStack = doc["DynamicStack"].GetBool();

	// Added fields - only update if present in JSON
	if (doc.HasMember("Requote"))
		quoteInstrumentSettings.requote = doc["Requote"].GetInt();
	if (doc.HasMember("ClearPosition"))
		quoteInstrumentSettings.clearPosition = doc["ClearPosition"].GetInt();
	if (doc.HasMember("Adopt"))
		quoteInstrumentSettings.adopt = doc["Adopt"].GetInt();
	if (doc.HasMember("RandomLotSizeMult"))
		quoteInstrumentSettings.randomLotSizeMult = doc["RandomLotSizeMult"].GetDouble();
	if (doc.HasMember("AlgoCancelAction"))
		quoteInstrumentSettings.algoCancelAction = doc["AlgoCancelAction"].GetInt();
	if (doc.HasMember("StrategyStatus"))
		quoteInstrumentSettings.strategyStatus = doc["StrategyStatus"].GetInt();
	if (doc.HasMember("Instance"))
		quoteInstrumentSettings.instance = doc["Instance"].GetInt();
	if (doc.HasMember("DisconnectAction"))
		quoteInstrumentSettings.disconnectAction = doc["DisconnectAction"].GetInt();
	if (doc.HasMember("StackRequoteCount"))
	{
		quoteInstrumentSettings.requoteCount = doc["StackRequoteCount"].GetInt();
		if (quoteInstrumentSettings.requoteCount > QuoteInstrumentSettings::MAX_REQUOTE_COUNT)
		{
			quoteInstrumentSettings.requoteCount = QuoteInstrumentSettings::MAX_REQUOTE_COUNT;
			// TODO Log?
		}
	}
	if (doc.HasMember("StackRequoteDelay"))
		quoteInstrumentSettings.requoteDelay = doc["StackRequoteDelay"].GetInt();
	if (doc.HasMember("TimeInForce"))
		quoteInstrumentSettings.timeInForce = doc["TimeInForce"].GetInt();

	// Risk settings - only update if present in JSON
	if (doc.HasMember("MaxOrderQty"))
		quoteInstrumentSettings.riskSettings.maxOrderQty = Quantity(doc["MaxOrderQty"].GetInt());
	if (doc.HasMember("MaxPosition"))
		quoteInstrumentSettings.riskSettings.maxPosition = Quantity(doc["MaxPosition"].GetInt());

	// Rate limit settings - only update if present in JSON
	if (doc.HasMember("RateLimit"))
		quoteInstrumentSettings.rateLimitSettings.limit = doc["RateLimit"].GetInt();
	if (doc.HasMember("RateLimitWindow"))
		quoteInstrumentSettings.rateLimitSettings.window = doc["RateLimitWindow"].GetInt();

	// Parse quoteTimeout and prorataQuoteTimeout from StackRequoteDelay - only update if present
	if (doc.HasMember("QuoteTimeout"))
		quoteInstrumentSettings.quoteTimeout = doc["QuoteTimeout"].GetInt();
	if (doc.HasMember("ProrataQuoteTimeout"))
		quoteInstrumentSettings.prorataQuoteTimeout = doc["ProrataQuoteTimeout"].GetInt();

	// Parse BID side settings
	auto &bidSettings = quoteInstrumentSettings.bidSettings;

	// Bid side main settings - only update if present in JSON
	if (doc.HasMember("BidProrataProfile"))
		bidSettings.prorataProfile = doc["BidProrataProfile"].GetString();
	if (doc.HasMember("BidAltProrataProfile"))
		bidSettings.alternateProrataProfile = doc["BidAltProrataProfile"].GetString();
	// Not updating alternateProrataProfile as there's no field for it in the JSON
	if (doc.HasMember("BidDynamicProfile"))
		bidSettings.dynamicProfile = doc["BidDynamicProfile"].GetString();
	if (doc.HasMember("BidPayup"))
		bidSettings.payup = Quantity(doc["BidPayup"].GetInt());
	if (doc.HasMember("BidWinnerPayup"))
		bidSettings.winnerPayup = Quantity(doc["BidWinnerPayup"].GetInt());
	if (doc.HasMember("BidAddHedgeQty"))
		bidSettings.addHedgeQty = Quantity(doc["BidAddHedgeQty"].GetInt());
	if (doc.HasMember("BidMaxPayupTicks"))
		bidSettings.maxPayupTicks = doc["BidMaxPayupTicks"].GetInt();
	if (doc.HasMember("BidTimeoutCover"))
		bidSettings.timeoutCover = doc["BidTimeoutCover"].GetInt();
	if (doc.HasMember("BidAltProrataProfileDelay"))
		bidSettings.alternateProrataProfileDelay = doc["BidAltProrataProfileDelay"].GetInt();
	if (doc.HasMember("BidProrataEnabled"))
		bidSettings.prorataEnabled = doc["BidProrataEnabled"].GetBool();
	// Not updating addHedgeEnabled as there's no field for it in the JSON
	if (doc.HasMember("BidDynamicEnabled"))
		bidSettings.dynamicProfileEnabled = doc["BidDynamicEnabled"].GetBool();

	// Bid stack side settings - First stack (index 0) - only update if present in JSON
	auto &bidStack1 = bidSettings.stackSideSettings[0];
	if (doc.HasMember("BidQty"))
		bidStack1.qty = Quantity(doc["BidQty"].GetInt());
	if (doc.HasMember("BidLength"))
		bidStack1.length = doc["BidLength"].GetInt();
	if (doc.HasMember("BidLean"))
		bidStack1.lean = Quantity(doc["BidLean"].GetInt());
	if (doc.HasMember("BidQuickAddLean"))
		bidStack1.quickAddLean = Quantity(doc["BidQuickAddLean"].GetInt());
	if (doc.HasMember("BidQuickAddTime"))
		bidStack1.quickAddTime = doc["BidQuickAddTime"].GetInt();
	if (doc.HasMember("BidManualAdj"))
		bidStack1.manualAdj = doc["BidManualAdj"].GetInt();
	if (doc.HasMember("BidAltManualAdj"))
		bidStack1.altManualAdj = doc["BidAltManualAdj"].GetInt();
	if (doc.HasMember("BidQuickAddEnabled"))
		bidStack1.quickAddEnabled = doc["BidQuickAddEnabled"].GetBool();
	if (doc.HasMember("BidNoAggressing"))
		bidStack1.noAggressing = doc["BidNoAggressing"].GetBool();

	// Set price values if available
	pr.priceBandsChanged |= parsePriceParam(doc, "BidMaxPrice", bidStack1.insidePrice);
	pr.priceBandsChanged |= parsePriceParam(doc, "BidMinPrice", bidStack1.outsidePrice);
	pr.priceBandsChanged |= parsePriceParam(doc, "BidAltPrice", bidStack1.altManualAdjPrice);

	// Bid stack side settings - Second stack (index 1) - only update if present in JSON
	auto &bidStack2 = bidSettings.stackSideSettings[1];
	if (doc.HasMember("BidQty2"))
		bidStack2.qty = Quantity(doc["BidQty2"].GetInt());
	if (doc.HasMember("BidLength2"))
		bidStack2.length = doc["BidLength2"].GetInt();
	if (doc.HasMember("BidLean2"))
		bidStack2.lean = Quantity(doc["BidLean2"].GetInt());
	if (doc.HasMember("BidQuickAddLean2"))
		bidStack2.quickAddLean = Quantity(doc["BidQuickAddLean2"].GetInt());
	if (doc.HasMember("BidQuickAddTime2"))
		bidStack2.quickAddTime = doc["BidQuickAddTime2"].GetInt();
	if (doc.HasMember("BidManualAdj2"))
		bidStack2.manualAdj = doc["BidManualAdj2"].GetInt();
	if (doc.HasMember("BidAltManualAdj2"))
		bidStack2.altManualAdj = doc["BidAltManualAdj2"].GetInt();
	if (doc.HasMember("BidQuickAddEnabled2"))
		bidStack2.quickAddEnabled = doc["BidQuickAddEnabled2"].GetBool();
	if (doc.HasMember("BidNoAggressing2"))
		bidStack2.noAggressing = doc["BidNoAggressing2"].GetBool();

	// Set price values if available
	pr.priceBandsChanged |= parsePriceParam(doc, "BidMaxPrice2", bidStack2.insidePrice);
	pr.priceBandsChanged |= parsePriceParam(doc, "BidMinPrice2", bidStack2.outsidePrice);
	pr.priceBandsChanged |= parsePriceParam(doc, "BidAltPrice2", bidStack2.altManualAdjPrice);

	// Parse ASK side settings
	auto &askSettings = quoteInstrumentSettings.askSettings;

	// Ask side main settings - only update if present in JSON
	if (doc.HasMember("AskProrataProfile"))
		askSettings.prorataProfile = doc["AskProrataProfile"].GetString();
	if (doc.HasMember("AskAltProrataProfile"))
		askSettings.alternateProrataProfile = doc["AskAltProrataProfile"].GetString();
	// Not updating alternateProrataProfile as there's no field for it in the JSON
	if (doc.HasMember("AskDynamicProfile"))
		askSettings.dynamicProfile = doc["AskDynamicProfile"].GetString();
	if (doc.HasMember("AskPayup"))
		askSettings.payup = Quantity(doc["AskPayup"].GetInt());
	if (doc.HasMember("AskWinnerPayup"))
		askSettings.winnerPayup = Quantity(doc["AskWinnerPayup"].GetInt());
	if (doc.HasMember("AskAddHedgeQty"))
		askSettings.addHedgeQty = Quantity(doc["AskAddHedgeQty"].GetInt());
	if (doc.HasMember("AskMaxPayupTicks"))
		askSettings.maxPayupTicks = doc["AskMaxPayupTicks"].GetInt();
	if (doc.HasMember("AskTimeoutCover"))
		askSettings.timeoutCover = doc["AskTimeoutCover"].GetInt();
	if (doc.HasMember("AskAltProrataProfileDelay"))
		askSettings.alternateProrataProfileDelay = doc["AskAltProrataProfileDelay"].GetInt();
	if (doc.HasMember("AskProrataEnabled"))
		askSettings.prorataEnabled = doc["AskProrataEnabled"].GetBool();
	// Not updating addHedgeEnabled as there's no field for it in the JSON
	if (doc.HasMember("AskDynamicEnabled"))
		askSettings.dynamicProfileEnabled = doc["AskDynamicEnabled"].GetBool();

	// Ask stack side settings - First stack (index 0) - only update if present in JSON
	auto &askStack1 = askSettings.stackSideSettings[0];
	if (doc.HasMember("AskQty"))
		askStack1.qty = Quantity(doc["AskQty"].GetInt());
	if (doc.HasMember("AskLength"))
		askStack1.length = doc["AskLength"].GetInt();
	if (doc.HasMember("AskLean"))
		askStack1.lean = Quantity(doc["AskLean"].GetInt());
	if (doc.HasMember("AskQuickAddLean"))
		askStack1.quickAddLean = Quantity(doc["AskQuickAddLean"].GetInt());
	if (doc.HasMember("AskQuickAddTime"))
		askStack1.quickAddTime = doc["AskQuickAddTime"].GetInt();
	if (doc.HasMember("AskManualAdj"))
		askStack1.manualAdj = doc["AskManualAdj"].GetInt();
	if (doc.HasMember("AskAltManualAdj"))
		askStack1.altManualAdj = doc["AskAltManualAdj"].GetInt();
	if (doc.HasMember("AskQuickAddEnabled"))
		askStack1.quickAddEnabled = doc["AskQuickAddEnabled"].GetBool();
	if (doc.HasMember("AskNoAggressing"))
		askStack1.noAggressing = doc["AskNoAggressing"].GetBool();

	// Set price values if available
	pr.priceBandsChanged |= parsePriceParam(doc, "AskMaxPrice", 	askStack1.outsidePrice);
	pr.priceBandsChanged |= parsePriceParam(doc, "AskMinPrice", askStack1.insidePrice);
	pr.priceBandsChanged |= parsePriceParam(doc, "AskAltPrice", askStack1.altManualAdjPrice);

	// Ask stack side settings - Second stack (index 1) - only update if present in JSON
	auto &askStack2 = askSettings.stackSideSettings[1];
	if (doc.HasMember("AskQty2"))
		askStack2.qty = Quantity(doc["AskQty2"].GetInt());
	if (doc.HasMember("AskLength2"))
		askStack2.length = doc["AskLength2"].GetInt();
	if (doc.HasMember("AskLean2"))
		askStack2.lean = Quantity(doc["AskLean2"].GetInt());
	if (doc.HasMember("AskQuickAddLean2"))
		askStack2.quickAddLean = Quantity(doc["AskQuickAddLean2"].GetInt());
	if (doc.HasMember("AskQuickAddTime2"))
		askStack2.quickAddTime = doc["AskQuickAddTime2"].GetInt();
	if (doc.HasMember("AskManualAdj2"))
		askStack2.manualAdj = doc["AskManualAdj2"].GetInt();
	if (doc.HasMember("AskAltManualAdj2"))
		askStack2.altManualAdj = doc["AskAltManualAdj2"].GetInt();
	if (doc.HasMember("AskQuickAddEnabled2"))
		askStack2.quickAddEnabled = doc["AskQuickAddEnabled2"].GetBool();
	if (doc.HasMember("AskNoAggressing2"))
		askStack2.noAggressing = doc["AskNoAggressing2"].GetBool();

	// Set price values if available
	pr.priceBandsChanged |= parsePriceParam(doc, "AskMaxPrice2", askStack2.outsidePrice);
	pr.priceBandsChanged |= parsePriceParam(doc, "AskMinPrice2", askStack2.insidePrice);
	pr.priceBandsChanged |= parsePriceParam(doc, "AskAltPrice2", askStack2.altManualAdjPrice);

	// Parse stop quantities - only update if present in JSON
	if (doc.HasMember("StopQuantity1"))
		stopSettings.stopQuantities[0] = doc["StopQuantity1"].GetInt();
	if (doc.HasMember("StopQuantity2"))
		stopSettings.stopQuantities[1] = doc["StopQuantity2"].GetInt();
	if (doc.HasMember("StopQuantity3"))
		stopSettings.stopQuantities[2] = doc["StopQuantity3"].GetInt();
	if (doc.HasMember("StopQuantity4"))
		stopSettings.stopQuantities[3] = doc["StopQuantity4"].GetInt();
	if (doc.HasMember("StopQuantity5"))
		stopSettings.stopQuantities[4] = doc["StopQuantity5"].GetInt();

	// Parse stop triggers - only update if present in JSON
	if (doc.HasMember("StopTrigger1"))
		stopSettings.stopTriggers[0] = doc["StopTrigger1"].GetInt();
	if (doc.HasMember("StopTrigger2"))
		stopSettings.stopTriggers[1] = doc["StopTrigger2"].GetInt();
	if (doc.HasMember("StopTrigger3"))
		stopSettings.stopTriggers[2] = doc["StopTrigger3"].GetInt();
	if (doc.HasMember("StopTrigger4"))
		stopSettings.stopTriggers[3] = doc["StopTrigger4"].GetInt();
	if (doc.HasMember("StopTrigger5"))
		stopSettings.stopTriggers[4] = doc["StopTrigger5"].GetInt();

	// Parse stop lean - only update if present in JSON
	if (doc.HasMember("StopLean1"))
		stopSettings.stopLean[0] = doc["StopLean1"].GetInt();
	if (doc.HasMember("StopLean2"))
		stopSettings.stopLean[1] = doc["StopLean2"].GetInt();
	if (doc.HasMember("StopLean3"))
		stopSettings.stopLean[2] = doc["StopLean3"].GetInt();
	if (doc.HasMember("StopLean4"))
		stopSettings.stopLean[3] = doc["StopLean4"].GetInt();
	if (doc.HasMember("StopLean5"))
		stopSettings.stopLean[4] = doc["StopLean5"].GetInt();

	// Parse other stop settings - only update if present in JSON
	if (doc.HasMember("MiddleClickQuantity"))
		stopSettings.middleClickQuantity = doc["MiddleClickQuantity"].GetInt();
	if (doc.HasMember("MiddleClickTrigger"))
		stopSettings.middleClickTrigger = doc["MiddleClickTrigger"].GetInt();
	if (doc.HasMember("StopQuantityMax"))
		stopSettings.stopQuantityMax = doc["StopQuantityMax"].GetInt();
	if (doc.HasMember("FlipDelay"))
		stopSettings.flipDelay = doc["FlipDelay"].GetInt();
	if (doc.HasMember("TriggerQuoteDelay"))
		stopSettings.triggerQuoteDelay = doc["TriggerQuoteDelay"].GetInt();
	if (doc.HasMember("DeleteOnFlipDefault"))
		stopSettings.deleteOnFlipDefault = doc["DeleteOnFlipDefault"].GetBool();
	if (doc.HasMember("ShowLeanColumn"))
		stopSettings.showLeanColumn = doc["ShowLeanColumn"].GetBool();
	if (doc.HasMember("ShowTradeColumn"))
		stopSettings.showTradeColumn = doc["ShowTradeColumn"].GetBool();

	SafeParse(doc, "SpreaderBidEnabled", spreaderPricingSettings.bidEnabled[0]);
	SafeParse(doc, "SpreaderBidEnabled2", spreaderPricingSettings.bidEnabled[1]);
	SafeParse(doc, "SpreaderBidEnabledProrata", spreaderPricingSettings.bidEnabled[2]);
	SafeParse(doc, "SpreaderAskEnabled", spreaderPricingSettings.askEnabled[0]);
	SafeParse(doc, "SpreaderAskEnabled2", spreaderPricingSettings.askEnabled[1]);
	SafeParse(doc, "SpreaderAskEnabledProrata", spreaderPricingSettings.askEnabled[2]);
	SafeParse(doc, "SpreaderAddTicks", spreaderPricingSettings.addTicks[0]);
	SafeParse(doc, "SpreaderAddTicks2", spreaderPricingSettings.addTicks[1]);
	SafeParse(doc, "SpreaderAddTicksProrata", spreaderPricingSettings.addTicks[2]);
	SafeParse(doc, "SpreaderRemoveTicks", spreaderPricingSettings.removeTicks[0]);
	SafeParse(doc, "SpreaderRemoveTicks2", spreaderPricingSettings.removeTicks[1]);
	SafeParse(doc, "SpreaderRemoveTicksProrata", spreaderPricingSettings.removeTicks[2]);
	SafeParse<Quantity, int>(doc, "SpreaderLockQty", spreaderPricingSettings.lockQty[0]);
	SafeParse<Quantity, int>(doc, "SpreaderLockQty2", spreaderPricingSettings.lockQty[1]);
	SafeParse<Quantity, int>(doc, "SpreaderLockQtyProrata", spreaderPricingSettings.lockQty[2]);
	SafeParse<Quantity, int>(doc, "SpreaderResetQty", spreaderPricingSettings.resetQty);
	SafeParse(doc, "SpreaderTickVariance", spreaderPricingSettings.tickVariance);

	// bidLinkEnabled and askLinkEnabled aren't in the JSON, so we're not updating them

	// Parse legs information if present
	if (doc.HasMember("Legs") && doc["Legs"].IsArray())
	{
		// Only clear and replace legs if the field is present
		legs.clear();
		for (const auto &leg : doc["Legs"].GetArray())
		{
			LegInfo legInfo;
			if (leg.HasMember("LegId"))
				legInfo.legId = leg["LegId"].GetInt();
			if (leg.HasMember("Exch"))
				legInfo.exchange = leg["Exch"].GetString();
			if (leg.HasMember("Symbol"))
				legInfo.symbol = leg["Symbol"].GetString();
			if (leg.HasMember("ProdType"))
				legInfo.prodType = leg["ProdType"].GetString();
			if (leg.HasMember("Inverse"))
				legInfo.inverse = leg["Inverse"].GetBool();
			if (leg.HasMember("PriceMult"))
				legInfo.priceMult = leg["PriceMult"].GetDouble();
			if (leg.HasMember("SpreadMult"))
				legInfo.spreadMult = leg["SpreadMult"].GetDouble();
			if (leg.HasMember("ActiveQuoting"))
				legInfo.activeQuoting = leg["ActiveQuoting"].GetBool();
			legs.push_back(legInfo);
		}
	}

	return pr;
}

std::string AklStackerParams::toJson() const
{
	// Create a document with an array as the root
	rapidjson::Document document;
	document.SetArray();
	rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

	// Create the main object inside the array
	rapidjson::Value mainObject(rapidjson::kObjectType);

	const auto &quoteSettings = this->quoteInstrumentSettings;
	const auto &bidSettings = quoteSettings.bidSettings;
	const auto &askSettings = quoteSettings.askSettings;
	const auto &stopSettings = this->stopSettings;

	// Add metadata fields
	mainObject.AddMember("topic", rapidjson::Value(topic.c_str(), allocator), allocator);
	mainObject.AddMember("source", rapidjson::Value(source.c_str(), allocator), allocator);
	mainObject.AddMember("Strategy", rapidjson::Value(strategy.c_str(), allocator), allocator);
	mainObject.AddMember("UniqueId", rapidjson::Value(uniqueId.c_str(), allocator), allocator);

	// Add added common settings
	mainObject.AddMember("Requote", quoteSettings.requote, allocator);
	mainObject.AddMember("ClearPosition", quoteSettings.clearPosition, allocator);
	mainObject.AddMember("Adopt", quoteSettings.adopt, allocator);
	mainObject.AddMember("NewOrders", quoteSettings.newOrders, allocator);
	mainObject.AddMember("MoveWithCancelNew", quoteSettings.moveWithCancelNew, allocator);
	mainObject.AddMember("RequoteOnMove", quoteSettings.requoteOnMove, allocator);
	mainObject.AddMember("RandomLotSizeMult", quoteSettings.randomLotSizeMult, allocator);
	mainObject.AddMember("AlgoCancelAction", quoteSettings.algoCancelAction, allocator);
	mainObject.AddMember("StrategyStatus", quoteSettings.strategyStatus, allocator);
	mainObject.AddMember("Instance", quoteSettings.instance, allocator);
	mainObject.AddMember("DisconnectAction", quoteSettings.disconnectAction, allocator);
	mainObject.AddMember("StackRequoteCount", quoteSettings.requoteCount, allocator);
	mainObject.AddMember("StackRequoteDelay", quoteSettings.requoteDelay, allocator);
	mainObject.AddMember("DynamicStack", quoteSettings.dynamicStack, allocator);
	mainObject.AddMember("TimeInForce", quoteSettings.timeInForce == 1 ? 1 : 0, allocator);

	// Add common settings to the JSON
	mainObject.AddMember("ProrataGroup", rapidjson::Value(quoteSettings.prorataGroup.c_str(), allocator), allocator);
	mainObject.AddMember("ProrataAllocation", quoteSettings.prorataAllocation, allocator);
	mainObject.AddMember("LeanPayupIncrement", quoteSettings.leanPayupIncrement, allocator);
	mainObject.AddMember("ProrataRequoteCount", quoteSettings.prorataRequoteCount, allocator);
	mainObject.AddMember("NoPayup", quoteSettings.noPayup, allocator);
	mainObject.AddMember("QuoteInPreopen", quoteSettings.quoteInPreopen, allocator);
	mainObject.AddMember("PreventAlgoCancelWhenWorkingOrders", quoteSettings.preventAlgoCancelWhenWorkingOrders,
						 allocator);
	mainObject.AddMember("AdoptFromUnknownStrategies", quoteSettings.adoptFromUnknownStrategies, allocator);
	mainObject.AddMember("HedgeOnLargeTrade", quoteSettings.hedgeOnLargeTrade, allocator);
	mainObject.AddMember("HedgeOnLargeFill", quoteSettings.hedgeOnLargeFill, allocator);
	mainObject.AddMember("CancelUnadoptedOrders", quoteSettings.cancelUnadoptedOrders, allocator);
	mainObject.AddMember("LinkLeanPayup", quoteSettings.linkLeanPayup, allocator);
	mainObject.AddMember("SecondStackEnabled", quoteSettings.secondStackEnabled, allocator);
	mainObject.AddMember("ShowProrata", quoteSettings.showProrata, allocator);
	mainObject.AddMember("ShowAltManualAdj", quoteSettings.showAltManualAdj, allocator);

	// Add risk settings
	mainObject.AddMember("MaxOrderQty", quoteSettings.riskSettings.maxOrderQty.value(), allocator);
	mainObject.AddMember("MaxPosition", quoteSettings.riskSettings.maxPosition.value(), allocator);

	// Add rate limit settings
	mainObject.AddMember("RateLimit", quoteSettings.rateLimitSettings.limit, allocator);
	mainObject.AddMember("RateLimitWindow", quoteSettings.rateLimitSettings.window, allocator);

	// Quote timeout settings
	mainObject.AddMember("QuoteTimeout", quoteSettings.quoteTimeout, allocator);
	mainObject.AddMember("ProrataQuoteTimeout", quoteSettings.prorataQuoteTimeout, allocator);

	// Serialize BID side settings

	// Bid side main settings
	mainObject.AddMember("BidProrataProfile", rapidjson::Value(bidSettings.prorataProfile.c_str(), allocator),
						 allocator);
	mainObject.AddMember("BidAltProrataProfile", rapidjson::Value(bidSettings.alternateProrataProfile.c_str(), allocator),
						allocator);
	mainObject.AddMember("BidDynamicProfile", rapidjson::Value(bidSettings.dynamicProfile.c_str(), allocator),
						 allocator);
	mainObject.AddMember("BidPayup", bidSettings.payup.value(), allocator);
	mainObject.AddMember("BidWinnerPayup", bidSettings.winnerPayup.value(), allocator);
	mainObject.AddMember("BidAddHedgeQty", bidSettings.addHedgeQty.value(), allocator);
	mainObject.AddMember("BidMaxPayupTicks", bidSettings.maxPayupTicks, allocator);
	mainObject.AddMember("BidTimeoutCover", bidSettings.timeoutCover, allocator);
	mainObject.AddMember("BidAltProrataProfileDelay", bidSettings.alternateProrataProfileDelay, allocator);
	mainObject.AddMember("BidProrataEnabled", bidSettings.prorataEnabled, allocator);
	mainObject.AddMember("BidDynamicEnabled", bidSettings.dynamicProfileEnabled, allocator);

	// Bid stack side settings - First stack (index 0)
	const auto &bidStack1 = bidSettings.stackSideSettings[0];
	mainObject.AddMember("BidQty", bidStack1.qty.value(), allocator);
	mainObject.AddMember("BidLength", bidStack1.length, allocator);
	mainObject.AddMember("BidLean", bidStack1.lean.value(), allocator);
	mainObject.AddMember("BidQuickAddLean", bidStack1.quickAddLean.value(), allocator);
	mainObject.AddMember("BidQuickAddTime", bidStack1.quickAddTime, allocator);
	mainObject.AddMember("BidManualAdj", bidStack1.manualAdj, allocator);
	mainObject.AddMember("BidAltManualAdj", bidStack1.altManualAdj, allocator);
	mainObject.AddMember("BidQuickAddEnabled", bidStack1.quickAddEnabled, allocator);
	mainObject.AddMember("BidNoAggressing", bidStack1.noAggressing, allocator);

	// Add price values
	setPriceParam(mainObject, "BidMaxPrice", bidStack1.insidePrice, allocator);
	setPriceParam(mainObject, "BidMinPrice", bidStack1.outsidePrice, allocator);
	setPriceParam(mainObject, "BidAltPrice", bidStack1.altManualAdjPrice, allocator);

	// Bid stack side settings - Second stack (index 1)
	const auto &bidStack2 = bidSettings.stackSideSettings[1];
	mainObject.AddMember("BidQty2", bidStack2.qty.value(), allocator);
	mainObject.AddMember("BidLength2", bidStack2.length, allocator);
	mainObject.AddMember("BidLean2", bidStack2.lean.value(), allocator);
	mainObject.AddMember("BidQuickAddLean2", bidStack2.quickAddLean.value(), allocator);
	mainObject.AddMember("BidQuickAddTime2", bidStack2.quickAddTime, allocator);
	mainObject.AddMember("BidManualAdj2", bidStack2.manualAdj, allocator);
	mainObject.AddMember("BidAltManualAdj2", bidStack2.altManualAdj, allocator);
	mainObject.AddMember("BidQuickAddEnabled2", bidStack2.quickAddEnabled, allocator);
	mainObject.AddMember("BidNoAggressing2", bidStack2.noAggressing, allocator);

	// Add price values for stack 2
	setPriceParam(mainObject, "BidMaxPrice2", bidStack2.insidePrice, allocator);
	setPriceParam(mainObject, "BidMinPrice2", bidStack2.outsidePrice, allocator);
	setPriceParam(mainObject, "BidAltPrice2", bidStack2.altManualAdjPrice, allocator);

	// Serialize ASK side settings

	// Ask side main settings
	mainObject.AddMember("AskProrataProfile", rapidjson::Value(askSettings.prorataProfile.c_str(), allocator),
						 allocator);
	mainObject.AddMember("AskAltProrataProfile", rapidjson::Value(askSettings.alternateProrataProfile.c_str(), allocator),
						allocator);
	mainObject.AddMember("AskDynamicProfile", rapidjson::Value(askSettings.dynamicProfile.c_str(), allocator),
						 allocator);
	mainObject.AddMember("AskPayup", askSettings.payup.value(), allocator);
	mainObject.AddMember("AskWinnerPayup", askSettings.winnerPayup.value(), allocator);
	mainObject.AddMember("AskAddHedgeQty", askSettings.addHedgeQty.value(), allocator);
	mainObject.AddMember("AskMaxPayupTicks", askSettings.maxPayupTicks, allocator);
	mainObject.AddMember("AskTimeoutCover", askSettings.timeoutCover, allocator);
	mainObject.AddMember("AskAltProrataProfileDelay", askSettings.alternateProrataProfileDelay, allocator);
	mainObject.AddMember("AskProrataEnabled", askSettings.prorataEnabled, allocator);
	mainObject.AddMember("AskDynamicEnabled", askSettings.dynamicProfileEnabled, allocator);

	// Ask stack side settings - First stack (index 0)
	const auto &askStack1 = askSettings.stackSideSettings[0];
	mainObject.AddMember("AskQty", askStack1.qty.value(), allocator);
	mainObject.AddMember("AskLength", askStack1.length, allocator);
	mainObject.AddMember("AskLean", askStack1.lean.value(), allocator);
	mainObject.AddMember("AskQuickAddLean", askStack1.quickAddLean.value(), allocator);
	mainObject.AddMember("AskQuickAddTime", askStack1.quickAddTime, allocator);
	mainObject.AddMember("AskManualAdj", askStack1.manualAdj, allocator);
	mainObject.AddMember("AskAltManualAdj", askStack1.altManualAdj, allocator);
	mainObject.AddMember("AskQuickAddEnabled", askStack1.quickAddEnabled, allocator);
	mainObject.AddMember("AskNoAggressing", askStack1.noAggressing, allocator);

	// Add price values
	setPriceParam(mainObject, "AskMaxPrice", askStack1.outsidePrice, allocator);
	setPriceParam(mainObject, "AskMinPrice", askStack1.insidePrice, allocator);
	setPriceParam(mainObject, "AskAltPrice", askStack1.altManualAdjPrice, allocator);

	// Ask stack side settings - Second stack (index 1)
	const auto &askStack2 = askSettings.stackSideSettings[1];
	mainObject.AddMember("AskQty2", askStack2.qty.value(), allocator);
	mainObject.AddMember("AskLength2", askStack2.length, allocator);
	mainObject.AddMember("AskLean2", askStack2.lean.value(), allocator);
	mainObject.AddMember("AskQuickAddLean2", askStack2.quickAddLean.value(), allocator);
	mainObject.AddMember("AskQuickAddTime2", askStack2.quickAddTime, allocator);
	mainObject.AddMember("AskManualAdj2", askStack2.manualAdj, allocator);
	mainObject.AddMember("AskAltManualAdj2", askStack2.altManualAdj, allocator);
	mainObject.AddMember("AskQuickAddEnabled2", askStack2.quickAddEnabled, allocator);
	mainObject.AddMember("AskNoAggressing2", askStack2.noAggressing, allocator);

	// Add price values for stack 2
	setPriceParam(mainObject, "AskMaxPrice2", askStack2.outsidePrice, allocator);
	setPriceParam(mainObject, "AskMinPrice2", askStack2.insidePrice, allocator);
	setPriceParam(mainObject, "AskAltPrice2", askStack2.altManualAdjPrice, allocator);

	// Serialize StopSettings

	// Stop quantities
	mainObject.AddMember("StopQuantity1", stopSettings.stopQuantities[0], allocator);
	mainObject.AddMember("StopQuantity2", stopSettings.stopQuantities[1], allocator);
	mainObject.AddMember("StopQuantity3", stopSettings.stopQuantities[2], allocator);
	mainObject.AddMember("StopQuantity4", stopSettings.stopQuantities[3], allocator);
	mainObject.AddMember("StopQuantity5", stopSettings.stopQuantities[4], allocator);

	// Stop triggers
	mainObject.AddMember("StopTrigger1", stopSettings.stopTriggers[0], allocator);
	mainObject.AddMember("StopTrigger2", stopSettings.stopTriggers[1], allocator);
	mainObject.AddMember("StopTrigger3", stopSettings.stopTriggers[2], allocator);
	mainObject.AddMember("StopTrigger4", stopSettings.stopTriggers[3], allocator);
	mainObject.AddMember("StopTrigger5", stopSettings.stopTriggers[4], allocator);

	// Stop lean
	mainObject.AddMember("StopLean1", stopSettings.stopLean[0], allocator);
	mainObject.AddMember("StopLean2", stopSettings.stopLean[1], allocator);
	mainObject.AddMember("StopLean3", stopSettings.stopLean[2], allocator);
	mainObject.AddMember("StopLean4", stopSettings.stopLean[3], allocator);
	mainObject.AddMember("StopLean5", stopSettings.stopLean[4], allocator);

	// Other stop settings
	mainObject.AddMember("MiddleClickQuantity", stopSettings.middleClickQuantity, allocator);
	mainObject.AddMember("MiddleClickTrigger", stopSettings.middleClickTrigger, allocator);
	mainObject.AddMember("StopQuantityMax", stopSettings.stopQuantityMax, allocator);
	mainObject.AddMember("FlipDelay", stopSettings.flipDelay, allocator);
	mainObject.AddMember("TriggerQuoteDelay", stopSettings.triggerQuoteDelay, allocator);
	mainObject.AddMember("DeleteOnFlipDefault", stopSettings.deleteOnFlipDefault, allocator);
	mainObject.AddMember("ShowLeanColumn", stopSettings.showLeanColumn, allocator);
	mainObject.AddMember("ShowTradeColumn", stopSettings.showTradeColumn, allocator);

	mainObject.AddMember("GroupName", rapidjson::Value(meta.userinfo.groupname.c_str(), allocator), allocator);
	mainObject.AddMember("Org", rapidjson::Value(meta.userinfo.org.c_str(), allocator), allocator);
	mainObject.AddMember("User", rapidjson::Value(meta.userinfo.user.c_str(), allocator), allocator);
	mainObject.AddMember("Enabled", meta.Enabled, allocator);
	mainObject.AddMember("UniqueId", rapidjson::Value(meta.UniqueId.c_str(), allocator), allocator);
	mainObject.AddMember("Strategy", rapidjson::Value(meta.Strategy.c_str(), allocator), allocator);
	mainObject.AddMember("TemplateName", rapidjson::Value(meta.TemplateName.c_str(), allocator), allocator);
	mainObject.AddMember("Tag", rapidjson::Value(meta.Tag.c_str(), allocator), allocator);

	// Serialize spreader pricing settings
	mainObject.AddMember("SpreaderBidEnabled", spreaderPricingSettings.bidEnabled[0], allocator);
	mainObject.AddMember("SpreaderBidEnabled2", spreaderPricingSettings.bidEnabled[1], allocator);
	mainObject.AddMember("SpreaderBidEnabledProrata", spreaderPricingSettings.bidEnabled[2], allocator);
	mainObject.AddMember("SpreaderAskEnabled", spreaderPricingSettings.askEnabled[0], allocator);
	mainObject.AddMember("SpreaderAskEnabled2", spreaderPricingSettings.askEnabled[1], allocator);
	mainObject.AddMember("SpreaderAskEnabledProrata", spreaderPricingSettings.askEnabled[2], allocator);
	mainObject.AddMember("SpreaderAddTicks", spreaderPricingSettings.addTicks[0], allocator);
	mainObject.AddMember("SpreaderAddTicks2", spreaderPricingSettings.addTicks[1], allocator);
	mainObject.AddMember("SpreaderAddTicksProrata", spreaderPricingSettings.addTicks[2], allocator);
	mainObject.AddMember("SpreaderRemoveTicks", spreaderPricingSettings.removeTicks[0], allocator);
	mainObject.AddMember("SpreaderRemoveTicks2", spreaderPricingSettings.removeTicks[1], allocator);
	mainObject.AddMember("SpreaderRemoveTicksProrata", spreaderPricingSettings.removeTicks[2], allocator);
	mainObject.AddMember("SpreaderLockQty", spreaderPricingSettings.lockQty[0].value(), allocator);
	mainObject.AddMember("SpreaderLockQty2", spreaderPricingSettings.lockQty[1].value(), allocator);
	mainObject.AddMember("SpreaderLockQtyProrata", spreaderPricingSettings.lockQty[2].value(), allocator);
	mainObject.AddMember("SpreaderResetQty", spreaderPricingSettings.resetQty.value(), allocator);
	mainObject.AddMember("SpreaderTickVariance", spreaderPricingSettings.tickVariance, allocator);

	// Add the legs array
	rapidjson::Value legsArray(rapidjson::kArrayType);
	for (const auto &leg : legs)
	{
		rapidjson::Value legObject(rapidjson::kObjectType);
		legObject.AddMember("LegId", leg.legId, allocator);
		legObject.AddMember("Exch", rapidjson::Value(leg.exchange.c_str(), allocator), allocator);
		legObject.AddMember("Symbol", rapidjson::Value(leg.symbol.c_str(), allocator), allocator);
		legObject.AddMember("ProdType", rapidjson::Value(leg.prodType.c_str(), allocator), allocator);
		legObject.AddMember("Inverse", leg.inverse, allocator);
		legObject.AddMember("PriceMult", leg.priceMult, allocator);
		legObject.AddMember("SpreadMult", leg.spreadMult, allocator);
		legObject.AddMember("ActiveQuoting", leg.activeQuoting, allocator);
		legsArray.PushBack(legObject, allocator);
	}
	mainObject.AddMember("Legs", legsArray, allocator);

	// Add Source field (separate from lowercase source)
	mainObject.AddMember("Source", rapidjson::Value(source.c_str(), allocator), allocator);

	// Add the mainObject to the document array
	document.PushBack(mainObject, allocator);

	// Convert document to string
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);

	return buffer.GetString();
};

} // namespace akl::stacker