//============================================================================
// Name        	: SessErrors.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Jun 26, 2023 12:16:20 PM
//============================================================================

#ifndef SRC_EXCHSUPPORT_CME_IL3_SESSERRORS_HPP_
#define SRC_EXCHSUPPORT_CME_IL3_SESSERRORS_HPP_

#include <map>
#include <string>

namespace KTN{
namespace CME{
namespace IL3{

class ErrorCodes {
public:
    static std::string getSessionErrorMessage(int errorCode);
    static std::string getBusRejectErrorMessage(int errorCode);
    static std::string getTerminateMessage(int errorCode);

    static std::string getOrderRejectMessage(int errorCode);
    static std::string getCxlRejectMessage(int errorCode);
    static std::string getCxlReplaceRejectMessage(int errorCode);
    static std::string getRetransmitRejectMessage(int errorCode);



private:
    static const std::map<int, std::string> sessionErrorMessages;
    static const std::map<int, std::string> busRejectErrorMessages;
    static const std::map<int, std::string> terminateCodes;

    static const std::map<int, std::string> orderRejectErrorCodes;
    static const std::map<int, std::string> cxlRejectErrorCodes;
    static const std::map<int, std::string> cxlReplaceRejectErrorCodes;

    static const std::map<int, std::string> retransmitRejectCodes;

};

} } }


#endif /* SRC_EXCHSUPPORT_CME_IL3_SESSERRORS_HPP_ */
