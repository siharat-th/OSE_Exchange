/*
 * SpreadDefinition.cc
 *
 *  Created on: May 21, 2010
 *      Author: val
 */

#include "SpreadDefinition.hh"
#include <algorithm>
#include "util.hh"
#include "common_util.h"

namespace LimeBrokerage {

//SpreadDefinition implementation

SpreadDefinition::SpreadDefinition(std::string underlying)
	: underlyingName(underlying),
	ratioMultiplier(1),
	strikePriceExponent(-4),
	normalized(false)
{

}

SpreadDefinition::SpreadDefinition(spread_def_data_t& spread_raw, std::string quote_source_id, std::string spread_name, bool networkOrder)
	: underlyingName(spread_raw.underlying_name), spreadName(spread_name), ratioMultiplier(0),
	strikePriceExponent(0), normalized(true), quoteSourceId(quote_source_id)
{
	uint8_t num_legs = 0, pad = 0;
	if (networkOrder) spread_raw.spread_common_data = ntohs(spread_raw.spread_common_data);
	uint8_t exponent = 0;
	uint8_t exponent_sign = 0;
	spread_data_decode(&spread_raw.spread_common_data, &pad, &num_legs, &ratioMultiplier, &exponent_sign, &exponent);
	strikePriceExponent = exponent * (exponent_sign == BIT_FIELD_POSITIVE_SIGN ? 1 : -1);
	for (int i = 0; i < num_legs; i++) {
		if (networkOrder) spread_raw.legs[i] = lime_ntohll(spread_raw.legs[i]);
		SpreadLegDefinition leg(spread_raw.legs[i], this);
		addLeg(leg);
	}
}

SpreadDefinition::SpreadDefinition(const SpreadDefinition& spread)
	: underlyingName(spread.underlyingName), ratioMultiplier(spread.ratioMultiplier),
	strikePriceExponent(spread.strikePriceExponent), legs(spread.legs), normalized(false)
{

}

void SpreadDefinition::addLeg(SpreadLegType type,
			uint16_t exp_year,
			uint8_t exp_month,
			uint8_t exp_day,
			int32_t strike_price_mantissa,
			int8_t strike_price_exponent,
			SpreadLegSide leg_side,
			uint16_t leg_ratio)
{
	if (type == SPREAD_LEG_TYPE_EQUITY) {
		strike_price_mantissa = 0;
		strike_price_exponent = 0;
		exp_year = BASE_YEAR;
		exp_month = 0;
		exp_day = 0;
	}
	SpreadLegDefinition leg = SpreadLegDefinition(type,
			exp_year,
			exp_month,
			exp_day,
			strike_price_mantissa,
			strike_price_exponent,
			leg_side,
			leg_ratio,
			this);
	legs.push_back(leg);
}

void SpreadDefinition::addLeg(SpreadLegDefinition& leg)
{
	legs.push_back(leg);
}

SpreadDefinition& SpreadDefinition::operator=(const SpreadDefinition& spread)
{
	if (this != &spread) {
		underlyingName = spread.underlyingName;
		ratioMultiplier = spread.ratioMultiplier;
		strikePriceExponent = spread.strikePriceExponent;
		legs = spread.legs;
		normalized = spread.normalized;
	}
	return *this;
}

bool SpreadDefinition::operator==(SpreadDefinition& spread)
{
	normalize();
	spread.normalize();
	bool res = (underlyingName == spread.underlyingName &&
				strikePriceExponent == spread.strikePriceExponent);
	if (res == false ) return res;
	res = (legs == spread.legs);
	return res;
}
std::string SpreadDefinition::getUnderlying() const
{
	return underlyingName;
}

std::string SpreadDefinition::getSpreadName() const
{
	return spreadName;
}

uint8_t SpreadDefinition::getNumberOfLegs() const
{
	return legs.size();
}
uint8_t SpreadDefinition::getRatioMultiplier()
{
	return ratioMultiplier;
}
int8_t SpreadDefinition::getStrikePriceExponent()
{
	return strikePriceExponent;
}
SpreadDefinition::~SpreadDefinition()
{

}

static limeq_price prices[MAX_LEGS];
static uint16_t ratios[MAX_LEGS];
/*
 * This method normalizing the spread as following:
 * - it figure out greater common divider for the ratios of all legs and divide every ratio by it
 * - it figure out minimum exponent for every price and normalize it
 * - it also sorts all the legs in descending order
 * This method is called before accessing legs, ratioGCD or strikePriceExponent
 * It is also called before comparison
 */
void SpreadDefinition::normalize()
{
	if (normalized) return;
	int8_t min_exponent = 127;
	bool exponent_only = false;

	for (unsigned int i = 0; i < legs.size(); i++) {
		prices[i].mantissa = legs[i].getStrikePriceMantissa();
		prices[i].exponent = legs[i].getStrikePriceExponent();
		ratios[i] = legs[i].getRatio();
		if (ratios[i] == 1) exponent_only = true;
		normalize_mantissa(&prices[i], &min_exponent);
		min_exponent = min_exponent < prices[i].exponent ? min_exponent : prices[i].exponent;
	}
	uint16_t ratio_multiplier = 1;
	if (!exponent_only) {
		ratio_multiplier = greater_common_divider(ratios, sizeof(uint16_t), 0, legs.size());
	}
	if (!normalize_exponent(prices, legs.size(), &min_exponent)) {
		std::cout  << "Unable to normalize prices\n";
		min_exponent = strikePriceExponent;
	}
	if (ratio_multiplier == ratioMultiplier && strikePriceExponent == min_exponent) return;
	ratioMultiplier = ratio_multiplier;
	strikePriceExponent = min_exponent;
	int i = 0;
	for (LegSet::iterator it = legs.begin(); it != legs.end(); it++, i++) {
		uint16_t leg_ratio = it->getRatio() / ratioMultiplier;
		it->setStrikePriceExponent(min_exponent);
		it->setStrikePriceMantissa(prices[i].mantissa);
		it->setRatio(leg_ratio);
		it->reComputeRawLegData();
	}
	std::sort(legs.begin(), legs.end());
	normalized = true;


}

//SpreadLegDefinition implementation

SpreadLegDefinition::SpreadLegDefinition(SpreadLegType type,
		uint16_t exp_year,
		uint8_t exp_month,
		uint8_t exp_day,
		int32_t strike_price_mantissa,
		int8_t exp,
		SpreadLegSide leg_side,
		uint16_t leg_ratio,
		SpreadDefinition* spread_definition)
	: legType(type), expirationYear(exp_year), expirationMonth(exp_month), expirationDay(exp_day),
	strikePriceMantissa(strike_price_mantissa), exponent(exp), side(leg_side), ratio(leg_ratio),
	spreadDefinition(spread_definition), dirty(true)
{
	reComputeRawLegData();
}

SpreadLegDefinition::SpreadLegDefinition(const SpreadLegDefinition& leg)
{
	if (&leg != this) {
		legType = leg.legType;
		expirationYear = leg.expirationYear;
		expirationMonth = leg.expirationMonth;
		expirationDay = leg.expirationDay;
		strikePriceMantissa = leg.strikePriceMantissa;
		exponent = leg.exponent;
		side = leg.side;
		ratio = leg.ratio;
		rawLegData = leg.rawLegData;
		spreadDefinition = leg.spreadDefinition;
		dirty = leg.dirty;
	}
}

SpreadLegDefinition::SpreadLegDefinition(leg_def_data_t& leg_raw, SpreadDefinition* spread)
	: exponent(spread->getStrikePriceExponent()), spreadDefinition(spread),
	rawLegData(leg_raw), dirty(false)
{
	uint8_t pad = 0, leg_type = 255, leg_side = 255;
	uint8_t mantissa_sign = BIT_FIELD_POSITIVE_SIGN;
	uint32_t mantissa = 0;
	uint8_t expiration_year= 0;
	leg_def_data_decode(&leg_raw, &pad, &leg_type, &expiration_year, &expirationMonth,
			&expirationDay, &mantissa_sign, &mantissa, &leg_side, &ratio);
	expirationYear = BASE_YEAR + expiration_year;
	strikePriceMantissa = mantissa * (mantissa_sign == BIT_FIELD_POSITIVE_SIGN ? 1 : -1);
	legType = static_cast<SpreadLegType>(leg_type);
	side = static_cast<SpreadLegSide>(leg_side);
}

SpreadLegDefinition& SpreadLegDefinition::operator=(const SpreadLegDefinition& leg)
{
	if (&leg != this) {
		legType = leg.legType;
		expirationYear = leg.expirationYear;
		expirationMonth = leg.expirationMonth;
		expirationDay = leg.expirationDay;
		strikePriceMantissa = leg.strikePriceMantissa;
		exponent = leg.exponent;
		side = leg.side;
		ratio = leg.ratio;
		rawLegData = leg.rawLegData;
		spreadDefinition = leg.spreadDefinition;
		dirty = leg.dirty;
	}
	return *this;
}

bool SpreadLegDefinition::operator==(const SpreadLegDefinition& leg) const
{
	return (rawLegData == leg.rawLegData);
}

bool SpreadLegDefinition::operator<(const SpreadLegDefinition& leg) const
{
	return rawLegData < leg.rawLegData;
}
SpreadLegType SpreadLegDefinition::getLegType() const
{
	return legType;
}

uint16_t SpreadLegDefinition::getExpirationYear() const
{
	return expirationYear;
}
uint8_t SpreadLegDefinition::getExpirationMonth() const
{
	return expirationMonth;
}
uint8_t SpreadLegDefinition::getExpirationDay() const
{
	return expirationDay;
}
uint32_t SpreadLegDefinition::getStrikePriceMantissa() const
{
	return strikePriceMantissa;
}

void SpreadLegDefinition::setStrikePriceMantissa(uint32_t mantissa)
{
	strikePriceMantissa = mantissa;
	dirty = true;
}
int8_t SpreadLegDefinition::getStrikePriceExponent() const
{
	return exponent;
}
void SpreadLegDefinition::setStrikePriceExponent(int8_t leg_exponent)
{
	exponent = leg_exponent;
	dirty = true;
}
SpreadLegSide SpreadLegDefinition::getSide() const
{
	return side;
}
uint16_t SpreadLegDefinition::getRatio() const
{
	return ratio;
}
void SpreadLegDefinition::setRatio(uint16_t leg_ratio)
{
	ratio = leg_ratio;
	dirty = true;
}
const SpreadDefinition* SpreadLegDefinition::getSpreadDefinition() const
{
	return spreadDefinition;
}
void SpreadLegDefinition::reComputeRawLegData()
{
	if (!dirty) return;
	uint8_t t = static_cast<uint8_t>(legType);
	uint8_t s = static_cast<uint8_t>(side);
	rawLegData = 0;
	uint8_t mantissa_sign = BIT_FIELD_POSITIVE_SIGN, pad = 0;
	uint32_t mantissa = strikePriceMantissa;
	if (strikePriceMantissa < 0) {
		mantissa_sign = BIT_FIELD_NEGATIVE_SIGN;
		mantissa = strikePriceMantissa * -1;
	}
	uint8_t expiration_year = expirationYear - BASE_YEAR;
	leg_def_data_encode(&rawLegData, &pad, &t, &expiration_year, &expirationMonth, &expirationDay,
			&mantissa_sign, &mantissa, &s, &ratio);
}

std::ostream& operator<<(std::ostream& s, const SpreadLegSide& side)
{
	switch(side) {
	case SPREAD_SIDE_BUY:
		s << "BUY";
		break;
	case SPREAD_SIDE_SELL:
		s << "SELL";
		break;
	case SPREAD_SIDE_SELL_SHORT:
		s << "SELL SHORT";
		break;
	default:
		s << "UNKNOWN";
	}
	return s;
}
std::ostream& operator<<(std::ostream& s, const SpreadLegType& type)
{
	switch (type) {
	case SPREAD_LEG_TYPE_PUT:
		s << "PUT";
		break;
	case SPREAD_LEG_TYPE_CALL:
		s << "CALL";
		break;
	case SPREAD_LEG_TYPE_EQUITY:
		s << "EQUITY";
		break;
	default:
		s << "UNKNOWN";
	}
	return s;
}
LegSet& SpreadDefinition::getLegs()
{
	return legs;
}

std::ostream&
operator<<(std::ostream& s, SpreadLegDefinition& spread)
{

	s << spread.legType << " " <<
		(int)spread.expirationMonth << "/"
		<< (int)spread.expirationDay << "/"
		<< spread.expirationYear
	  << " " << spread.strikePriceMantissa
	  << "E" << (int)spread.spreadDefinition->getStrikePriceExponent() << " "
	  << spread.side << " " << spread.ratio;
	return s;
}
std::ostream&
operator<<(std::ostream& s, SpreadDefinition& spread)
{
	s << "Spread '" << spread.getSpreadName() << "', ratio gcd = " << (int)spread.getRatioMultiplier() <<
	", common exponent = " << (int)spread.getStrikePriceExponent() << ", total legs = " << spread.legs.size() << ": " << std::endl;
	for (unsigned int jj = 0; jj < spread.legs.size(); jj++) {
		s << "[" << jj << "]: " << spread.legs[jj] << std::endl;
	}
	return s;
}

}//namespace LimeBrokerage
