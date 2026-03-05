#include "PayupCalculator.hpp"
#include "Order.hpp"

namespace akl::stacker
{

PayupCalculator::PayupCalculator(const Book &book, const QuoteInstrumentSettings &settings, const PricingModel &spm, /*const DynamicPricingModel &dpm,*/ const PositionManager &pm)
: book(book)
, settings(settings)
, spm(spm)
//, dpm(dpm)
, pm(pm)
{

}

void PayupCalculator::Recalculate()
{
	const Quantity pos = pm.Position();
	if (pos > Quantity(0))
	{
		if (!bidMap.empty())
		{
			removeLockedLevels<KTN::ORD::KOrderSide::Enum::BUY>();
			removeCrossingLevels<KTN::ORD::KOrderSide::Enum::BUY>();
		}
		payupQty[0] = calculatePayup<KTN::ORD::KOrderSide::Enum::BUY>();
		payupQty[1] = Quantity(0);
		if (unlikely(!askMap.empty()))
		{
			askMap.clear();
		}
	}
	else if (pos < Quantity(0))
	{
		if (!askMap.empty())
		{
			removeLockedLevels<KTN::ORD::KOrderSide::Enum::SELL>();
			removeCrossingLevels<KTN::ORD::KOrderSide::Enum::SELL>();
		}
		payupQty[0] = Quantity(0);
		payupQty[1] = calculatePayup<KTN::ORD::KOrderSide::Enum::SELL>();
		if (unlikely(!bidMap.empty()))
		{
			bidMap.clear();
		}
	}
	else
	{
		payupQty[0] = Quantity(0);
		payupQty[1] = Quantity(0);
		if (unlikely(!bidMap.empty()))
		{
			bidMap.clear();
		}
		if (unlikely(!askMap.empty()))
		{
			askMap.clear();
		}
	}
}

void PayupCalculator::OnFill(const Order *o, const Price fillPx)
{
	if (o->dynamicIndex >= 0)
	{
		switch (o->side)
		{
			case KTN::ORD::KOrderSide::Enum::BUY:
			{
				int8_t &index = bidMap[fillPx];
				index = std::max(index, o->dynamicIndex);
				break;
			}
			case KTN::ORD::KOrderSide::Enum::SELL:
			{
				int8_t &index = askMap[fillPx];
				index = std::max(index, o->dynamicIndex);
				break;
			}
			default:
				break;
		}
	}
}

void PayupCalculator::Clear()
{
	bidMap.clear();
	askMap.clear();
}

template<>
PayupCalculator::MapType<KTN::ORD::KOrderSide::Enum::BUY> &PayupCalculator::getMap<KTN::ORD::KOrderSide::Enum::BUY>()
{
	return bidMap;
}

template<>
PayupCalculator::MapType<KTN::ORD::KOrderSide::Enum::SELL> &PayupCalculator::getMap<KTN::ORD::KOrderSide::Enum::SELL>()
{
	return askMap;
}

template<>
const PayupCalculator::MapType<KTN::ORD::KOrderSide::Enum::BUY> &PayupCalculator::getMap<KTN::ORD::KOrderSide::Enum::BUY>() const
{
	return bidMap;
}

template<>
const PayupCalculator::MapType<KTN::ORD::KOrderSide::Enum::SELL> &PayupCalculator::getMap<KTN::ORD::KOrderSide::Enum::SELL>() const
{
	return askMap;
}

}