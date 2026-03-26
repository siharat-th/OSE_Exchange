//============================================================================
// Name        	: OmnetSession.cpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
//============================================================================

#include "OmnetSession.hpp"
#include <KT01/Core/Log.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#undef __CLASS__
#define __CLASS__ "OmnetSession"

namespace KTN::OSE {

OmnetSession::OmnetSession()
	: _hSession(nullptr)
	, _ep0(0)
	, _loggedIn(false)
{
	memset(&_userCode, 0, sizeof(_userCode));
}

OmnetSession::~OmnetSession()
{
	if (_loggedIn)
		Logout();
}

bool OmnetSession::Login(const OseSessionSettings& sett, bool forceLogin)
{
	int32 txstatus = 0;

	// Create session (returns handle directly, no status param)
	_hSession = omniapi_create_session();
	if (_hSession == nullptr)
	{
		KT01_LOG_ERROR(__CLASS__, "omniapi_create_session returned NULL");
		return false;
	}

	// Build login struct
	omni_login_t login;
	memset(&login, 0, sizeof(login));

	strncpy(login.user_s, sett.LoginId.c_str(), sizeof(login.user_s) - 1);
	strncpy(login.pass_s, sett.Password.c_str(), sizeof(login.pass_s) - 1);
	strncpy(login.gateway_node_s, sett.GatewayHost.c_str(), sizeof(login.gateway_node_s) - 1);
	login.port_u = (uint32_t)sett.GatewayPort;

	if (forceLogin)
		login.forced_u = 1;  // LOGIN_FORCED

	KT01_LOG_INFO(__CLASS__, "Logging in to OMnet Gateway: " + sett.GatewayHost +
	              ":" + std::to_string(sett.GatewayPort) +
	              " user=" + sett.LoginId +
	              " forced=" + std::to_string(forceLogin));

	// Login
	int32 status = omniapi_login_ex(_hSession, &txstatus, &login);

	// Handle password expired (2027) — auto change if NewPassword is configured
	if (status == OMNIAPI_PWD_CHANGE_REQ)
	{
		KT01_LOG_INFO(__CLASS__, "Password expired (2027) — attempting password change");

		if (sett.NewPassword.empty())
		{
			KT01_LOG_ERROR(__CLASS__, "Password expired but OSE.NewPassword not set in config!");
			omniapi_close_session(_hSession);
			_hSession = nullptr;
			return false;
		}

		omni_set_password_t pwdchange;
		memset(&pwdchange, 0, sizeof(pwdchange));
		strncpy(pwdchange.pass_s, sett.Password.c_str(), sizeof(pwdchange.pass_s) - 1);
		strncpy(pwdchange.new_pass_s, sett.NewPassword.c_str(), sizeof(pwdchange.new_pass_s) - 1);

		int32 pwdstatus = 0;
		int32 rc = omniapi_set_newpwd_ex(_hSession, &pwdstatus, &pwdchange);
		if (rc != OMNIAPI_SUCCESS)
		{
			std::string errmsg = GetErrorMessage(rc);
			KT01_LOG_ERROR(__CLASS__, "Password change failed: status=" + std::to_string(rc) +
			               " txstatus=" + std::to_string(pwdstatus) + " msg=" + errmsg);
			omniapi_close_session(_hSession);
			_hSession = nullptr;
			return false;
		}

		KT01_LOG_INFO(__CLASS__, "Password changed successfully! Update OSE.Password in config and remove OSE.NewPassword");
	}
	else if (status == OMNIAPI_PWD_IMPEND_EXP)
	{
		KT01_LOG_INFO(__CLASS__, "Login OK — password expiration impending (2028), consider changing soon");
	}
	else if (status != OMNIAPI_SUCCESS)
	{
		std::string errmsg = GetErrorMessage(status);
		KT01_LOG_ERROR(__CLASS__, "omniapi_login_ex failed: status=" + std::to_string(status) +
		               " txstatus=" + std::to_string(txstatus) + " msg=" + errmsg);
		omniapi_close_session(_hSession);
		_hSession = nullptr;
		return false;
	}

	KT01_LOG_INFO(__CLASS__, "Login successful");

	// Get user code
	uint32 len = sizeof(user_code_t);
	status = omniapi_get_info_ex(_hSession, &txstatus, OMNI_INFTYP_USERCODE, &len, (char*)&_userCode);
	if (status != OMNIAPI_SUCCESS)
	{
		KT01_LOG_ERROR(__CLASS__, "Failed to get user code: " + std::to_string(status));
	}

	// Get facility EP0
	len = sizeof(_ep0);
	status = omniapi_get_info_ex(_hSession, &txstatus, OMNI_INFTYP_FACTYP_E0, &len, (char*)&_ep0);
	if (status != OMNIAPI_SUCCESS)
	{
		KT01_LOG_ERROR(__CLASS__, "Failed to get EP0 facility: " + std::to_string(status));
	}
	else
	{
		PUTLONG(_ep0, _ep0); // Convert to host byte order
		KT01_LOG_INFO(__CLASS__, "EP0 facility: " + std::to_string(_ep0));
	}

	_loggedIn = true;
	return true;
}

void OmnetSession::Logout()
{
	if (!_loggedIn || !_hSession)
		return;

	int32 txstatus = 0;
	int32 status = omniapi_logout_ex(_hSession, &txstatus);
	if (status != OMNIAPI_SUCCESS)
	{
		KT01_LOG_ERROR(__CLASS__, "omniapi_logout_ex failed: " + std::to_string(status));
	}
	else
	{
		KT01_LOG_INFO(__CLASS__, "Logout successful");
	}

	omniapi_close_session(_hSession);
	_hSession = nullptr;
	_loggedIn = false;
}

int OmnetSession::SendTransaction(const void* txbuf, size_t len, uint32 facility,
                                   uint32* txid, quad_word* ordid, int32* txstatus_out)
{
	if (!_loggedIn)
		return OMNIAPI_NOT_LOGGED_IN;

	// Allocate MAX_REQUEST_SIZE — API may write response back into the same buffer
	omni_message* msg = (omni_message*)malloc(MAX_REQUEST_SIZE);
	if (!msg)
		return OMNIAPI_FAILURE;

	memset(msg, 0, MAX_REQUEST_SIZE);
	msg->length_u = (uint32)len;
	memcpy((char*)(msg + 1), txbuf, len);

	int32 txstatus = 0;

	// quad_word for txid/ordid — API writes 8 bytes (quad_word) per apisample.c
	quad_word txidQw;
	quad_word ordidQw;
	memset(&txidQw, 0, sizeof(quad_word));
	memset(&ordidQw, 0, sizeof(quad_word));

	int32 status = omniapi_tx_ex(_hSession, &txstatus, facility, &msg,
	                             (uint32*)&txidQw, (uint32*)&ordidQw);

	// Copy back to caller
	if (txid)
		*txid = *(uint32*)&txidQw; // low 32 bits for logging/compat
	if (ordid)
		*ordid = ordidQw; // full 8-byte quad_word

	// Debug: hex dump of ordid quad_word
	{
		char hbuf[4];
		std::string ordHex, txHex;
		for (int i = 0; i < 8; ++i)
		{
			snprintf(hbuf, sizeof(hbuf), "%02X ", (unsigned char)ordidQw.quad_word[i]);
			ordHex += hbuf;
			snprintf(hbuf, sizeof(hbuf), "%02X ", (unsigned char)txidQw.quad_word[i]);
			txHex += hbuf;
		}
		KT01_LOG_INFO(__CLASS__, "TX result: status=" + std::to_string(status) +
		              " txstatus=" + std::to_string(txstatus) +
		              " txid=" + txHex + " ordid=" + ordHex +
		              " rsp_len=" + std::to_string(msg ? msg->length_u : 0));
	}

	// Return txstatus to caller
	if (txstatus_out)
		*txstatus_out = txstatus;

	if (status != OMNIAPI_SUCCESS)
	{
		std::string errmsg = GetErrorMessage(txstatus);
		std::string statusmsg = GetErrorMessage(status);
		KT01_LOG_ERROR(__CLASS__, "omniapi_tx_ex failed: status=" + std::to_string(status) +
		               " (" + statusmsg + ")" +
		               " txstatus=" + std::to_string(txstatus) +
		               " (" + errmsg + ")");

		// Mark session dead on fatal connection errors
		if (status == OMNIAPI_INTFAILURE || status == OMNIAPI_NOTCONNECTED)
			_loggedIn = false;
	}

	free(msg);
	return status;
}

int OmnetSession::SendQuery(const void* qrybuf, size_t len,
                             void* rcvbuf, size_t& rcvlen, uint32 facility,
                             uint32* txid, uint32* ordid)
{
	if (!_loggedIn)
		return OMNIAPI_NOT_LOGGED_IN;

	// Build send message
	omni_message* sndmsg = (omni_message*)malloc(sizeof(omni_message) + len);
	if (!sndmsg)
		return OMNIAPI_FAILURE;

	sndmsg->length_u = (uint32)len;
	memcpy((char*)(sndmsg + 1), qrybuf, len);

	// Use caller's buffer directly (DQ124 VIM can be >8KB)
	uint32 msglen = static_cast<uint32>(rcvlen);

	int32 txstatus = 0;
	uint32 localTxid = 0;
	uint32 localOrdid = 0;

	int32 status = omniapi_query_ex(_hSession, &txstatus, facility, sndmsg, 1,
	                                (int8*)rcvbuf, &msglen,
	                                txid ? txid : &localTxid,
	                                ordid ? ordid : &localOrdid);

	if (status == OMNIAPI_SUCCESS)
	{
		rcvlen = static_cast<size_t>(msglen);
	}
	else
	{
		std::string errmsg = GetErrorMessage(txstatus);
		KT01_LOG_ERROR(__CLASS__, "omniapi_query_ex failed: status=" + std::to_string(status) +
		               " txstatus=" + std::to_string(txstatus) + " msg=" + errmsg);
	}

	free(sndmsg);
	return status;
}

int OmnetSession::SubscribeAll()
{
	if (!_loggedIn)
		return OMNIAPI_NOT_LOGGED_IN;

	// Step 1: Get all available event types (per apisample.c pattern)
	uint32 buffSize = 256;
	char* strBuffer = nullptr;
	int32 status;

	do
	{
		free(strBuffer);
		strBuffer = (char*)malloc(buffSize);
		uint32 sz = buffSize;
		status = omniapi_read_event_ext_ex(_hSession, OMNI_EVTTYP_SHOW,
		                                    (int8*)strBuffer, &sz, nullptr, 0);
		if (status == OMNIAPI_TRUNCATED)
			buffSize *= 2;
	} while (status == OMNIAPI_TRUNCATED);

	if (status != OMNIAPI_SUCCESS)
	{
		KT01_LOG_ERROR(__CLASS__, "Failed to get event types: " + std::to_string(status));
		free(strBuffer);
		return status;
	}

	// Step 2: Parse comma-separated event type list and subscribe to each
	std::string eventList(strBuffer);
	free(strBuffer);

	KT01_LOG_INFO(__CLASS__, "Available event types: " + eventList);

	char* ptr = &eventList[0];
	int subscribed = 0;
	while (*ptr != '\0')
	{
		uint32 eventType = strtoul(ptr, &ptr, 10);
		if (eventType == 0)
			break;

		int32 rc = omniapi_set_event_ex(_hSession, eventType, nullptr);
		if (rc == OMNIAPI_SUCCESS || rc == OMNIAPI_ALR_SET)
			subscribed++;
		else
			KT01_LOG_WARN(__CLASS__, "Failed to subscribe event type " +
			              std::to_string(eventType) + ": " + std::to_string(rc));

		if (*ptr == ',')
			ptr++; // skip comma delimiter
	}

	KT01_LOG_INFO(__CLASS__, "Subscribed to " + std::to_string(subscribed) + " event types");

	// Force-subscribe to Info Type 4 (BD6) and 8 (BO5) even if not in available list
	const uint32 extraTypes[] = {4, 8};
	for (uint32 et : extraTypes)
	{
		int32 rc = omniapi_set_event_ex(_hSession, et, nullptr);
		KT01_LOG_INFO(__CLASS__, "Force subscribe event type " + std::to_string(et) +
		              ": rc=" + std::to_string(rc));
	}

	return OMNIAPI_SUCCESS;
}

int OmnetSession::ReadEvent(void* buf, size_t& len, uint32 eventType, int32 flags)
{
	if (!_loggedIn)
		return OMNIAPI_NOT_LOGGED_IN;

	uint32 msglen = (uint32)len;
	int32 evtmask = 0;
	int32 status = omniapi_read_event_ext_ex(_hSession, eventType, (int8*)buf, &msglen, &evtmask, flags);

	if (status == OMNIAPI_SUCCESS)
		len = (size_t)msglen;

	return status;
}

std::string OmnetSession::GetErrorMessage(int32 status)
{
	if (!_hSession)
		return "no session";

	int8 msgbuf[256];
	memset(msgbuf, 0, sizeof(msgbuf));
	uint32 msglen = sizeof(msgbuf);

	int32 rc = omniapi_get_message_ex(_hSession, status, msgbuf, &msglen, 1);
	if (rc == OMNIAPI_SUCCESS)
		return std::string((char*)msgbuf, msglen);

	return "unknown error " + std::to_string(status);
}

} // namespace KTN::OSE
