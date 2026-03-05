#pragma once

#include "Instrument.hpp"

namespace akl::stacker
{

template<KTN::ORD::KOrderSide::Enum SIDE>
std::optional<Price> Instrument::GetBestQuotePrice() const
{
	if (likely(book.HasSide<SIDE>()))
	{
		if (book.BestQuantity<SIDE>() < pc.GetPayup<SIDE>())
		{
			return SideTraits<SIDE>::PriceOut(book.BestPrice<SIDE>(), TickIncrement);
		}

		constexpr KTN::ORD::KOrderSide::Enum OPP = SideTraits<SIDE>::OPPOSITE;
		const StopInfo *stopInfo = sh.GetStop<OPP>(book.BestPrice<SIDE>());
		if (stopInfo != nullptr)
		{
			if (book.BestQuantity<SIDE>() < stopInfo->triggerQuantity || book.BestQuantity<SIDE>() < stopInfo->lean)
			{
				return SideTraits<SIDE>::PriceOut(book.BestPrice<SIDE>(), TickIncrement);
			}
		}

		const std::optional<Price> lastTriggeredStopPx = sh.LastTriggeredStop<OPP>();
		if (lastTriggeredStopPx)
		{
			return  SideTraits<SIDE>::PriceOut(*lastTriggeredStopPx, TickIncrement);
		}

		return book.BestPrice<SIDE>();
	}
	else
	{
		return std::nullopt;
	}
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void Instrument::OnProrataProfileUpdate(const ProrataProfileUpdateMessage &msg)
{
	//ProrataProfile profile = ProrataProfile::FromMessage(msg);
	prorataPricingModel.SetSideProfile<SIDE>(msg);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void Instrument::ClearProrataProfile()
{
	prorataPricingModel.ClearSideProfile<SIDE>();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void Instrument::OnAltProrataProfileUpdate(const ProrataProfileUpdateMessage &msg)
{
	prorataPricingModel.SetAlternateSideProfile<SIDE>(msg);
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void Instrument::ClearAltProrataProfile()
{
	prorataPricingModel.ClearAlternateSideProfile<SIDE>();
}

template<KTN::ORD::KOrderSide::Enum SIDE>
void Instrument::DisableAdditionalHedge()
{
	disableAdditionalHedge[SideTraits<SIDE>::INDEX] = true;
}

}