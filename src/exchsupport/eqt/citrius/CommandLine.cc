// "commandLine.cc" - Command line parsing functionality
//
// Copyright 2008-2012 Lime Brokerage LLC. All rights reserved.
//

#include "CommandLine.hh"

// Standard C++ headers.
#include <cstdlib>
#include <iomanip>
#include <iostream>

// Third-party headers.
extern "C" {
#include <getopt.h>
}

int CommandLine::currentID = 256;

CommandLine::FlagOption CommandLine::helpOption("help", "Provide program usage information", 'h');

CommandLine::CommandLine(const std::string& _name, const std::string& _parameterDesc, const std::string& _info):
        name(_name),
        parameterDescription(_parameterDesc),
        info(_info)
{ addOption(helpOption); }

CommandLine::~CommandLine()
{
    for (ParserMap::iterator iter = parsers.begin(); iter != parsers.end(); ++iter)
        delete (*iter).second;
}

int CommandLine::getCurrentID()
{
    return ++currentID;
}

CommandLine::MissingOptionExn::MissingOptionExn(const OptionBase& option)
{
    std::ostringstream temp;
    temp << "Option \"--" << option.getLongName() << "\" ";
    if (option.getShortName() != 0)
        temp << "(-" << option.getShortName() << ") ";
    temp << "is not specified!" << std::endl;

    txt += temp.str();
}

CommandLine::MissingOptionValueExn::MissingOptionValueExn(const OptionBase& option)
{
    std::ostringstream temp;
    temp << "Option \"--" << option.getLongName() << "\" ";
    if (option.getShortName() != 0)
        temp << "(-" << option.getShortName() << ") ";
    temp << "is missing a required value!" << std::endl;

    txt += temp.str();
}

CommandLine::BadOptionValueExn::BadOptionValueExn(const OptionBase& option, const std::string& argument)
{
    std::ostringstream temp;
    temp << "Invalid value passed to option \"--" << option.getLongName() << "\"";
    if (option.getShortName() != 0)
        temp << " (-" << option.getShortName() << ")";
    temp << "! Option Value = \"" << argument << "\"" << std::endl;

    txt += temp.str();
}

CommandLine::MissingParameterExn::MissingParameterExn(const unsigned int index)
{
    std::ostringstream temp;
    temp << "Parameter #" << index+1 << " is not present!" << std::endl;
    txt += temp.str();
}

CommandLine::BadParameterExn::BadParameterExn(const unsigned int index, const std::string& parameter)
{
    std::ostringstream temp;
    temp << "Parameter #" << index+1 << " is invalid! Parameter Value = \"" << parameter << "\"" << std::endl;
    txt += temp.str();
}

CommandLine::UnknownOptionExn::UnknownOptionExn(const std::string& option)
{
    std::ostringstream temp;
    temp << "Unknown option \"" << option << "\" specified" << std::endl;
    txt += temp.str();
}

struct option CommandLine::ParserBase::getLongOption() const
{
    struct option temp = { option.getLongName().c_str(),
                           isFlag() ? no_argument : required_argument,
                           NULL, option.getID() };
    return temp;
}

std::string CommandLine::ParserBase::getShortOption() const
{
    if (option.getShortName() != 0) {
        std::ostringstream temp;
        temp << option.getShortName();
        if (!isFlag())
            temp << ":";
        return temp.str();
    } else
        return "";
}

void CommandLine::parseCommandLine(int argc, char** argv)
{
    std::vector<struct option> longOptions;
    for (ParserMap::iterator iter = parsers.begin(); iter != parsers.end(); ++iter)
        longOptions.push_back((*iter).second->getLongOption());

    struct option nullOption = { 0, 0, 0, 0 };
    longOptions.push_back(nullOption);

    std::map<char, unsigned int> shortNameToID;

    std::ostringstream shortOptionsStream;
    shortOptionsStream << '+';
    for (ParserMap::iterator iter = parsers.begin(); iter != parsers.end(); ++iter) {
        char shortName = (*iter).second->getOption().getShortName();
        if (shortName != 0) {
            shortOptionsStream << (*iter).second->getShortOption();
            shortNameToID.insert(std::make_pair(shortName, (*iter).second->getOption().getID()));
        }
    }

    std::string shortOptionsStr = shortOptionsStream.str();

    opterr = 0;

    int indexPtr = 0;
    while (1) {
        int val = getopt_long(argc, argv, shortOptionsStr.c_str(),
                              &(*longOptions.begin()), &indexPtr);

        if (val == -1)
            break;
        else if (val == '?') {
            if (optopt == 0)
                throw UnknownOptionExn(argv[optind-1]);
            else {
                ParserMap::iterator iter = parsers.find(optopt);
                if (iter == parsers.end())
                    throw UnknownOptionExn(argv[optind-1]);
                else
                    throw MissingOptionValueExn((*iter).second->getOption());
            }
        } else if (val < 256) {
            if (val == helpOption.getShortName()) {
                doUsage(std::cerr);
                std::exit(-1);
            }

            std::map<char, unsigned int>::iterator iter = shortNameToID.find(val);
            if (iter != shortNameToID.end()) {
                unsigned int id = (*iter).second;
                ParserMap::iterator iter2 = parsers.find(id);
                if (iter2 != parsers.end())
                    (*iter2).second->parse(optarg);
            }
        } else {
            if (val == helpOption.getID()) {
                doUsage(std::cerr);
                std::exit(-1);
            }

            ParserMap::iterator iter = parsers.find(val);
            if (iter != parsers.end())
                (*iter).second->parse(optarg);
        }
    }

    for (ParserMap::const_iterator iter = parsers.begin(); iter != parsers.end(); ++iter) {
        ParserBase* p = (*iter).second;
        if (p->getIsRequired() && !p->getIsSet())
            throw MissingOptionExn(p->getOption());
    }

    parameters.clear();
    for (int i = optind; i < argc; ++i)
        parameters.push_back(argv[i]);
}

CommandLine::ParserBase* CommandLine::getParser(const OptionBase& option) const
{
    ParserMap::const_iterator iter = parsers.find(option.getID());
    if (iter == parsers.end())
        return NULL;
    ParserBase* p = (*iter).second;
    return p->getIsSet() ? p : NULL;
}

void CommandLine::doUsage(std::ostream& os) const
{
    size_t maxSize = 0;
    for (ParserMap::const_iterator iter = parsers.begin(); iter != parsers.end(); ++iter) {
        std::ostringstream temp;
        ParserBase* parser = (*iter).second;
        if (parser->getOption().getLongName().size() > 0)
            temp << "--" << parser->getOption().getLongName();
        if ((parser->getOption().getLongName().size() > 0) &&
                (parser->getOption().getShortName() != 0))
            temp << ",";
        if (parser->getOption().getShortName() != 0)
            temp << " -" << parser->getOption().getShortName();
        if (parser->getOption().getValueType().size() > 0)
            temp << " <" << parser->getOption().getValueType() << ">";
        maxSize = std::max(maxSize, temp.str().size());
    }

    os << info << std::endl << std::endl;

    os << "Usage: " << name << " [options] " << parameterDescription << std::endl << std::endl;

    os << "Options:" << std::endl;
    for (ParserMap::const_iterator iter = parsers.begin(); iter != parsers.end(); ++iter) {
        ParserBase* parser = (*iter).second;
        os << std::setfill(' ');

        std::ostringstream temp;
        if (parser->getOption().getLongName().size() > 0)
            temp << "--" << parser->getOption().getLongName();
        if ((parser->getOption().getLongName().size() > 0) &&
                (parser->getOption().getShortName() != 0))
            temp << ",";
        if (parser->getOption().getShortName() != 0)
            temp << " -" << parser->getOption().getShortName();
        if (parser->getOption().getValueType().size() > 0)
            temp << " <" << parser->getOption().getValueType() << ">";
        os << std::setw(maxSize+4) << std::left << temp.str();

        os << " " << parser->getOption().getDescription();
        if (parser->getIsRequired())
            os << " [REQUIRED]";

        if (parser->isMultiple())
            os << " [MULTIPLE]";

        if (parser->getDefault().size() > 0)
            os << " (Default: \"" << parser->getDefault() << "\")";

        os << std::endl;
    }
}

