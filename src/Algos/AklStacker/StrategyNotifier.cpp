#include "StrategyNotifier.hpp"

namespace akl::stacker
{

StrategyNotifier::StrategyNotifier(const std::string &source, const std::string &org)
: source(source)
, org(org)
{

}

void StrategyNotifier::Notify(std::string msg, KTN::notify::MsgLevel::Enum level, KTN::notify::MsgType::Enum msgtype)
{
#if !AKL_TEST_MODE
	KTN::notify::Notification n = {};
	n.exch = "ALGO";
	n.source = source;
	n.org = org;
	n.level = level;
	n.imsg = msgtype;
	n.text = msg;
	KTN::notify::NotifierRest::instance().Notify(n);
#endif
}

}