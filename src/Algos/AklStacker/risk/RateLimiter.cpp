#include "RateLimiter.hpp"

#include <AlgoParams/AklStackerParams.hpp>

#include <akl/BranchPrediction.hpp>

#include "../Order.hpp"

namespace akl::stacker
{

RateLimiter::RateLimiter(const RateLimitSettings &settings)
: settings(settings)
{

}

bool RateLimiter::TryAdd(const Timestamp now, const int numTransactions)
{
	CheckTransactions(now);

	if (likely(transactionsInWindow + numTransactions <= settings.limit))
	{
		if (!transactions.Empty() && transactions.Back().time == now)
		{
			transactionsInWindow += numTransactions;
			transactions.Back().numTransactions += numTransactions;
			return true;
		}
		else if (!transactions.Full())
		{
			transactionsInWindow += numTransactions;
			transactions.PushBack({now, numTransactions});
			return true;
		}
		else
		{
			LOGWARN("Failed to queue transaction: transactions list is full");
			return false;
		}
	}
	else
	{
		return false;
	}
}

void RateLimiter::CheckTransactions(const Timestamp now)
{
	if (!transactions.Empty())
	{
		const std::chrono::milliseconds window(settings.window);
		while (!transactions.Empty())
		{
			const Transaction &t = transactions.Front();
			if (now >= t.time && now - t.time >= window)
			{
				if (transactionsInWindow >= t.numTransactions)
				{
					transactionsInWindow -= t.numTransactions;
				}
				else
				{
					transactionsInWindow = 0;
					LOGWARN("Number of popped transactions greater than transactions in window");
				}
				LOGINFO("Popping transaction {}", t.numTransactions);
				transactions.PopFront();
			}
			else
			{
				break;
			}
		}
	}
}

RateLimiter::Result RateLimiter::TrySubmit(const Timestamp now, Order *o)
{
	if (likely(TryAdd(now, 1)))
	{
		return Result::ALLOWED;
	}
	else
	{
		if (likely(!queuedSubmits.Full() && settings.limit > 0))
		{
			queuedSubmits.PushBack(o);
			o->submitQueued = true;
			return Result::QUEUED;
		}
		else
		{
			return Result::REJECTED;
		}
	}
}

RateLimiter::Result RateLimiter::TryModify(const Timestamp now, Order *o, const Price px)
{
	for (int i = 0; i < queuedSubmits.Size(); ++i)
	{
		if (queuedSubmits[i] == o)
		{
			queuedSubmits[i]->price = px;
			queuedSubmits[i]->orderPod.price = px;
			return Result::UPDATED;
		}
	}
	for (int i = 0; i < queuedMods.Size(); ++i)
	{
		if (queuedMods[i].o == o)
		{
			queuedMods[i].px = px;
			return Result::UPDATED;
		}
	}

	if (likely(TryAdd(now, 1)))
	{
		return Result::ALLOWED;
	}
	else
	{
		if (!queuedMods.Full())
		{
			queuedMods.PushBack({ o, px, o->targetQuantity, QueuedModify::Type::PRICE });
			return Result::QUEUED;
		}
		else
		{
			return Result::REJECTED;
		}
	}
}

RateLimiter::Result RateLimiter::TryModify(const Timestamp now, Order *o, const Quantity qty)
{
	for (int i = 0; i < queuedSubmits.Size(); ++i)
	{
		if (queuedSubmits[i] == o)
		{
			queuedSubmits[i]->targetQuantity = qty;
			queuedSubmits[i]->orderPod.quantity = qty.value();
			return Result::UPDATED;
		}
	}
	for (int i = 0; i < queuedMods.Size(); ++i)
	{
		if (queuedMods[i].o == o)
		{
			queuedMods[i].qty = qty;
			return Result::UPDATED;
		}
	}

	if (likely(TryAdd(now, 1)))
	{
		return Result::ALLOWED;
	}
	else
	{
		if (!queuedMods.Full())
		{
			queuedMods.PushBack({ o, o->price, qty, QueuedModify::Type::QUANTITY });
			return Result::QUEUED;
		}
		else
		{
			return Result::REJECTED;
		}
	}
}

RateLimiter::Result RateLimiter::TryCancel(const Timestamp now, Order *o)
{
	for (int i = 0; i < queuedSubmits.Size(); ++i)
	{
		if (queuedSubmits[i] == o)
		{
			queuedSubmits.RemoveAt(i);
			return Result::UPDATED; // Order was never submitted; only need to remove from queue
		}
	}
	for (int i = 0; i < queuedMods.Size(); ++i)
	{
		if (queuedMods[i].o == o)
		{
			queuedMods.RemoveAt(i);
			break; // Still need to cancel
		}
	}

	if (TryAdd(now, 1))
	{
		return Result::ALLOWED;
	}
	else
	{
		if (!queuedCancels.Full())
		{
			queuedCancels.PushBack(o);
			o->cancelQueued.store(true, std::memory_order_release);
			return Result::QUEUED;
		}
		else
		{
			return Result::REJECTED;
		}
	}
}

int RateLimiter::TransactionsInWindow() const
{
	return transactionsInWindow;
}

int RateLimiter::TransactionsLeft() const
{
	return settings.limit > transactionsInWindow ? settings.limit - transactionsInWindow : 0;
}

RateLimiter::OrderAction RateLimiter::GetNextAction(const Timestamp now)
{
	if (TransactionsInWindow() < settings.limit)
	{
		if (!queuedCancels.Empty())
		{
			if (TryAdd(now, 1))
			{
				Order *o = queuedCancels.Front();
				queuedCancels.PopFront();
				return OrderAction { o, std::nullopt, std::nullopt, ActionType::CANCEL };
			}
		}
		else if (!queuedMods.Empty())
		{
			if (TryAdd(now, 1))
			{
				QueuedModify mod = queuedMods.Front();
				queuedMods.PopFront();
				if (mod.type == QueuedModify::Type::PRICE)
				{
					return OrderAction { mod.o, mod.px, std::nullopt, ActionType::PRICE_MODIFY };
				}
				else if (mod.type == QueuedModify::Type::QUANTITY)
				{
					return OrderAction { mod.o, std::nullopt, mod.qty, ActionType::QUANTITY_MODIFY };
				}
				else
				{

				}
			}
		}
		else if (!queuedSubmits.Empty())
		{
			if (TryAdd(now, 1))
			{
				Order *o = queuedSubmits.Front();
				queuedSubmits.PopFront();
				o->submitQueued = false;
				return OrderAction { o, std::nullopt, std::nullopt, ActionType::SUBMIT };
			}
		}
	}
	return OrderAction { nullptr, std::nullopt, std::nullopt, ActionType::NONE };
}

void RateLimiter::OnStrategyPause(std::vector<Order *> &ordersCanceled)
{
	for (int i = queuedSubmits.Size() - 1; i >= 0; --i)
	{
		if (queuedSubmits[i]->type == Order::Type::STACK
			|| queuedSubmits[i]->type == Order::Type::HEDGE
			|| queuedSubmits[i]->type == Order::Type::PRORATA)
		{
			ordersCanceled.push_back(queuedSubmits[i]);
			queuedSubmits.RemoveAt(i);
		}
	}

	clearPendingMods();
	queuedCancels.Clear();
}

void RateLimiter::CancelAll(std::vector<Order *> &ordersCanceled)
{
	for (int i = 0; i < queuedSubmits.Size(); ++i)
	{
		ordersCanceled.push_back(queuedSubmits[i]);
	}

	queuedSubmits.Clear();
	clearPendingMods();
	queuedCancels.Clear();
}

void RateLimiter::clearPendingMods()
{
	for (int i = 0; i < queuedMods.Size(); ++i)
	{
		const QueuedModify &mod = queuedMods[i];
		if (mod.type == QueuedModify::Type::PRICE)
		{
			// TODO
			// if (mod.o->strategyInfo.lastProcessedRequestId >= mod.o->strategyInfo.lastSentRequestId)
			// {
			// 	mod.o->price = mod.o->strategyInfo.ackedPrice;
			// }
		}
	}
	queuedMods.Clear();
}

}
