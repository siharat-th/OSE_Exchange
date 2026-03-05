//============================================================================
// Name        	: SesssionStatus.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 22, 2023 1:44:00 PM
//============================================================================

#ifndef SRC_SESSION_SESSIONSTATUS_HPP_
#define SRC_SESSION_SESSIONSTATUS_HPP_
#pragma once
#include <vector>
#include <string>
using namespace std;

/**
 * @brief Represents the status of a session.
 */
class SessionStatus
{
public:
	std::string source; /**< The source of the session. */
	std::string subid; /**< The subid of the session. */
	std::string ok; /**< The ok status of the session. */
	std::string sessionstatus; /**< The status of the session. */
	int	msgseqnum; /**< The message sequence number of the session. */
	std::string ipaddress; /**< The IP address of the session. */
	int port; /**< The port of the session. */

	string org; /**< The organization of the session. */

	int mktsegid; /**< The market segment ID of the session. */
	string desc; /**< The description of the session. */
	int protocol; /**< The protocol of the session. */
	string state; /**< The state of the session. */
	string exch; /**< The exchange of the session. */

	bool m_bIsLoggedIn; /**< Flag indicating if the session is logged in. */
	bool m_bHbStarted; /**< Flag indicating if the session heartbeat has started. */
	char m_LoginResponseStatus; /**< The login response status of the session. */
	std::string m_LoginResponseText; /**< The login response text of the session. */
	uint32_t m_LastSeqNum; /**< The last sequence number of the session. */

	uint32_t MSG_SEQ_OUT_; /**< The outgoing message sequence number of the session. */
	int TAS_SEQ_LAST; /**< The last TAS sequence number of the session. */
};

#endif /* SRC_SESSION_SESSIONSTATUS_HPP_ */
