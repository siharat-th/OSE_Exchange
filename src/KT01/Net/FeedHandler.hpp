/*
 * FeedHandler.hpp
 *
 *  Created on: Feb 28, 2017
 *      Author: sgaer
 */

#ifndef FEEDHANDLER_HPP_
#define FEEDHANDLER_HPP_

#pragma once
#pragma GCC diagnostic ignored "-Wreorder"

#include <KT01/Net/feed_includes.hpp>
#include <KT01/Net/feed_defines.hpp>
#include <KT01/Net/feed_structs.hpp>
#include <KT01/Net/Udp/UdpCallback.hpp>
#include <Settings/CmeMdpSettings.h>


#include <linux/if_ether.h>



namespace KTN {
namespace EFVI {

/**
 * @brief The FeedHandler class is an abstract base class for feed handlers.
 */
class FeedHandler {
public:
	/**
  * @brief Constructs a FeedHandler object with the given name.
  * @param name The name of the feed handler.
  */
	FeedHandler(const std::string& name, UdpCallback& callback);

	/**
  * @brief Destroys the FeedHandler object.
  */
	virtual ~FeedHandler();

	/**
  * @brief Starts the feed handler.
  */
	virtual void Start()=0;

	/**
  * @brief Stops the feed handler.
  */
	virtual void Stop()=0;

	/**
  * @brief Resets the feed handler.
  */
	virtual void Reset()=0;

	/**
  * @brief Prints the statistics of the feed handler.
  */
	void PrintStats();

	/**
  * @brief Checks the onload status of the feed handler.
  */
	void CheckOnload();

	/**
  * @brief Adds a filter for the given symbol.
  * @param symbol The symbol to add the filter for.
  */
	virtual void AddFilter(const std::string& ipaddress, int port, const std::string& feedab)=0;

	/**
  * @brief Prints the sequences.
  */
	virtual void printSeqs() = 0;

protected:
	/**
  * @brief Logs the given message.
  * @param szMsg The message to log.
  * @param iColor The color of the log message.
  */
	void LogMe(std::string szMsg);

	/**
  * @brief Makes the socket non-blocking.
  * @param sfd The socket file descriptor.
  * @return 0 if successful, -1 otherwise.
  */
	int make_socket_non_blocking (int sfd);

	/**
  * @brief Handles the einprogress event.
  * @param sfd The socket file descriptor.
  * @return 0 if successful, -1 otherwise.
  */
	int handle_einprogress(int sfd);

	/**
  * @brief Outputs the statistical information.
  * @param name The name of the statistical information.
  * @param r The result of the statistical information.
  * @param stat The onload statistics.
  */
	void outputStatInfo(std::string name,int r, struct onload_stat *stat);

	/**
  * @brief Prints the statistics.
  * @param vec The vector of statistics.
  * @param title The title of the statistics.
  */
	void printstats(std::vector<int> & vec, std::string title);

	//void msg_handler_csm(int feedtype, int iSource, char* msgbuf, int nbytes);

	/**
  * @brief Writes the CSM latency log.
  * @param source The source of the log.
  * @param feedtype The feed type of the log.
  * @param templateid The template ID of the log.
  * @param msgseqnum The message sequence number of the log.
  * @param msgnum The message number of the log.
  * @param num The number of the log.
  */
	void WriteCsmLatLog(int source, std::string feedtype, int templateid, int msgseqnum, int msgnum, int num);

	/**
	 * @brief Accessor for the UDP callback.
	 */
	UdpCallback& callback() { return m_callback; }
	const UdpCallback& callback() const { return m_callback; }

	//bool checkForPreOpen(int feedtype, int iSource, char* msgbuf, int nbytes);
	//void addFilter(int iSecId);

protected:
	const std::string _MDPFile = "MdpSettings.txt";

	std::vector<int> vLatsL1;
	std::vector<int> vLatsL2;
	std::vector<int> vLatsL3;
	std::vector<int> vLatsL4;

	typedef std::vector<int> vlats;

	vlats myVLats[4];

	std::vector<int> vLatsRX;
	std::vector<int> vEvsRX;

	uint32_t LAST_L1_SEQ;
	uint32_t LAST_L2_SEQ;
	std::map<int, uint32_t> seqMap;

	uint32_t LAST_L1_SEQa;
	uint32_t LAST_L2_SEQa;

	uint8_t L1_TEMPLATE;
	uint8_t L2_TEMPLATE;

	int BAD_RESULT_NS;
	bool USE_MULTICAST_ALL;

	//the symbol we're looking for
	uint32_t m_iPre0;
	bool m_bPre0Sent;
	bool m_TAS_SENT;

	std::string m_Me;
	UdpCallback& m_callback;

	bool m_bCSM_TEST;
	int m_iTestCounter;
	int m_iTestTrigger;

	//global
	PerformanceCounter::Count lat_;
	PerformanceCounter::Span nanos_;

	PerformanceCounter::Count lat2_;
	PerformanceCounter::Span nanos2_;

	inline uint64_t rdtsc() {
		uint32_t low, high;
		asm volatile ("rdtsc" : "=a" (low), "=d" (high));
		return (uint64_t)high << 32 | low;
	}
};

} /* namespace core */
} /* namespace LWT */

#endif /* FEEDHANDLER_HPP_ */
