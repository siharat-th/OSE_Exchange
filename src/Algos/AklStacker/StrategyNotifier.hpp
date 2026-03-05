#pragma once

#include <string>
#include <Notifications/notifier_structs.hpp>
#include <Notifications/NotifierRest.hpp>
#include <KT01/Core/Singleton.hpp>

namespace akl::stacker
{

class StrategyNotifier
{
public:
	StrategyNotifier(const std::string &source, const std::string &org);
	void Notify(std::string msg, KTN::notify::MsgLevel::Enum level, KTN::notify::MsgType::Enum msgtype);

private:
	StrategyNotifier(const StrategyNotifier &) = delete;
	StrategyNotifier &operator=(const StrategyNotifier &) = delete;
	StrategyNotifier(const StrategyNotifier &&) = delete;
	StrategyNotifier &operator=(const StrategyNotifier &&) = delete;

	const std::string source;
	const std::string org;
};

}