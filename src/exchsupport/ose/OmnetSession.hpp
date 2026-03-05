//============================================================================
// Name        	: OmnetSession.hpp
// Author      	: Katana Financial
// Copyright   	: Copyright (C) 2026 Katana Financial
// Description 	: OMnet API session wrapper (login/logout/tx/query/broadcast)
//============================================================================

#ifndef SRC_EXCHSUPPORT_OSE_OMNETSESSION_HPP_
#define SRC_EXCHSUPPORT_OSE_OMNETSESSION_HPP_

#pragma once

#include <string>
#include <cstring>
#include <omni.h>
#include <omnifact.h>
#include <omniapi.h>
#include <omex_ose.h>
#include "settings/OseSessionSettings.hpp"

namespace KTN::OSE {

class OmnetSession
{
public:
	OmnetSession();
	~OmnetSession();

	// Session lifecycle
	bool Login(const OseSessionSettings& sett, bool forceLogin = false);
	void Logout();
	bool IsLoggedIn() const { return _loggedIn; }

	// Blocking transaction (MO31, MO33, MO4, UI1, etc.)
	// ordid is quad_word (8 bytes) — exchange order number
	int SendTransaction(const void* txbuf, size_t len, uint32 facility,
	                    uint32* txid = nullptr, quad_word* ordid = nullptr,
	                    int32* txstatus_out = nullptr);

	// Blocking query (DQ2, DQ3, DQ7, etc.)
	int SendQuery(const void* qrybuf, size_t len,
	              void* rcvbuf, size_t& rcvlen, uint32 facility,
	              uint32* txid = nullptr, uint32* ordid = nullptr);

	// Broadcast subscription & polling
	int SubscribeAll();
	int ReadEvent(void* buf, size_t& len, uint32 eventType = 1, int32 flags = 0);

	// Info
	omniapi_session_handle GetHandle() const { return _hSession; }
	uint32 GetFacilityEP0() const { return _ep0; }
	const user_code_t& GetUserCode() const { return _userCode; }
	std::string GetErrorMessage(int32 status);

private:
	omniapi_session_handle _hSession;
	uint32 _ep0;
	user_code_t _userCode;
	bool _loggedIn;
};

} // namespace KTN::OSE

#endif /* SRC_EXCHSUPPORT_OSE_OMNETSESSION_HPP_ */
