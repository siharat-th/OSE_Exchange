/*
 * CmdLineHandler.hh
 *
 *  Created on: May 13, 2010
 *      Author: val
 */

#ifndef CMDLINEHANDLER_HH_
#define CMDLINEHANDLER_HH_
#include <string>
#include <map>
#include <vector>

namespace LimeBrokerage {

typedef std::vector<std::string> ValueList;

class CmdArgument {
public:
	CmdArgument(const char arg_char, const char* arg_name,
			bool require_argument, bool is_mandatory,
			const char* description, const char* default_value = "");

    CmdArgument(const char arg_char, const char* arg_name,
            bool require_argument, bool is_mandatory,
            const char* description, bool repeating);


	int getIntValue() const;

	bool getBoolValue() const;

	std::string getStringValue() const;

	ValueList getValueList() const;

	const std::string operator() () const;

	friend std::ostream& operator<<(std::ostream& s, const CmdArgument& line);
	bool isOptionSet() { return _optionExist; }
	void setValue(const char* arg_value)
	{
	    if (_repeating) {
	        _value.push_back(arg_value);
	    }
	    else {
	        _value[0]=arg_value;
	    }
	    _optionExist = true;
	}

	const std::string getValue()
	{
		return _value.front();
	}

    char getArgChar() const
    {
        return _argChar;
    }

    std::string getArgName() const
    {
        return _argName;
    }

    std::string getDescription() const
    {
        return _description;
    }

    bool getIsMandatory() const
    {
        return _isMandatory;
    }

    bool getRequireArgument() const
    {
        return _requireArgument;
    }


private:
	char _argChar;

	std::string _argName;

	bool _requireArgument;

	bool _isMandatory;

	std::string _description;

	ValueList _value;

	bool _repeating;

	int _upToDescription;

	bool _optionExist;
	static int maxDescription;
	static int upToLimit;

	CmdArgument() {}
};

typedef std::map<char, CmdArgument*> ArgumentOptionMap;

typedef std::map<std::string, CmdArgument*> ArgumentNameMap;

typedef std::vector<std::string> VectorString;

class CmdLineHandler {
public:
	CmdLineHandler(CmdArgument argList[], int argSize, const char* description, const char* usage);

	static CmdArgument* printUsage;

	bool processCmd(int argc, char** argv);

	CmdArgument* getArgumentByName(const std::string arg_name) ;

	int getIntValue(const std::string arg_name) ;

	bool getBoolValue(const std::string arg_name) ;

	std::string getStringValue(const std::string arg_name) ;

	ValueList getValueList(const std::string arg_name);

	virtual ~CmdLineHandler();

	friend std::ostream& operator<<(std::ostream& s, CmdLineHandler& line);

	std::string getProgramName() const
	{
		return _programName;
	}
    std::string getDescription() const
    {
        return _description;
    }

    std::string getGetoptString() const
    {
        return _getoptString;
    }

    std::string getOptString() const
    {
        return _optString;
    }

    bool getProcessedCmd() const
    {
        return _processedCmd;
    }

    int getTotalMandatoryArgs() const
    {
        return _totalMandatoryArgs;
    }

    std::string getUsage() const
    {
        return _usage;
    }

    const VectorString& getMandatoryAttributes() const
    {
    	return _mandatoryAttributes;
    }

    const ArgumentOptionMap& getArgumentOptionMap() const
    {
    	return _argOptionMap;
    }
    const ArgumentNameMap& getArgumentNameMap() const
    {
        return _argNameMap;
    }

    bool isOptionSet(std::string arg) { return _argNameMap[arg]->isOptionSet() ; }
private:

    const static ValueList EMPTY_LIST;

	bool _processedCmd;

	std:: string _programName;

	std::string _description;

	std::string _usage;

    int _totalMandatoryArgs;

	std::string _getoptString;

	std::string _optString;


	ArgumentOptionMap _argOptionMap;

	ArgumentNameMap _argNameMap;

	VectorString _mandatoryAttributes;

	CmdLineHandler() {}



};
}//namespace LimeBrokerage
#endif /* CMDLINEHANDLER_HH_ */
