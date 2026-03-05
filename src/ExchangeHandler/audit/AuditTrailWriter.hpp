//============================================================================
// Name        	: AuditTrailWriter.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Feb 3, 2023 1:50:50 PM
//============================================================================

#ifndef SRC_LOGGERS_AUDITTRAILWRITERSVC_HPP_
#define SRC_LOGGERS_AUDITTRAILWRITERSVC_HPP_
#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <cstring>
#include <pthread.h>
#include <unistd.h>  // For usleep
#include <memory>    // For std::unique_ptr
#include <set>
#include <variant>
#include <tbb/concurrent_queue.h>
using namespace std;

#include <KT01/Core/BufferPool.hpp>

#include <KT01/Core/Macro.hpp>
#include <KT01/Core/NamedThread.hpp>
#include <KT01/Core/Log.hpp>
#include <Loggers/FileLogger.hpp>

#include <KT01/Net/netstructs.hpp>
#include <KT01/Net/KtnBuf8t.hpp>
using namespace KTN::NET;

#include <Maps/SPSCRingBuffer.hpp>
using namespace KTN::Core;


#include <ExchangeHandler/audit/IAuditProcessor.hpp>
using namespace KTN;


#include <KT01/Net/KtnBuf8t.hpp>
#include <tbb/concurrent_queue.h>
#include <variant>
#include <KT01/Core/BufferPool.hpp>

/**
 * @class AuditTrailWriter
 * @brief Represents a ultra low latency class for writing audit trail logs using SPSC RingBuffer Queues.
 */
class AuditTrailWriter : public NamedThread{

private:
	std::unique_ptr<IAuditProcessor> _processor;

public:
	/**
  * @brief Constructor for AuditTrailWriter.
  * @param processor A unique pointer to an exchange/protocol specific IAuditProcessor object.
  */
    AuditTrailWriter(std::unique_ptr<IAuditProcessor> processor, BufferPool &sendBufferPool);

	/**
  * @brief Destructor for AuditTrailWriter.
  */
	virtual ~AuditTrailWriter();

	/**
  * @brief Starts the audit trail writer.
  * @param dir The directory path for storing the audit trail files.
  * @param sessid The session ID.
  * @param firmid The firm ID.
  * @param core The core number.
  */
	void Start(string dir, string sessid, string firmid, int core);

	/**
  * @brief Initializes the audit trail file for a specific segment.
  * @param segid The segment ID.
  */
	void InitFileForSeg(int segid);

	/**
  * @brief Creates an audit trail file for a specific session, firm, and segment.
  * @param sessid The session ID.
  * @param firmid The firm ID.
  * @param segid The segment ID.
  */
	void CreateAuditFile(string sessid, string firmid, int segid);

	/**
  * @brief Writes a message to the audit trail.
  * @param msg The message to write.
  */
	void Write(KTNBuf* msg);

	/**
  * @brief Writes a message to the audit trail.
  * @param buf The buffer containing the message.
  * @param msgtype The message type.
  * @param segid The segment ID.
  * @param customerOrderTime The customer order timestamp (nanoseconds).
  */
	//void Write(char* buf, int msgtype, int segid);

	void Write(KTNBuf8t &&msg);

	/**
  * @brief Gets the size of the audit trail writer.
  * @return The size of the audit trail writer.
  */
	// inline int Size()
	// {
	// 	//return _BUFQ.size_approx();
	// 	return _BUFQ.empty();
	// }

private:
	/**
  * @brief Runs the audit trail writer.
  */
	void Run();

	/**
  * @brief Starts the logging thread.
  */
	void StartLoggingThread();

	/**
  * @brief Checks if a file exists.
  * @param filename The name of the file.
  * @return True if the file exists, false otherwise.
  */
	bool fileExists(string filename);

	/**
  * @brief Checks if a file is open.
  * @param filename The name of the file.
  * @return True if the file is open, false otherwise.
  */
	bool fileIsOpen(string filename);

	/**
  * @brief Processes a line of the audit trail.
  * @param buf The buffer containing the line.
  * @param iTemplateid The template ID.
  * @param segid The segment ID.
  * @param customerOrderTime The customer order timestamp (nanoseconds).
  */
	void ProcessLine(char* buf, int iTemplateid, int segid, uint64_t customerOrderTime = 0);

	/**
  * @brief Processes a message.
  * @param buf The buffer containing the message.
  * @param iTemplateid The template ID.
  * @param customerOrderTime The customer order timestamp (nanoseconds).
  * @return The processed message.
  */
	std::string process(char* buf, int iTemplateid, uint64_t customerOrderTime = 0);

	/**
  * @brief Checks if a key exists in the audit trail writer.
  * @param segid The segment ID.
  * @return True if the key exists, false otherwise.
  */
	bool KeyExists(int segid);

private:
	string _DIR; /**< The directory path for storing the audit trail files. */
	string _FIRMID; /**< The firm ID. */
	string _SESSID; /**< The session ID. */
	int _CORE; /**< The core number. */

	//SPSCRingBuffer<KTNBuf*> _MQ; /**< The message queue. */
	//SPSCRingBuffer<std::tuple<char*, int, int>> _BUFQ; /**< The buffer queue. */

	using MessageVariantType = std::variant<KTNBuf *, KTNBuf8t>;
	using MessageQueueType = tbb::concurrent_queue<MessageVariantType>;
	MessageQueueType msgQueue;

	std::atomic<bool> _ACTIVE; /**< Flag indicating if the audit trail writer is active. */

	const string _me = "AuditTrailWriter"; /**< The name of the audit trail writer. */

	struct Key {
		std::string sessid; /**< The session ID. */
		std::string firmid; /**< The firm ID. */
		int segid; /**< The segment ID. */

		/**
   * @brief Checks if two keys are equal.
   * @param other The other key to compare.
   * @return True if the keys are equal, false otherwise.
   */
		bool operator==(const Key& other) const {
			return sessid == other.sessid && firmid == other.firmid && segid == other.segid;
		}

		/**
   * @brief Defines the comparison operator for Key.
   * @param other The other key to compare.
   * @return True if this key is less than the other key, false otherwise.
   */
		bool operator<(const Key& other) const {
			if (segid != other.segid)
				return segid < other.segid;
			if (firmid != other.firmid)
				return firmid < other.firmid;
			return sessid < other.sessid;
		}
	};

	struct KeyHasher {
		/**
   * @brief Calculates the hash value for a key.
   * @param key The key.
   * @return The hash value.
   */
		std::size_t operator()(const Key& key) const {
			std::size_t hash = 0;
			hash_combine(hash, key.sessid);
			hash_combine(hash, key.firmid);
			hash_combine(hash, key.segid);
			return hash;
		}

		/**
   * @brief Combines the hash value with a value.
   * @param seed The seed value.
   * @param value The value to combine.
   */
		template <typename T>
		void hash_combine(std::size_t& seed, const T& value) const {
			seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};

	std::unordered_map<Key, FileLogger, KeyHasher> _logmap; /**< The map of keys to file loggers. */
	std::set<Key> _keySet; /**< The set of keys. */
	std::set<int> _segs; /**< The set of segment IDs. */

	BufferPool &sendBufferPool;
};

#endif /* SRC_LOGGERS_AUDITTRAILWRITERSVC_HPP_ */
