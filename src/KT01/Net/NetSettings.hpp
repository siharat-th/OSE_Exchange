//============================================================================
// Name        	: NetSettings.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Aug 20, 2023 9:33:03 AM
//============================================================================

#ifndef SRC_KT01_NETSETTINGS_HPP_
#define SRC_KT01_NETSETTINGS_HPP_

#include <KT01/Core/Macro.hpp>
#include <KT01/Core/Settings.hpp>
#include <KT01/Net/helpers.hpp>

namespace KT01 {
namespace NET {

	/**
	 * @brief Enum class representing the protocol type.
	 */
	enum class Protocol {
		TCP,
		UDP
	};

	/**
	 * @brief Structure representing network settings.
	 */
	struct NetSettings {
		// Common fields
		bool Debug;
		int Affinity;
		bool CalcLatency;
	
		int Offset;
		int RxPollCnt;
		int PollWaitNs;
		int RxCapacity;
		int RxBatchSize;
		int MsgBufSize;
		int Timeout;
	
		std::string IntfA, IntfB;
		std::string IntfNameA, IntfNameB;
	
		// TCP-specific fields
		int EventPollMax = 0;
		bool UseDelSends = false;
		bool UseCtpio = false;
	
		/**
		 * @brief Load settings from config based on protocol.
		 */
		static void Load(NetSettings& ns, Protocol proto, string filename) {
			std::string prefix = (proto == Protocol::TCP) ? "Tcp" : "Udp";
			Settings st(filename);
			ns.Debug = st.getBoolean(prefix + ".Debug");
			ns.IntfNameA = st.getString(prefix + ".InterfaceNameA");
			ns.IntfA = NetHelpers::getIPAddress(ns.IntfNameA);
	
			ns.IntfNameB = st.getString(prefix + ".InterfaceNameB");
			ns.IntfB = NetHelpers::getIPAddress(ns.IntfNameB);
	
			ns.Affinity = st.getInteger(prefix + ".Affinity");
			ns.CalcLatency = st.getBoolean(prefix + ".CalculateUDPLatency");
			ns.MsgBufSize = st.getInteger(prefix + ".MsgBuffSize");
			ns.Offset = st.getInteger(prefix + ".EFVI_OFFSET");
			ns.PollWaitNs = st.getInteger(prefix + ".PollWaitNs");
			ns.RxBatchSize = st.getInteger(prefix + ".RxBatchSize");
			ns.RxCapacity = st.getInteger(prefix + ".RxCapacity");
			ns.RxPollCnt = st.getInteger(prefix + ".RxPollCnt");
			ns.Timeout = st.getInteger(prefix + ".TimeoutMS");
	
			if (proto == Protocol::TCP) {
				ns.EventPollMax = st.getInteger("Tcp.EventPollLooper");
				ns.UseDelSends = st.getBoolean("Tcp.UseDelegatedSends");
				ns.UseCtpio = st.getBoolean("Tcp.UseCtpio");
			}
		}
	};
	
} // namespace NET
} // namespace KTN
//============================================================================
#endif /* SRC_KT01_NET_SFCSTRUCTS_HPP_ */
