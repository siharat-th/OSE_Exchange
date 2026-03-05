/*
 * TransportException.hh
 *
 *  Created on: Apr 8, 2011
 *      Author: val
 */

#ifndef SystemCallException_HH_
#define SystemCallException_HH_

#include "exception.hh"

#include <string>


using namespace std;

namespace LimeBrokerage
{

static const string SYSTEM_EXCEPTION = "System Exception";
class SystemCallException : public Exception
{
public:
    SystemCallException(int errorCode);
    SystemCallException(int errorCode, const string& message);
    SystemCallException(const string& message);
    virtual ~SystemCallException() {};

    int getError() const { return errno_; }
private:
    SystemCallException();
    int errno_;
};

}

#endif /* SystemCallException_HH_ */
