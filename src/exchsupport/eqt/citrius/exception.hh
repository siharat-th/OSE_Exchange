//
// "exception.hh" - common exception base class definition
//
// Copyright 2008 Lime Brokerage LLC.  All rights reserved.
//
// Change history:
//    4/18/08  rmf  Initial coding.
//

#ifndef EXCEPTION_HH
#define EXCEPTION_HH

// Standard C++ headers.
#include <string>
#include <vector>



// Exception base class providing the ability to generate a stack backtrace from
// the point from which the (derived) exception was thrown.

class Exception {
public:
    Exception();
    Exception(const std::string& errorMessage);
    virtual ~Exception();
    virtual const std::string& what() const;
    std::vector<std::string> getStackBacktrace() const;
    static std::string demangle(const std::string& name);
private:
    static const int backtraceLimit = 100;
    void* backtraceBuffer[backtraceLimit];
    int backtraceSize;
    std::string errorMessage_;
};



#endif  // EXCEPTION_HH
