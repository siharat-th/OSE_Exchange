//============================================================================
// Name        	: Mdp3Templates.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Oct 27, 2022 8:30:33 PM
//============================================================================


#ifndef SRC_CME_MDP3TEMPLATES_HPP_
#define SRC_CME_MDP3TEMPLATES_HPP_

#include <KT01/Core/Macro.hpp>

namespace KTN{
namespace Mdp3{

class Template
{
public:
	enum TemplateEnum
	{
		ChannelReset = 4,
		AdminHearbeat12 = 12,
		AdminLogin15 = 15,
		AdminLogout16 = 16,
		SecurityStatus30 = 30,
		MDIncrementalRefreshVolume37 = 37,
		QuoteRequest39 = 39,
		MDIncrementalRefreshBook46 = 46,
		MDIncrementalRefreshOrderBook47 = 47,
		MDIncrementalRefreshTradeSummary48 = 48,
		MDIncrementalRefreshDailyStatistics49 = 49,
		MDIncrementalRefreshLimitsBanding50 = 50,
		MDIncrementalRefreshSessionStatistics51 = 51,
		SnapshotFullRefresh52 = 52,
		SnapshotFullRefreshOrderBook53 = 53,
		MDInstrumentDefinitionFuture54 = 54,
		MDInstrumentDefinitionOption55 = 55,
		MDInstrumentDefinitionSpread56 = 56,
		MDInstrumentDefinitionFixedIncome57 = 57,
		MDInstrumentDefinitionRepo58 = 58,
		SnapshotRefreshTopOrders59 = 59,
		SecurityStatusWorkup60 = 60,
		SnapshotFullRefreshTCP61 = 61,
		CollateralMarketValue62 = 62,
		MDInstrumentDefinitionFX63 = 63,
		MDIncrementalRefreshBookLongQty64 = 64,
		MDIncrementalRefreshTradeSummaryLongQty65 = 65,
		MDIncrementalRefreshVolumeLongQty66 = 66,
		MDIncrementalRefreshSessionStatisticsLongQty67 = 67,
		SnapshotFullRefreshTCPLongQty68 = 68,
		SnapshotFullRefreshLongQty69 = 69,
		MD_BBO_ONLY=999
	};




	static TemplateEnum toEnum(int id)
	{
		TemplateEnum val = static_cast<TemplateEnum>(id);
		return val;
	}

	static std::string toString(TemplateEnum msgid)
	{
		std::string res = STRINGIFY(msgid);
		return res;
	}

	// Commenting this out because it's broken and throwing a compiler warning.
	// This will return the string "msgid" rather than enum as a string
	//
	// static std::string toString(int id)
	// {
	// 	TemplateEnum msgid = toEnum(id);
	// 	std::string res = STRINGIFY(msgid);
	// 	return res;
	// }

};


}
}




#endif /* SRC_CME_MDP3TEMPLATES_HPP_ */
