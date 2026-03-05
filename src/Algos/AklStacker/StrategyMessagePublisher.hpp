#pragma once

#include <Reporters/ReporterBase.hpp>
#include <Maps/SPSCRingBuffer.hpp>

#include "messages/StackerPositionUpdateMessage.hpp"
#include "messages/StackTargetsMessage.hpp"
#include "messages/StackInfoMessage.hpp"
#include "messages/StopUpdateMessage.hpp"
#include "messages/StackerRiskInfoMessage.hpp"
#include "messages/StackerConfigMessage.hpp"
#include "messages/StackerPriceBandsMessage.hpp"
#include "messages/StackerAdoptMessage.hpp"
#include "messages/StackerProrataConfigMessage.hpp"
#include "messages/StopSettingsMessage.hpp"
#include "messages/StackerProrataOrderResponseMessage.hpp"
#include "messages/StrategyStatusMessage.hpp"
#include "messages/StackerSpreaderUpdateMessage.hpp"

#include "messages/AlgoStats.hpp"

#include <tbb/concurrent_queue.h>
#include <variant>

#include <akl/BranchPrediction.hpp>

namespace akl::stacker
{

class StrategyMessagePublisher : public KTN::REPORTER::ReporterBase
{
public:
	using MessageVariant = std::variant
	<
		std::monostate,
		StrategyStatusMessage,
		StackerPositionUpdateMessage,
		StackerRiskInfoMessage,
		StackTargetsMessage,
		StackInfoMessage,
		StackerConfigMessage,
		StackerPriceBandsMessage,
		StackerAdoptMessage,
		StackerProrataConfigMessage,
		StopUpdateMessage,
		StopSettingsMessage,
		StackerProrataOrderResponseMessage,
		StackerSpreaderUpdateMessage
	>;

	StrategyMessagePublisher(const std::string &name, int sleepms, const std::string &strategyId)
		: KTN::REPORTER::ReporterBase(name, sleepms)
		, strategyId(strategyId)
	{
		algoMsg.instance = strategyId;
	}

	void SetSource(const std::string &source)
	{
		algoMsg.source = source;
	}

	template<typename MESSAGE_TYPE>
	void Push(const MESSAGE_TYPE &msg)
	{
		msgQueue.push(msg);
	}

	void Push(const AklStackerParams &params)
	{
		paramsQueue.push(params);
	}

	MessageVariant Pop()
	{
		MessageVariant msg;
		if (msgQueue.try_pop(msg))
		{
			return msg;
		}
		else
		{
			return std::monostate{};
		}
	}

	bool IsMessageQueueEmpty() const { return msgQueue.empty(); }
	bool IsParamsQueueEmpty() const { return paramsQueue.empty(); }

	void Process()
	{
		MessageVariant msg;
		while (msgQueue.try_pop(msg))
		{
			const std::string s = std::visit([this](auto &&m) { return toJsonString(m); }, msg);
			if (likely(!s.empty()))
			{
				SendJson(s);
				KT01_LOG_INFO(__CLASS__, "Published message: {}", s); // TODO change level?
			}
		}
		AklStackerParams params;
		while (paramsQueue.try_pop(params))
		{
			if (paramsQueue.empty()) // Only send last one
			{
				const std::string s = toJsonString(params);
				if (likely(!s.empty()))
				{
					SendJson(s);
					KT01_LOG_INFO(__CLASS__,"Published params: ", s); // TODO change level
				}
			}

		}
	}

	virtual void DoWork() override final
	{
		while (!IsMessageQueueEmpty() || !IsParamsQueueEmpty())
		{
			Process();
		}
	}

private:
	tbb::concurrent_queue<MessageVariant> msgQueue;
	tbb::concurrent_queue<AklStackerParams> paramsQueue;
	const std::string strategyId;
	AlgoStats algoMsg;

	template<typename MESSAGE_TYPE>
	std::string toJsonString(const MESSAGE_TYPE &msg)
	{
		algoMsg.messageType = ToString(msg.type);
		algoMsg.text = msg.SerializeToString();
		return algoMsg.SerializeToString();
	}

	std::string toJsonString(const AklStackerParams &params)
	{
		algoMsg.messageType = "ALGOPARAMS";
		algoMsg.text = params.toJson();
		return algoMsg.SerializeToString();
	}

	std::string toJsonString(const std::monostate &)
	{
		return std::string{};
	}
};

}