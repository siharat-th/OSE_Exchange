//============================================================================
// Name        	: IL3LoginHelpers.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 11, 2023 9:20:18 AM
//============================================================================


/******
 *
 * 10/24/2024 - Change to latest OpenSSL
 *
 * Made changes to bring OpenSSL up to date.
 * I resisted using newer version previously as older compilers gave weird results.
 *
 * This is a good forcing function to bring up to date.
 *
 * OpenSSL 1.1.0 introduced changes where the internal structure of HMAC_CTX became opaque,
 * meaning it can no longer be allocated as a regular object.
 *
 * Instead, you must allocate and manage it using
 * HMAC_CTX_new and HMAC_CTX_free, which ensures compatibility with the latest versions of OpenSSL.
 * This updated code should resolve the issues related to deprecated functions and provide better
 * memory management through the new dynamic allocation approach.
 *
 */

#ifndef SRC_EXCHSUPPORT_CME_IL3_IL3LOGINHELPERS_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_IL3LOGINHELPERS_HPP_

#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>
#include <algorithm>
 
#include <cstdint>  // For fixed-width integer types (e.g., int32_t, uint16_t)
#include <cstring>  // For string operations (e.g., strncpy)
#include <stdint.h>
#include <unistd.h>

#include <map>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <string.h>
#include <cstring>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>


#include <exchsupport/cme/il3/IL3Types.hpp>
#include <exchsupport/cme/il3/IL3Composites.hpp>
#include <exchsupport/cme/il3/IL3Enums.hpp>
#include <exchsupport/cme/auth/sign.hpp>

#include <mutex>

using namespace std;

namespace KTN{
namespace CME{
namespace IL3{

class IL3LoginHelpers {
public:
	IL3LoginHelpers();
	virtual ~IL3LoginHelpers();

	// Definition of ByteArray as a typedef for std::vector<unsigned char>
	typedef std::vector<unsigned char> ByteArray;


	static uint64_t GenerateRequestTimestamp() {
	    auto currentTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
	        std::chrono::high_resolution_clock::now().time_since_epoch()
	    ).count();

	    return static_cast<uint64_t>(currentTimestamp);
	}

	static uint64_t GenFastTimestamp() {
	    uint64_t timestamp;
	#if defined(_MSC_VER)
	    // For Microsoft Visual C++ compiler (MSVC) on Windows
	    // Use __rdtsc intrinsic to access the timestamp counter
	    timestamp = __rdtsc();
	#elif defined(__GNUC__) || defined(__clang__)
	    // For GCC or Clang on x86_64 (Intel/AMD) architecture
	    // Use inline assembly to access the timestamp counter
	    uint32_t low, high;
	    asm volatile("rdtsc" : "=a"(low), "=d"(high));
	    timestamp = static_cast<uint64_t>(low) | (static_cast<uint64_t>(high) << 32);
	#else
	    // Use the std::chrono approach as fallback for other platforms or compilers
	    auto currentTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
	        std::chrono::high_resolution_clock::now().time_since_epoch()
	    ).count();
	    timestamp = static_cast<uint64_t>(currentTimestamp);
	#endif
	    return timestamp;
	}

	static uint64_t  GenerateUUID()
	{
		 auto currentTimestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
		).count();
		return static_cast<uint64_t>(currentTimestamp);
	}

	static std::string GenerateCanonicalMessageForNegotiate(string session, string firmID,
	    long requestTimestamp, long uUID)
	{

	    std::stringstream canonicalMsg;
	    canonicalMsg << requestTimestamp << std::endl;
	    canonicalMsg << uUID << std::endl;
	    canonicalMsg << session << std::endl;
	    canonicalMsg << firmID;

	  //  cout << "CANON FOR NEGOTIATE: \n" << canonicalMsg.str() << endl;

	    return canonicalMsg.str();
	}

	static std::string GenerateCanonicalMessageForEstablish(string session, string firmID,
			long requestTimestamp,
			long uUID,
			string tradingSystemName,
			string tradingSystemVersionId,
			string tradingSystemVendorId,
			long nextSeqNo = 1,
			long keepAliveInterval = 30)
	{

		long keepAliveMS = keepAliveInterval * 1000;
	    std::stringstream canonicalMsg;
	    canonicalMsg << requestTimestamp << std::endl;
	    canonicalMsg << uUID << std::endl;
	    canonicalMsg << session << std::endl;
	    canonicalMsg << firmID << std::endl;
	    canonicalMsg << tradingSystemName << std::endl;
	    canonicalMsg << tradingSystemVersionId << std::endl;
	    canonicalMsg << tradingSystemVendorId << std::endl;
	    canonicalMsg << nextSeqNo << std::endl;
	    canonicalMsg << keepAliveMS;

	 //   cout << "CANON FOR ESTABLISH: \n" << canonicalMsg.str() << endl;

	    return canonicalMsg.str();
	}

	static std::string calculateHMAC(const std::string& canonicalMsg, const std::string& SecretKey, bool debug=false)
	{
		auto signature = Sign::calculateHMAC(SecretKey, canonicalMsg, debug);
		return signature;
	}

//	static std::string calculateHMAC3Mtx(const std::string& canonicalRequest, const std::string& userKey) {
//	  try {
//		// Initialize HMAC instance with the key
//		// Decode the key first, since it is base64url encoded
//		BIO* bio = BIO_new(BIO_f_base64());
//		if (!bio) {
//		  throw std::runtime_error("calculateHMAC3 BIO_new() failed.");
//		}
//
//		// Remove base64url padding characters ('=')
//		std::string decodedUserKey = userKey;
//		decodedUserKey.erase(std::remove(decodedUserKey.begin(), decodedUserKey.end(), '='), decodedUserKey.end());
//
//		// Replace base64url characters '-' with '+', and '_' with '/'
//		std::replace(decodedUserKey.begin(), decodedUserKey.end(), '-', '+');
//		std::replace(decodedUserKey.begin(), decodedUserKey.end(), '_', '/');
//
//		// Calculate padding length
//		int paddingLength = decodedUserKey.size() % 4;
//		if (paddingLength == 2) {
//		  decodedUserKey += "==";
//		} else if (paddingLength == 3) {
//		  decodedUserKey += "=";
//		}
//
//		// Create BIO object with base64 decoding filter
//		BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
//		BIO* memBio = BIO_new_mem_buf(decodedUserKey.data(), static_cast<int>(decodedUserKey.size()));
//		memBio = BIO_push(bio, memBio);
//
//		// Decode the user key
//		std::vector<unsigned char> decodedKey(decodedUserKey.size());
//		int bytesRead = BIO_read(memBio, decodedKey.data(), static_cast<int>(decodedKey.size()));
//		decodedKey.resize(bytesRead);
//
//		// Clean up BIO objects
//		BIO_free_all(memBio);
//
//
//		std::mutex hmacMutex;
//
//			// Calculate HMAC
//			HMAC_CTX* hmacContext = HMAC_CTX_new();  // Use the new OpenSSL API
//			if (!hmacContext) {
//				throw std::runtime_error("Failed to create HMAC_CTX");
//			}
//
//			// Lock the mutex before accessing shared resources
//			std::lock_guard<std::mutex> lock(hmacMutex);
//
//			// Initialize the HMAC context
//			if (!HMAC_Init_ex(hmacContext, decodedKey.data(), static_cast<int>(decodedKey.size()), EVP_sha256(), nullptr)) {
//				HMAC_CTX_free(hmacContext);
//				throw std::runtime_error("HMAC_Init_ex failed");
//			}
//
//			// Update the HMAC context with the canonical request
//			if (!HMAC_Update(hmacContext, reinterpret_cast<const unsigned char*>(canonicalRequest.c_str()), canonicalRequest.size())) {
//				HMAC_CTX_free(hmacContext);
//				throw std::runtime_error("HMAC_Update failed");
//			}
//
//			// Prepare the result buffer and calculate the final HMAC digest
//			unsigned int digestLength = 0;
//			char result[32]; // Adjust the size based on the expected output length
//
//			if (!HMAC_Final(hmacContext, reinterpret_cast<unsigned char*>(result), &digestLength)) {
//				HMAC_CTX_free(hmacContext);
//				throw std::runtime_error("HMAC_Final failed");
//			}
//
//			// Clean up the HMAC context
//			HMAC_CTX_free(hmacContext);
//
//
//			// Unlock the mutex after finishing with shared resources
//
//			if (digestLength != 32) {
//			throw std::runtime_error("HMAC digest length is not 32.");
//			std::exit(1);
//			}
//
//			std::string res = "";
//			res = result;
//
//			return res;
////		} // Release the lock automatically at the end of the scope
//
//	  } catch (const std::exception& e) {
//		std::cerr << "Exception occurred: " << e.what() << std::endl;
//		return "";
//	  }
//	}

};




} } }

#endif /* SRC_EXCHSUPPORT_CME_IL3_IL3LOGINHELPERS_HPP_ */
