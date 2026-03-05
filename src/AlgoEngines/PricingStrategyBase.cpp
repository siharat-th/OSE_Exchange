//============================================================================
// Name        	: PricingStrategyBase.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 19, 2023 10:13:45 AM
//============================================================================

#include <AlgoEngines/PricingStrategyBase.hpp>

using namespace KTN::ALGO::PRICING;

PricingStrategyBase::PricingStrategyBase() {
}

PricingStrategyBase::~PricingStrategyBase() {
}

void PricingStrategyBase::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void PricingStrategyBase::Load(instrument* legs, int len)
{
	for (int i = 0; i < len; i++){
		_legs[i] = legs[i];
	}
	_numLegs = len;
}
