#pragma once

#include <Orders/OrderPod.hpp>
#include <Orders/Order.hpp>
#include <akl/Price.hpp>

template <>
struct fmt::formatter<OrderPod> {
    // Parse format specifications (empty for now, but you could add options later)
    constexpr auto parse(format_parse_context& ctx) {
        // Return an iterator to the end of the parsed range
        return ctx.begin();
    }

    // Format the OrderPod object
    template <typename FormatContext>
    auto format(const OrderPod& pod, FormatContext& ctx) const
	{
        // Return an iterator past the end of the output
        return fmt::format_to(
            ctx.out(),
            "OrderPod{{"
            "execid=\"{}\", ordernum=\"{}\", price={}, stopprice={}, lastpx={}, "
            "timestamp={}, orderReqId={}, exchordid={}, secid={}, "
            "quantity={}, leavesqty={}, fillqty={}, lastqty={}, dispqty={}, minqty={}, "
            "stratid={}, rejreason={}, callbackid={}, mktsegid={}, instindex={}, "
            "isManual={}, OrdExch={}, OrdType={}, OrdTif={}, "
            "OrdAction={}, OrdSide={}, OrdStatus={}, OrdState={}, "
            "OrdFillType={}, OrdAlgoTrig={}"
            "}}",
            pod.execid, pod.ordernum, pod.price, pod.stopprice, pod.lastpx,
            pod.timestamp, pod.orderReqId, pod.exchordid, pod.secid,
            pod.quantity, pod.leavesqty, pod.fillqty, pod.lastqty, pod.dispqty, pod.minqty,
            pod.stratid, pod.rejreason, pod.callbackid, pod.mktsegid, pod.instindex,
            pod.isManual, 
            static_cast<int>(pod.OrdExch), static_cast<int>(pod.OrdType), static_cast<int>(pod.OrdTif),
            static_cast<int>(pod.OrdAction), static_cast<int>(pod.OrdSide), static_cast<int>(pod.OrdStatus), 
            static_cast<int>(pod.OrdState), static_cast<int>(pod.OrdFillType), static_cast<int>(pod.OrdAlgoTrig)
        );
    }
};

// Formatter for libfmt
template <>
struct fmt::formatter<KTN::Order> {
	// Parse format specification (empty for now)
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	// Format the Order object
	template <typename FormatContext>
	auto format(const KTN::Order& o, FormatContext& ctx) const {
		return fmt::format_to(
			ctx.out(),
			"Order[reqId={}, exchordid={}, sym={}, ordernum={}, side={}, type={}, "
			"status={}, qty={}/{}/{}, price={:.2f}, stoppx={:.2f}, "
			"action={}, text={}]",
			o.orderReqId,
			o.exchordid,
			o.symbol,
			o.ordernum,
			static_cast<int>(o.OrdSide),
			static_cast<int>(o.OrdType),
			static_cast<int>(o.OrdStatus),
			o.quantity, o.fillqty, o.leavesqty,
			o.price,
			o.stoppx,
			static_cast<int>(o.OrdAction),
			o.text
		);
	}
};

template <int PLACES>
struct fmt::formatter<akl::Decimal<PLACES>> {
	constexpr auto parse(format_parse_context& ctx) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const akl::Decimal<PLACES>& dec, FormatContext& ctx) const {
		return fmt::format_to(ctx.out(), "{}", dec.AsShifted());
	}
};