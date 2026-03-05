/*
 * CmdLineHandler.cc
 *
 *  Created on: May 13, 2010
 *      Author: val
 */

#include "CmdLineHandler.hh"

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>

namespace LimeBrokerage {

int CmdArgument::maxDescription = 0;
int CmdArgument::upToLimit = 30;
CmdArgument::CmdArgument(const char arg_char, const char* arg_name,
		bool require_argument, bool is_mandatory,
		const char* description, const char* default_value)
	: _argChar(arg_char), _argName(arg_name), _requireArgument(require_argument),
	_isMandatory(is_mandatory), _description(description), _repeating(false),
	_upToDescription(20+_argName.size()+strlen(default_value))
    , _optionExist(false)

{
    _value.push_back(default_value);
    CmdArgument::maxDescription = std::max(maxDescription, _upToDescription);
}

CmdArgument::CmdArgument(const char arg_char, const char* arg_name,
        bool require_argument, bool is_mandatory,
        const char* description, bool repeating)
    : _argChar(arg_char), _argName(arg_name), _requireArgument(require_argument),
    _isMandatory(is_mandatory), _description(description), _repeating(repeating),
	_upToDescription(20+_argName.size())
    , _optionExist(false)
{
    CmdArgument::maxDescription = std::max(maxDescription, _upToDescription);

}
const std::string CmdArgument::operator() () const
{
    return _argName;
}
int CmdArgument::getIntValue() const
{

	return atoi(_value[0].c_str());
}

bool CmdArgument::getBoolValue() const
{
	return _value[0] == "true" ? true : false;
}

std::string CmdArgument::getStringValue() const
{
	return _value[0];
}

ValueList CmdArgument::getValueList() const
{
    return _value;
}
CmdArgument* CmdLineHandler::printUsage = NULL;
const ValueList CmdLineHandler::EMPTY_LIST;

CmdLineHandler::CmdLineHandler(CmdArgument argList[], int argSize, const char* description, const char* usage)
    : _processedCmd(false), _programName(""), _description(description), _usage(usage), _totalMandatoryArgs(0)
{
    if (printUsage == NULL)
        printUsage = new CmdArgument('h', "help", false, false, "Print This Usage Message. Default false", "false");
    _argOptionMap[printUsage->getArgChar()] = printUsage;
    _argNameMap[printUsage->getArgName()] = printUsage;
    _optString += printUsage->getArgChar();
    _getoptString = printUsage->getArgChar();

    for (int i = 0; i < argSize; i++) {
        if (_getoptString.find(argList[i].getArgChar()) != std::string::npos) {
            std::cerr << "Ignoring Duplicate Option: " << argList[i]  << std::endl;
            continue;
        }
        if (_argNameMap[argList[i].getArgName()] != NULL) {
            std::cerr << "Ignoring Duplicate Option: " << argList[i] << std::endl;
            continue;
        }
        _optString += argList[i].getArgChar();
        _getoptString += argList[i].getArgChar();
        if (argList[i].getRequireArgument()) {
            _getoptString += ":";
        }
        _argOptionMap[argList[i].getArgChar()] =  &argList[i];
        if (argList[i].getIsMandatory()) {
            _totalMandatoryArgs++;
        }
        _argNameMap[argList[i].getArgName()] = &argList[i];
    }
}

bool CmdLineHandler::processCmd(int argc, char** argv)
{
    if (_processedCmd) return true;
    int m = 0;
    _programName = argv[0];
    for (int i = 1; i < argc; i++) {
        char c = argv[i][0];
        if (c != '-') {
            //Assume non option argument i.e.
            _mandatoryAttributes.push_back(argv[i]);
            continue;
        }
        if (c == 'h') {
            return false;
        }
        char* cmdArg = argv[i];
        for (unsigned int k = 1; k < strlen(cmdArg); k++) {
            if (cmdArg[k] == 'h')
                return false;
            CmdArgument* arg = _argOptionMap[cmdArg[k]];
            if (arg == NULL) {
                std::cout << "Unsupported option '" << cmdArg[k] << "'" << std::endl;
                return false;
            }
            if (arg->getIsMandatory()) m++;
            if (arg->getRequireArgument()) {
                if (k < strlen(cmdArg) - 1 ) {
                    arg->setValue(&cmdArg[++k]);
                    break;
                }
                else if (i < argc - 1) {
                    arg->setValue(argv[++i]);
                }
                else {
                    std::cout << "Missing parameter for option '-" << cmdArg[k] << "'" << std::endl;
                    return false;
                }
            }
            else {
                //Assumes bool parameter. Set value to true
                arg->setValue("true");
            }
        }
    }
    _processedCmd = true;
    return (_totalMandatoryArgs == m);

}

CmdArgument* CmdLineHandler::getArgumentByName(const std::string arg_name)
{
    CmdArgument* arg = _argNameMap[arg_name];
    return arg;
}


int CmdLineHandler::getIntValue( std::string arg_name)
{
    CmdArgument* arg = getArgumentByName(arg_name);
    if (arg != NULL)
        return arg->getIntValue();
    return -1;
}

bool CmdLineHandler::getBoolValue(const std::string arg_name)
{
    CmdArgument* arg = getArgumentByName(arg_name);
    if (arg != NULL)
        return arg->getBoolValue();
    return false;

}

std::string CmdLineHandler::getStringValue(const std::string arg_name)
{
    CmdArgument* arg = getArgumentByName(arg_name);
    if (arg != NULL)
        return arg->getStringValue();
    return "";
}


ValueList CmdLineHandler::getValueList(const std::string arg_name)
{

    CmdArgument* arg = getArgumentByName(arg_name);
    if (arg != NULL)
        return arg->getValueList();
    return EMPTY_LIST;
}
CmdLineHandler::~CmdLineHandler()
{
}

std::ostream& operator<<(std::ostream& s, const CmdArgument& cmdArg)
{
    s << "  -" << cmdArg._argChar << " <" << cmdArg._argName << ">";
    if (cmdArg._value.size() > 0)
        s << " [" << cmdArg._value[0] << "]";
	std::streamsize width = s.width();
	s.width(CmdArgument::maxDescription - cmdArg._upToDescription + 4);
	char f = s.fill(' ');
	s.flags(std::ios::left);
    s << " : ";
    s.width(width);
    s.fill(f);
    s << cmdArg._description;
    return s;
}

std::ostream& operator<<(std::ostream& s, CmdLineHandler& cmdLine)
{

    s << "Usage: " << cmdLine.getProgramName() <<
        " [-" << cmdLine.getOptString() << "] " << cmdLine.getUsage() << std::endl;
    s << cmdLine.getDescription() << std::endl << std::endl;
    s << "Arguments:" << std::endl;

    for (ArgumentOptionMap::iterator it = cmdLine._argOptionMap.begin(); it != cmdLine._argOptionMap.end(); it++) {
        if (it->second == 0) continue;
        if ((it->second)->getIsMandatory())
            s << *(it->second) << std::endl;
    }

    s << "Optional Arguments:" << std::endl;
    for (ArgumentOptionMap::iterator it = cmdLine._argOptionMap.begin(); it != cmdLine._argOptionMap.end(); it++) {
        if (it->second == 0) continue;
        if (!(it->second)->getIsMandatory())
            s << *(it->second) << std::endl;
    }
    return s;
}
}//namespace LimeBrokerage
