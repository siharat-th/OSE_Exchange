//============================================================================
// Name        	: EfViCallback.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 2, 2023 5:08:13 PM
//============================================================================

#ifndef SRC_KTNEFVI_UDPCALLBACK_HPP_
#define SRC_KTNEFVI_UDPCALLBACK_HPP_

/**
 * @class UdpCallback
 * @brief Represents a callback interface for UDP data reception.
 */
class UdpCallback {
public:
	/**
  * @brief Default constructor.
  */
	UdpCallback() {};

	/**
  * @brief Virtual destructor.
  */
	virtual ~UdpCallback() {};

	/**
  * @brief Called when UDP data is received.
  * @param data The received data.
  * @param len The length of the received data.
  * @param source The source of the received data.
  */
	virtual void onRecv(char * data, int len, int source) = 0;
};

/**
 * @class UdpCallbackV3
 * @brief Represents a callback interface for UDP data reception (version 3).
 */
class UdpCallbackV3 {
public:
	/**
  * @brief Called when UDP data is received.
  * @param data The received data.
  * @param len The length of the received data.
  */
	virtual void onRecv(char * data, int len) = 0;
};

#endif /* SRC_KTNEFVI_EFVICALLBACK_HPP_ */
