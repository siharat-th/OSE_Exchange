//============================================================================
// Name        	: EfViCallback.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 2, 2023 5:08:13 PM
//============================================================================

#ifndef SRC_KTNEFVI_TCPCALLBACK_HPP_
#define SRC_KTNEFVI_TCPCALLBACK_HPP_
#include <cstdint>

namespace KT01 {
namespace NET{
namespace TCP {

	using byte_ptr = uint8_t*;

/**
 * @brief The TcpCallback class is an abstract base class for TCP callbacks.
 */
class TcpCallback  {
public:
	/**
  * @brief Constructs a TcpCallback object.
  */
	TcpCallback(){};

	/**
  * @brief Destroys the TcpCallback object.
  */
	virtual ~TcpCallback() {};

	/**
  * @brief This method is called when data is received over TCP.
  * @param data The received data.
  * @param len The length of the received data.
  * @param segid The segment ID.
  * @param index The index.
  * @return The response code.
  */
	virtual uint16_t onRecv(char * data, int len, int segid, int index) = 0;
};

/**
 * @brief The TcpCallback8t class is an abstract base class for TCP callbacks with 8-bit data.
 */
class TcpCallback8t  {
public:
	/**
  * @brief This method is called when 8-bit data is received over TCP.
  * @param data The received data.
  * @param len The length of the received data.
  * @param segid The segment ID.
  * @param index The index.
  * @return The response code.
  */
	virtual uint16_t onRecv(uint8_t * data, int len, int segid, int index) = 0;

	virtual void onConnectionClosed(int segid, int index) { } // TODO mwojcik
};

} // namespace TCP
} // namespace NET 
} // namespace KT01

#endif /* SRC_KTNEFVI_EFVICALLBACK_HPP_ */
