//============================================================================
// Name        	: AppMessageFactoryFASTV2.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 20, 2023 4:11:32 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CFE_BOE3_APPMESSAGEFACTORYFASTV2_HPP_
#define SRC_EXCHSUPPORT_CFE_BOE3_APPMESSAGEFACTORYFASTV2_HPP_
#pragma once

#include <exchsupport/cfe/Boe3/Boe3Includes.hpp>

#include <KT01/Net/byte_writer.hpp>
#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>
#include <KT01/Net/helpers.hpp>
using namespace KTN::NET;

#include <external/tscns/tscns.h>

#include <KT01/Core/PerformanceTracker.hpp>
#include <KT01/Core/Macro.hpp>
#include <KT01/Core/Log.hpp>

#include <Orders/OrderPod.hpp>
#include <Orders/IdGen.h>
using namespace KTN;

#include <exchsupport/cfe/factory/AppMessageGen.hpp>


namespace KTN{
namespace CFE{
namespace Boe3{

class AppMessageFactoryFastV2 {
public:
	AppMessageFactoryFastV2();
	virtual ~AppMessageFactoryFastV2();
	void Init(string settingsfile);

	//order basic ops:
	void NewOrderEncodeMan(const KTN::OrderPod& ord, uint32_t seqnum, const char *oeoid, KTNBuf& msg, bool logtoscreen);
	void ModifyOrderEncodeMan(uint32_t qty, akl::Price price, const char* newordernum, char* origordernum, uint32_t seqnum, 
								KOrderType::Enum ordtype, const char *oeoid, KTNBuf &container);
	void CancelOrderEncodeMan(char* origordernum, uint32_t seqnum, const char *oeoid, KTNBuf& container);

	
	uint16_t NewOrderEncodeAuto(char* clordid, uint32_t qty, akl::Price px, uint64_t secid,
			KOrderSide::Enum side, KOrderTif::Enum tif, uint32_t seqnum, char* &buf);
	uint16_t ModifyOrderEncodeAuto(uint32_t qty, akl::Price price, const char* newordernum, char* origordernum, uint32_t seqnum, char* &buf);
	void CancelOrderEncodeAuto(char* origordernum, uint32_t seqnum, KTNBuf& container);

	//All assumed to be autos
	void GetNewOrderTemplate(NewOrderMsgV2*& msg);
	void GetModOrderTemplate(ModifyOrderMsg*& msg);
	void GetCxlOrderTemplate(CancelOrderMsg*& msg);

	inline void UpdateClock()
	{
		_tscns.calibrate();
	}

	inline uint64_t GetTimestamp()
	{
		return _tscns.rdns();
	}

private:

	void InitMessages(string settingsfile);

	inline void fastCopy(char* dest, const char* src, size_t length) {
	    for (size_t i = 0; i < length; ++i) {
	    	//if (src[i] == '\0')
	    	//	break;  // Stop copying if we reach the end of the source string
	        dest[i] = src[i];
	    }
	}

	inline void fastCopy2(char* dest, const char* src, size_t length) {
	    size_t i = 0;
	    for (; i + 4 <= length; i += 4) {
	        dest[i] = src[i];
	        dest[i + 1] = src[i + 1];
	        dest[i + 2] = src[i + 2];
	        dest[i + 3] = src[i + 3];
	    }
	    for (; i < length; ++i) {
	        dest[i] = src[i];
	    }
	}

	inline void bitCopy(char* dest, const char* src, size_t length)
	{
		size_t i;
		for (i = 0; i < length - 15; i += 16) {
		dest[i] = src[i];
		dest[i + 1] = src[i + 1];
		dest[i + 2] = src[i + 2];
		dest[i + 3] = src[i + 3];
		dest[i + 4] = src[i + 4];
		dest[i + 5] = src[i + 5];
		dest[i + 6] = src[i + 6];
		dest[i + 7] = src[i + 7];
		dest[i + 8] = src[i + 8];
		dest[i + 9] = src[i + 9];
		dest[i + 10] = src[i + 10];
		dest[i + 11] = src[i + 11];
		dest[i + 12] = src[i + 12];
		dest[i + 13] = src[i + 13];
		dest[i + 14] = src[i + 14];
		dest[i + 15] = src[i + 15];
	}

	// Copy any remaining bytes (if length is not a multiple of 16)
		for (; i < length; ++i) {
			dest[i] = src[i];
		}
	}

	void fast_memset(void* dest, int value, std::size_t count) {
	    asm volatile (
	        "cld; rep stosb"
	        :
	        : "D" (dest), "c" (count), "a" (value)
	        : "memory"
	    );
	}

//	inline void fastMemcpy(void *pvDest, void *pvSrc, size_t nBytes) {
//	  assert(nBytes % 32 == 0);
//	  assert((intptr_t(pvDest) & 31) == 0);
//	  assert((intptr_t(pvSrc) & 31) == 0);
//	  const __m256i *pSrc = reinterpret_cast<const __m256i*>(pvSrc);
//	  __m256i *pDest = reinterpret_cast<__m256i*>(pvDest);
//	  int64_t nVects = nBytes / sizeof(*pSrc);
//	  for (; nVects > 0; nVects--, pSrc++, pDest++) {
//	    const __m256i loaded = _mm256_stream_load_si256(pSrc);
//	    _mm256_stream_si256(pDest, loaded);
//	  }
//	  _mm_sfence();
//	}

private:
	CfeBoe3Settings _sess;
	ByteWriter _writer;
	PerformanceTracker _trk;

	KTNBuf _NEW_MANUAL;
	KTNBuf _NEW_ATS;

	KTNBuf _NEW_MANUAL_MKT;
	KTNBuf _NEW_ATS_MKT;

	KTNBuf _NEW_MANUAL_STOP_LIMIT;
	KTNBuf _NEW_ATS_STOP_LIMIT;

	KTNBuf _MOD_MANUAL;
	KTNBuf _MOD_ATS;

	KTNSimp _MODV4;
	KTNSimp _NEWV4;

	KTNBuf _CXL_MANUAL;
	KTNBuf _CXL_ATS;

	KTNBuf* _SLUGS[3];

	const int SLUG_NEW = 0;
	const int SLUG_MOD = 1;
	const int SLUG_CXL = 2;

	//EpochNanos _ts;
	TSCNS _tscns;

};


} } }
#endif /* SRC_EXCHSUPPORT_CME_IL3_APPMESSAGEFACTORY_HPP_ */
