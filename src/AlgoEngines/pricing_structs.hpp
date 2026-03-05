//============================================================================
// Name        	: pricing_structs.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 19, 2023 2:11:17 PM
//============================================================================

#ifndef SRC_ALGOENGINES_PRICING_STRUCTS_HPP_
#define SRC_ALGOENGINES_PRICING_STRUCTS_HPP_

#include <AlgoSupport/algo_structs.hpp>

namespace KTN {
	namespace ALGO {
		namespace PRICING {

		struct HedgeType
		{
		    enum Enum : int8_t
		    {
		    	SIMPLE_SINGLE = 1,
		    	SIMPLE_RATIO,
				COMPLEX
		    };

		    static std::string toString(Enum status)
		    {
		        switch(status) {
		        	case SIMPLE_SINGLE:
		        		return "SIMPLE_SINGLE";
		            case SIMPLE_RATIO:
		                return "SIMPLE_RATIO";
		            case COMPLEX:
		                return "COMPLEX";
		            // case KOrderStatus::UNKNOWN:
		            //     return "UNKNOWN";
		            default:
		                return "INVALID";
		        }
		    }
		};


		struct SpreadHedgeV3
		{
			//KTN::OrderPod ord;
			float ratio;
			float spreadmult;

			char ordernum[20];

			uint64_t reqid;
			uint32_t qty;
			int edge;
			akl::Price price;
			akl::Price primarypx;
			int32_t secid;
			uint8_t instindex;
			int8_t iside;
			uint8_t tickprecision;
			uint8_t mktsegid;
			uint8_t connindex;
			uint8_t cbid;
			uint8_t index;

			HedgeType::Enum hdgtype;
			KOrderSide::Enum OrdSide;

			bool sqz;
		};

		struct OrderWithHedge
		{
			KTN::OrderPod ord;
			vector<SpreadHedgeV3> hdgs;
		};


		struct LittleImpSide
		{
			vector<SpreadHedgeV3> hdgPath;
			akl::Price price;

			uint8_t index;
			uint8_t instindex;
			KOrderSide::Enum OrderSide;
			bool isBid;

			void reset()
			{
				index = -1;
				price = 0;
			//	edge = 0;
				instindex = -1;
				hdgPath.clear();
			}
		};


}}}

#endif /* SRC_ALGOENGINES_PRICING_STRUCTS_HPP_ */
