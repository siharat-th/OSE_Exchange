#pragma once

#include <AlgoParams/AlgoInitializer.hpp>
#include <Algos/AlgoBaseV3.hpp>
#include <akl/Quantity.hpp>
#include <akl/Price.hpp>

#include <Algos/AklStacker/Time.hpp>
#include <Algos/AklStacker/Helpers.hpp>
#include <Algos/AklStacker/messages/AlgoMessage.hpp>
#include <Algos/AklStacker/messages/StrategyStatusMessage.hpp>

namespace akl::stacker
{
	class Stack;
	class MStack;
}

namespace akl::test
{

struct QtyPricePair
{
	int qty { 0 };
	double px { 0 };
};

template<typename STRAT, typename = std::enable_if_t<std::is_base_of_v<KTN::AlgoBaseV3, STRAT>>>
class StrategyTester : public STRAT
{
public:
	static constexpr uint32_t secId = 10539;

	StrategyTester(AlgoInitializer params)
	: STRAT(params)
	{

	}

	void InjectBbo(int index, Quantity bidQty, Price bidPx, Quantity askQty, Price askPx)
	{
		this->_mdh->_algomds[index].bid = bidPx;
		this->_mdh->_algomds[index].ask = askPx;
		this->_mdh->_algomds[index].bidsize = bidQty.value();
		this->_mdh->_algomds[index].asksize = askQty.value();
		this->_mdh->_algomds[index].secid = secId;
		this->onMktDataBidAsk(this->_mdh->_algomds[index]);
	}

	void InjectMarketState(
		const KT01::DataStructures::MarketDepth::BookSecurityStatus secStatus,
		const KT01::DataStructures::MarketDepth::BookSecurityTradingEvent event = KT01::DataStructures::MarketDepth::BookSecurityTradingEvent::NoEvent)
	{
		this->onMktDataSecurityStatus(this->_mdh->Obm.books[0].SecId, secStatus, event, 0);
	}

	void InjectMarketState(
		int index,
		const KT01::DataStructures::MarketDepth::BookSecurityStatus secStatus,
		const KT01::DataStructures::MarketDepth::BookSecurityTradingEvent event = KT01::DataStructures::MarketDepth::BookSecurityTradingEvent::NoEvent)
	{
		const KT01::DataStructures::MarketDepth::DepthBook &book = this->_mdh->Obm.books[index];
		this->onMktDataSecurityStatus(book.SecId, secStatus, event, 0);
	}

	void InjectDepth(
		const std::vector<QtyPricePair> &bids,
		const std::vector<QtyPricePair> &asks,
		const KT01::DataStructures::MarketDepth::BookSecurityStatus secStatus = KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade)
	{
		static constexpr size_t MAX_DEPTH = 10;

		KT01::DataStructures::MarketDepth::DepthBook &book = this->_mdh->Obm.books[0];

		size_t i = 0;
		for (; i < bids.size() && i < MAX_DEPTH; ++i)
		{
			if (bids[i].qty != 0)
			{
				book.Bids[i].act = KT01::DataStructures::MarketDepth::BookAction::Change;
				book.Bids[i].lvl = i + 1;
				book.Bids[i].px = Price::FromUnshifted(bids[i].px);
				book.Bids[i].size = bids[i].qty;
				book.Bids[i].seq = 0;
				book.Bids[i].secid = book.SecId;
			}
		}
		for (; i < MAX_DEPTH; ++i)
		{
			book.Bids[i].lvl = std::numeric_limits<uint16_t>::max();
			book.Bids[i].size = 0;
			book.Bids[i].seq = 0;
			book.Bids[i].secid = book.SecId;
		}
		i = 0;
		for (; i < asks.size() && i < MAX_DEPTH; ++i)
		{
			if (asks[i].qty != 0)
			{
				book.Asks[i].act = KT01::DataStructures::MarketDepth::BookAction::Change;
				book.Asks[i].lvl = i + 1;
				book.Asks[i].px = Price::FromUnshifted(asks[i].px);
				book.Asks[i].size = asks[i].qty;
				book.Asks[i].seq = 0;
				book.Asks[i].secid = book.SecId;
			}
		}
		for (; i < MAX_DEPTH; ++i)
		{
			book.Asks[i].lvl = std::numeric_limits<uint16_t>::max();
			book.Asks[i].size = 0;
			book.Asks[i].seq = 0;
			book.Asks[i].secid = book.SecId;
		}

		book.SecStatus = secStatus;
		this->onMktDataBidAsk(book);
	}

	void InjectDepth(
		int index,
		const std::vector<QtyPricePair> &bids,
		const std::vector<QtyPricePair> &asks,
		const KT01::DataStructures::MarketDepth::BookSecurityStatus secStatus = KT01::DataStructures::MarketDepth::BookSecurityStatus::ReadyToTrade)
	{
		static constexpr size_t MAX_DEPTH = 10;

		KT01::DataStructures::MarketDepth::DepthBook &book = this->_mdh->Obm.books[index];
		size_t i = 0;
		for (; i < bids.size() && i < MAX_DEPTH; ++i)
		{
			if (bids[i].qty != 0)
			{
				book.Bids[i].act = KT01::DataStructures::MarketDepth::BookAction::Change;
				book.Bids[i].lvl = i + 1;
				book.Bids[i].px = Price::FromUnshifted(bids[i].px);
				book.Bids[i].size = bids[i].qty;
				book.Bids[i].seq = 0;
				book.Bids[i].secid = book.SecId;
			}
		}
		for (; i < MAX_DEPTH; ++i)
		{
			book.Bids[i].lvl = std::numeric_limits<uint16_t>::max();
			book.Bids[i].size = 0;
			book.Bids[i].seq = 0;
			book.Bids[i].secid = book.SecId;
		}
		i = 0;
		for (; i < asks.size() && i < MAX_DEPTH; ++i)
		{
			if (asks[i].qty != 0)
			{
				book.Asks[i].act = KT01::DataStructures::MarketDepth::BookAction::Change;
				book.Asks[i].lvl = i + 1;
				book.Asks[i].px = Price::FromUnshifted(asks[i].px);
				book.Asks[i].size = asks[i].qty;
				book.Asks[i].seq = 0;
				book.Asks[i].secid = book.SecId;
			}
		}
		for (; i < MAX_DEPTH; ++i)
		{
			book.Asks[i].lvl = std::numeric_limits<uint16_t>::max();
			book.Asks[i].size = 0;
			book.Asks[i].seq = 0;
			book.Asks[i].secid = book.SecId;
		}

		book.SecStatus = secStatus;
		this->onMktDataBidAsk(book);
	}

	const stacker::Stack &GetStack(const int idx)
	{
		return this->GetInstrument()->GetStack(idx);
	}

	const stacker::MStack &GetMStack(const int idx)
	{
		return this->GetInstrument()->GetMStack(idx);
	}

	void InjectTrade(KTN::ORD::KOrderSide::Enum side, int qty, double px)
	{
		KT01::DataStructures::MarketDepth::DepthBook &book = this->_mdh->Obm.books[0];
		book.LastSize = qty;
		book.Last = Price::FromUnshifted(px);
		book.LastAgressor = stacker::KtnSideToCmeSide(side);
		this->onMktDataTrade(book);
	}

	void AdvanceTime(const akl::stacker::Duration &duration)
	{
		this->GetTime().SetTime(this->Now() + duration);
		this->Recalculate();
	}

	void InjectFill(KTN::OrderPod &o, uint32_t qty)
	{
		InjectFill(o, qty, o.price.AsUnshifted());
	}

	void InjectFill(KTN::OrderPod &o, uint32_t qty, double px)
	{
		o.lastqty = qty;
		o.lastpx = px;
		o.fillqty += qty;
		o.leavesqty -= qty;
		this->minOrderExecution(o, false);
	}

	void InjectAck(KTN::OrderPod &o)
	{
		o.OrdStatus = KTN::ORD::KOrderStatus::NEW;
		o.OrdState = KTN::ORD::KOrderState::WORKING;
		o.exchordid = cmeId++;
		this->minOrderAck(o);
	}

	void InjectModifyQtyAck(KTN::OrderPod o, uint32_t qty)
	{
		o.OrdStatus = KTN::ORD::KOrderStatus::MODIFIED;
		o.OrdState = KTN::ORD::KOrderState::WORKING;
		o.quantity = qty;
		this->minOrderModify(o);
	}

	void InjectCancelAck(KTN::OrderPod o)
	{
		KTN::OrderPod copy = o;
		copy.OrdStatus = KTN::ORD::KOrderStatus::CANCELED;
		copy.OrdState = KTN::ORD::KOrderState::COMPLETE;
		copy.leavesqty = 0;
		copy.quantity = 0;
		this->minOrderCancel(copy);
	}

	void InjectSubmitReject(KTN::OrderPod o)
	{
		o.OrdStatus = KTN::ORD::KOrderStatus::REJECTED;
		o.OrdState = KTN::ORD::KOrderState::COMPLETE;
		this->minOrderReject(o, "");
	}

	uint64_t NextExchangeOrderId()
	{
		return cmeId++;
	}

	template<typename, typename = void>
	struct has_id_member : std::false_type {};

	template<typename T>
	struct has_id_member<T, std::void_t<decltype(std::declval<T&>().id)>> : std::true_type {};

	template<typename T>
	void InjectMessage(const T &msg)
	{
		T copy = msg;
		stacker::AlgoMessage algoMsg;
		algoMsg.MessageType = T::type;
		if constexpr (has_id_member<T>::value)
		{
			copy.id = this->StrategyId();
		}
		algoMsg.payload = copy.SerializeToString();
		this->onAlgoMessage(algoMsg.SerializeToString());
	}

	void ExecuteStatusThread()
	{
		this->runStatusThread();
	}

	void SetSecurityIds()
	{
		this->_mdh->_algomds[0].SecId = this->secId;
		this->_mdh->_algomds[1].SecId = this->spreaderSecId;
	}

	void Start()
	{
		this->InjectMessage(stacker::StrategyStatusMessage { this->StrategyId(), 1 });
	}

	void Stop()
	{
		this->InjectMessage(stacker::StrategyStatusMessage { this->StrategyId(), 0 });
	}

	uint64_t cmeId { 10000 };
};

}