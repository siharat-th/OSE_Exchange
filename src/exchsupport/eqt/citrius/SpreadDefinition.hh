/*
 * SpreadDefinition.hh
 *
 *  Created on: May 21, 2010
 *      Author: val
 */

#ifndef SPREADDEFINITION_HH_
#define SPREADDEFINITION_HH_
#include <string>
#include <vector>
#include <iostream>
#include "limeq_def.h"

namespace LimeBrokerage {

/**
 * Define type of the leg
 */
typedef enum leg_type SpreadLegType;

/**
 * Define side of the leg
 */
typedef enum leg_side SpreadLegSide;

class SpreadDefinition;

/**
 * A definition of an individual spread leg.
 *
 * A spread leg contains all information necessary to identify an option,
 * as well as the side, type, and ratio of the leg.
 *
 * SpreadLegDefinition is part of a SpreadDefinition.
 */
class SpreadLegDefinition {
    friend class SpreadDefinition;

public:

    /**
     * Constructor to create a spread leg from a set of option parameters.
     *
     * @param type
     *  Type of the leg defined by 'enum leg_type' in limeq_def.h
     *  with the following values: SPREAD_LEG_TYPE_PUT, SPREAD_LEG_TYPE_CALL, SPREAD_LEG_TYPE_EQUITY
     * @param expiration_year
     *  Expiration Year YYYY
     * @param expiration_month
     *  Expiration Month MM
     * @param expiration_day
     *  Expiration Day DD
     * @param strike_price_mantissa
     *  Mantissa of the strike price
     * @param strike_price_exponent
     *  Exponent of the strike price
     * @param leg_side
     *  'enum leg_side' defined in limeq_def.h
     *  with the following values: SPREAD_SIDE_BUY, SPREAD_SIDE_SELL, SPREAD_SID_SELL_SHORT
     * @param leg_ratio
     *  Ratio of this leg compared to the rest of the spread.
     *  This value has to be greater than or equal to 1.
     *  During the spread normalization process this value can be reduced by
     *  greatest common divisor of all spread legs.
     * @param spread_definition
     *  Spread definition for which this leg is defined.
     */
    SpreadLegDefinition(SpreadLegType type,
            uint16_t expiration_year,
            uint8_t expiration_month,
            uint8_t expiration_day,
            int32_t strike_price_mantissa,
            int8_t strike_price_exponent,
            SpreadLegSide leg_side,
            uint16_t leg_ratio,
            SpreadDefinition* spread_definition);

    /**
     * Copy constructor.
     */
    SpreadLegDefinition(const SpreadLegDefinition&);

    /**
     * Construct leg definition from the raw data sent by the Citrius server.
     */
    SpreadLegDefinition(leg_def_data_t& leg_raw, SpreadDefinition*);

    /**
     * Assignment operator.
     */
    SpreadLegDefinition& operator=(const SpreadLegDefinition&);

    /**
     * Equality operator.
     * Each leg is compared in following order:
     * leg type, expiration year,
     * expiration month, expiration day,
     * strike price mantissa, side.
     * Before comparing, legs are normalized:
     * 1) strike price is normalized to smallest exponent among all the legs.
     * 2) ratio is reduced by greater common divisor among all the legs.
     * 3) all legs are sorted in descended order.
     */
    bool operator==(const SpreadLegDefinition&) const;

    /**
     * Less than operator.
     * The comparison is the same as the comparison with equality operator.
     */
    bool operator<(const SpreadLegDefinition&) const;

    /*
     * Accessor methods.
     */

    /**
     * Returns the SpreadLegType. The type of the leg is defined by 'enum leg_type' in limeq_def.h
     * with the following values: SPREAD_LEG_TYPE_PUT, SPREAD_LEG_TYPE_CALL, SPREAD_LEG_TYPE_EQUITY
     */
    SpreadLegType getLegType() const;

    /**
     * Returns 4 digit expiration year for this leg.
     */
    uint16_t getExpirationYear() const;

    /**
     * Returns 2 digit expiration month for this leg.
     */
    uint8_t getExpirationMonth() const;

    /**
     * Returns 2 digit expiration day for this leg.
     */
    uint8_t    getExpirationDay() const;

    /**
     * Returns the strike price mantissa for this leg.
     */
    uint32_t getStrikePriceMantissa() const;

    /**
     * Set the strike price mantissa for this leg.
     */
    void setStrikePriceMantissa(uint32_t);

    /**
     * Returns the strike price exponent for this leg.
     */
    int8_t getStrikePriceExponent() const;

    /**
     * Set the strike price exponent for this leg.
     */
    void setStrikePriceExponent(int8_t);

    /**
     * Return the SpreadLegSide for this leg.  The side of the leg is defined by 'enum leg_side' in limeq_def.h
     * with the following values: SPREAD_SIDE_BUY, SPREAD_SIDE_SELL, SPREAD_SID_SELL_SHORT
     */
    SpreadLegSide getSide() const;

    /**
     * Returns the ratio for this leg.
     */
    uint16_t getRatio() const;

    /**
     * Set the ratio for this leg.
     */
    void setRatio(uint16_t);

    /**
     * Return a pointer to the SpreadDefinition associated with this leg.
     */
    const SpreadDefinition* getSpreadDefinition() const;


    friend std::ostream& operator<<(std::ostream& s, SpreadLegDefinition& b);

private:
    SpreadLegType legType;
    uint16_t expirationYear;
    uint8_t expirationMonth;
    uint8_t expirationDay;
    int32_t strikePriceMantissa;
    int8_t exponent;
    SpreadLegSide side;
    uint16_t ratio;
    SpreadDefinition* spreadDefinition;
    leg_def_data_t rawLegData;
    bool dirty;
    void reComputeRawLegData();
};
typedef std::vector<SpreadLegDefinition> LegSet;

/**
 * A spread definition.  Spreads are financial instruments that group multiple securities into a single instrument.
 * Spreads usually combine a set of derivatives with different side, expiration date, or strike price.  They may contain an equity leg.
 */
class SpreadDefinition {
public:
    /**
     * Spread constructor requiring only the underlying.
     * The rest of the arguments are set by user.
     * Spread is normalized during comparison.
     */
    SpreadDefinition (std::string underlying);

    /**
     * Copy constructor.
     */
    SpreadDefinition(const SpreadDefinition&);

    /**
     * Constructs a spread from the raw data off the wire.
     */
    SpreadDefinition(spread_def_data_t& spread_raw, std::string quoteSourceId, std::string spreadName, bool networkOrder = true);

    virtual ~SpreadDefinition();

    /**
     * Assignment operator.
     */
    SpreadDefinition& operator=(const SpreadDefinition&);

    /**
     * Equality operator.
     * Compare underlying name and then each leg.
     * This method normalizes both spreads before comparison.
     */
    bool operator==(SpreadDefinition&);

    /**
     * Create and add a leg to a spread definition.
     * @param type
     * @param exp_year
     * @param exp_month
     * @param exp_day
     * @param strike_price_mantissa
     * @param strike_price_exponent
     * @param leg_side
     * @param leg_ratio
     */
    void addLeg(SpreadLegType type,
            uint16_t exp_year,
            uint8_t exp_month,
            uint8_t exp_day,
            int32_t strike_price_mantissa,
            int8_t strike_price_exponent,
            SpreadLegSide leg_side,
            uint16_t leg_ratio);

    /**
     * Add an existing leg to a spread definition.
     * @param leg
     */
    void addLeg(SpreadLegDefinition& leg);

    /*
     * Accessor methods.
     */

    /**
     * Returns set of SpreadLegDefinitions corresponding to this spread.  Lets are sorted in ascending order to simplify comparison.
     */
    LegSet& getLegs();

    /**
     * Returns the name of the underlying security for this spread.
     */
    std::string getUnderlying() const;

    /**
     * Returns the unique spread name that identifies this spread.
     */
    std::string getSpreadName() const;

    /**
     * Set the spreadName field for this SpreadDefinition object.
     */
    void setSpreadName(std::string spread_name) { spreadName = spread_name; }

    /**
     * Returns the number of SpreadLegDifinitions in this spread.
     */
    uint8_t getNumberOfLegs() const;

    /**
     * Returns the ratio multiplier for the leg ratios in this spread. To facilitate the comparison of spreads, the Citrius Quote Server normalizes all spread legs by dividing their ratios by the GCD.  To see the actual ratio of a leg, you must multiply it by the ratioMultiplier.
     */
    uint8_t getRatioMultiplier();

    /**
     * Returns the strike price exponent for this spread.
     */
    int8_t getStrikePriceExponent();

    /**
     * Returns the quote source ID that this spread definition originated from.
     */
    std::string getQuoteSourceId() const;

    /**
     * Returns the SpreadLegDefinition for a specific leg number.
     */
    SpreadLegDefinition& getSpreadLegDefinition(int leg) const;

    friend std::ostream& operator<<(std::ostream& s, SpreadDefinition& b);
private:

    std::string underlyingName;
    std::string spreadName;
    uint8_t ratioMultiplier;
    int8_t strikePriceExponent;
    LegSet legs;
    bool normalized;
    std::string quoteSourceId;
    SpreadDefinition() {}
    void normalize();
};
std::ostream& operator<<(std::ostream& s, const SpreadLegSide& b);
std::ostream& operator<<(std::ostream& s, const SpreadLegType& b);
}
#endif /* SPREADDEFINITION_HH_ */
