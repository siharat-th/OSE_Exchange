#pragma once

#include <Reporters/ReporterBase.hpp>
#include <Maps/SPSCRingBuffer.hpp>

#include <Algos/AklStacker/messages/ProrataProfileUpdateMessage.hpp>
#include <Algos/AklStacker/messages/ProrataProfileDeleteMessage.hpp>
#include <Algos/AklStacker/messages/AlgoStats.hpp>

#include <tbb/concurrent_queue.h>
#include <variant>

#include <akl/BranchPrediction.hpp>

namespace akl
{

class ProrataMessagePublisher : public KTN::REPORTER::ReporterBase
{
public:
	using MessageVariant = std::variant
	<
		std::monostate,
		stacker::ProrataProfileUpdateMessage,
		stacker::ProrataProfileDeleteMessage
	>;

	ProrataMessagePublisher(const std::string &name, int sleepms)
		: KTN::REPORTER::ReporterBase(name, sleepms)
	{
		algoMsg.instance = "*";
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

	bool IsEmpty() const { return msgQueue.empty(); }

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
	}

	virtual void DoWork() override final
	{
		while (!IsEmpty())
		{
			Process();
		}
	}

private:
	tbb::concurrent_queue<MessageVariant> msgQueue;
	stacker::AlgoStats algoMsg;

	template<typename MESSAGE_TYPE>
	std::string toJsonString(const MESSAGE_TYPE &msg)
	{
		algoMsg.messageType = ToString(msg.type);
		algoMsg.text = msg.SerializeToString();
		return algoMsg.SerializeToString();
	}
	
	std::string toJsonString(const std::monostate &)
	{
		return std::string{};
	}
};

}