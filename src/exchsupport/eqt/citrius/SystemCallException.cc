
#include "SystemCallException.hh"
#include "Misc.hh"

extern "C" {
#include <string.h>
}
namespace LimeBrokerage
{

SystemCallException::SystemCallException(int error, const string& message)
    :  Exception(Misc::concatenate(message, ", ", strerror(error))), errno_(error)
{
}
SystemCallException::SystemCallException(const string& message)
    : Exception(message), errno_(0)
{

}
SystemCallException::SystemCallException(int errorCode)
    : Exception(strerror(errorCode)), errno_(errorCode)
{

}

}
