//
// "exception.cc" - common exception base class implementation
//
// Copyright 2008 Lime Brokerage LLC. All rights reserved.
//
// Change history:
//    4/18/08  rmf  Initial coding.
//

#include "exception.hh"

// Standard C++ headers.
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

// Other C++ headers.
#include <cxxabi.h>

// Other C headers.
extern "C" {
#include <execinfo.h>
}

namespace {
    void computeLineNumbers(std::vector<std::string>& result, std::string line)
    {
        // Each line has the form: program-name(mangled-function-name+offset) [address]
        std::string program = line.substr(0, line.find_first_of('('));
        line = line.substr(line.find_first_of('(') + 1);
        std::string mangledFunction = line.substr(0, line.find_first_of('+'));
        line = line.substr(line.find_first_of('+') + 1);
        std::string offset = line.substr(0, line.find_first_of(')'));
        line = line.substr(line.find_last_of('[') + 1);
        std::string address = line.substr(0, line.find_first_of(']'));

        std::ostringstream command;
        command << "addr2line -Cfi -e " << program << " " << address;
        FILE* commandFile = ::popen(command.str().c_str(), "r");
        if (!commandFile) {
            result.push_back("  [" + address + "]  " + Exception::demangle(mangledFunction) + "  (" + program + ")");
            return;
        }

        while (1) {
            char line[1024];
            if (!fgets(line, 1024, commandFile))
                break;
            std::string function(line, std::remove(line, std::find(line, line+1024, 0x0), '\n'));
            if (!fgets(line, 1024, commandFile))
                break;
            std::string file(line, std::remove(line, std::find(line, line+1024, 0x0), '\n'));
            if (function.find("??") == 0)
                function = Exception::demangle(mangledFunction);
            if (function.empty())
                function = "????";
            if (file.find("??") == 0)
                file = program;
            if (!result.empty() || function != "Exception")
                result.push_back("  [" + address + "]  " + function + "  (" + file + ")");
        }

        pclose(commandFile);
    }
}

// Exception constructor

Exception::Exception() :
    backtraceSize(::backtrace(backtraceBuffer, backtraceLimit)),
    errorMessage_("")
{
    // Since the backtrace buffer contains the *return* address,
    // subtract one from it to obtain the address which actually made
    // the function call.
    for (int i = 0; i < backtraceSize; ++i)
        backtraceBuffer[i] = reinterpret_cast<char*>(backtraceBuffer[i])-1;
}

Exception::Exception(const std::string& errorMessage) :
    backtraceSize(::backtrace(backtraceBuffer, backtraceLimit)),
    errorMessage_(errorMessage)
{
    // Since the backtrace buffer contains the *return* address,
    // subtract one from it to obtain the address which actually made
    // the function call.
    for (int i = 0; i < backtraceSize; ++i)
        backtraceBuffer[i] = reinterpret_cast<char*>(backtraceBuffer[i])-1;
}


// Exception destructor

Exception::~Exception()
{
}



// Exception::what()

// Default implementation.

const std::string& Exception::what() const
{
    return errorMessage_;
}



// Exception::getStackBacktrace

std::vector<std::string> Exception::getStackBacktrace() const
{
    std::vector<std::string> result;
    result.reserve(backtraceSize);
    if (char** symbols = ::backtrace_symbols(backtraceBuffer, backtraceSize)) {
        for (int i = 0; i < backtraceSize; ++i) {
            std::string line(symbols[i]);
            computeLineNumbers(result, line);
        }
        std::free(symbols);
    }
    return result;
}

// Exception::demangle

std::string Exception::demangle(const std::string& name)
{
    int status = 0;
    char* demangled = __cxxabiv1::__cxa_demangle(name.c_str(), 0, 0, &status);
    if (status != 0)
        return name;
    std::string result(demangled);
    std::free(demangled);
    return result;
}
