#pragma once

#include <optional>
#include <vector>
#include "../Time.hpp"
#include <akl/Quantity.hpp>
#include <akl/Price.hpp>
#include <akl/containers/CircularArray.hpp>

#include <Orders/OrderEnumsV2.hpp>

namespace akl::stacker
{

class RateLimitSettings;
struct Order;

class RateLimiter
{
private:
	struct Transaction
	{
		Timestamp time;
		int numTransactions;
	};

	using ContainerType = CircularArray<Transaction, 512>;

public:
	enum ActionType
	{
		SUBMIT,
		PRICE_MODIFY,
		QUANTITY_MODIFY,
		CANCEL,
		NONE
	};

	struct OrderAction
	{
		Order *o;
		std::optional<Price> px;
		std::optional<Quantity> qty;
		ActionType type;
	};

	enum Result
	{
		ALLOWED,
		QUEUED,
		UPDATED,
		REJECTED
	};

	RateLimiter(const RateLimitSettings &setting);

	bool TryAdd(const Timestamp now, const int numTransactions);
	void CheckTransactions(const Timestamp now);

	Result TrySubmit(const Timestamp now, Order *o);
	Result TryModify(const Timestamp now, Order *o, const Price px);
	Result TryModify(const Timestamp now, Order *o, const Quantity qty);
	Result TryCancel(const Timestamp now, Order *o);

	int TransactionsInWindow() const;
	int TransactionsLeft() const;
	inline bool SubmitQueueFull() const { return queuedSubmits.Full(); }
	inline int SubmitQueueSize() const { return queuedSubmits.Size(); }
	inline int SubmitQueueCapacity() const { return queuedSubmits.Capacity(); }
	inline int ModifyQueueSize() const { return queuedMods.Size(); }
	inline int CancelQueueSize() const { return queuedCancels.Size(); }

	OrderAction GetNextAction(const Timestamp now);

	void OnStrategyPause(std::vector<Order *> &ordersCanceled);
	void CancelAll(std::vector<Order *> &ordersCanceled);

private:
	RateLimiter(const RateLimiter &) = delete;
	RateLimiter &operator=(const RateLimiter &) = delete;
	RateLimiter(const RateLimiter &&) = delete;
	RateLimiter &operator=(const RateLimiter &&) = delete;

	const RateLimitSettings &settings;
	ContainerType transactions;
	int transactionsInWindow { 0 };

	void clearPendingMods();

	struct QueuedModify
	{
		Order *o;
		Price px;
		Quantity qty;

		enum Type
		{
			PRICE,
			QUANTITY
		};

		Type type;
	};

	struct Action
	{
		enum Type
		{
			SUBMIT,
			MODIFY,
			CANCEL
		};
		Order *o;
		Type type;
	};

	using QueuedOrderListType = CircularArray<Order *, 512>;
	using QueuedModifyListType = CircularArray<QueuedModify, 512>;
	QueuedOrderListType queuedSubmits;
	QueuedModifyListType queuedMods;
	QueuedOrderListType queuedCancels;
};

inline const char *toString(const RateLimiter::Result result)
{
	switch (result)
	{
	case RateLimiter::Result::ALLOWED:
		return "ALLOWED";
	case RateLimiter::Result::QUEUED:
		return "QUEUED";
	case RateLimiter::Result::UPDATED:
		return "UPDATED";
	case RateLimiter::Result::REJECTED:
		return "REJECTED";
	default:
		return "UNKNOWN";
	}
}

}