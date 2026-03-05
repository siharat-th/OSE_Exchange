// "commandLine.hh" - Command line parsing functionality
//
// Copyright 2008-2012 Lime Brokerage LLC. All rights reserved.
//

#ifndef COMMAND_LINE_HH
#define COMMAND_LINE_HH

// Standard C++ headers.
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <vector>

class CommandLine {
public:
    CommandLine(const std::string& _name, const std::string& _parameterDesc, const std::string& _info);
    ~CommandLine();

    class OptionBase {
    public:
        ~OptionBase() {};

        std::string getLongName() const { return longName; }
        char getShortName() const { return shortName; }
        int getID() const { return id; }
        std::string getValueType() const { return valueType; }
        std::string getDescription() const { return description; }
    protected:
        OptionBase(const std::string& _longName, const std::string& _valueType, const std::string& _description, const char _shortName) :
            longName(_longName),
            shortName(_shortName),
            id(getCurrentID()),
            valueType(_valueType),
            description(_description)
        {
        }
        OptionBase(const std::string& _longName, const std::string& _valueType, const std::string& _description) :
            longName(_longName),
            shortName(0),
            id(getCurrentID()),
            valueType(_valueType),
            description(_description)
        {
        }
    private:
        const std::string longName;
        const char shortName;
        const int id;
        const std::string valueType;
        const std::string description;
    };

    class FlagOption : public OptionBase {
    public:
        FlagOption(const std::string& longName, const std::string& description, const char shortName) :
            OptionBase(longName, "", description, shortName)
        {
        }
        FlagOption(const std::string& longName, const std::string& description) :
            OptionBase(longName, "", description)
        {
        }
        ~FlagOption()
        {
        }
    };

    template <class T> class Option : public OptionBase {
    public:
        Option(const std::string& longName, const std::string& valueType, const std::string& description, const char shortName) :
            OptionBase(longName, valueType, description, shortName)
        {
        }
        Option(const std::string& longName, const std::string& valueType, const std::string& description) :
            OptionBase(longName, valueType, description)
        {
        }
        ~Option()
        {
        }
    };

    template <class T> class MultipleOption : public OptionBase {
    public:
        MultipleOption(const std::string& longName, const std::string& valueType, const std::string& description, const char shortName):
            OptionBase(longName, valueType, description, shortName)
        {
        }
        MultipleOption(const std::string& longName, const std::string& valueType, const std::string& description):
            OptionBase(longName, valueType, description)
        {
        }
        ~MultipleOption() {}
    };

    class CommandLineExn : public std::exception {
    public:
        CommandLineExn() {}
        CommandLineExn(const std::string& msg): txt(msg) {}
        virtual ~CommandLineExn() throw() {}
        virtual const char* what() const throw() { return txt.c_str(); };
    protected:
        std::string txt;
    };

    class MissingOptionExn : public CommandLineExn {
    public:
        MissingOptionExn(const OptionBase&);
        virtual ~MissingOptionExn() throw() {}
    };

    class MissingOptionValueExn : public CommandLineExn {
    public:
        MissingOptionValueExn(const OptionBase&);
        virtual ~MissingOptionValueExn() throw() {}
    };

    class UnknownOptionExn : public CommandLineExn {
    public:
        UnknownOptionExn(const std::string&);
        virtual ~UnknownOptionExn() throw() {}
    };

    class BadOptionValueExn : public CommandLineExn {
    public:
        BadOptionValueExn(const OptionBase&, const std::string&);
        virtual ~BadOptionValueExn() throw() {}
    };

    class MissingParameterExn : public CommandLineExn {
    public:
        MissingParameterExn(const unsigned int);
        virtual ~MissingParameterExn() throw() {}
    };

    class BadParameterExn : public CommandLineExn {
    public:
        BadParameterExn(const unsigned int, const std::string&);
        virtual ~BadParameterExn() throw() {}
    };

    void addOption(const FlagOption& option)
    { parsers.insert(std::make_pair(option.getID(), new FlagParser(option))); }

    template <class T> void addOption(const Option<T>& option, bool isRequired = false)
    { parsers.insert(std::make_pair(option.getID(), new SingleParser<T>(option, isRequired))); }

    template <class T> void addOptionWithDefault(const Option<T>& option, const T& defaultVal)
    { parsers.insert(std::make_pair(option.getID(), new SingleParser<T>(option, defaultVal))); }

    template <class T> void addOption(const MultipleOption<T>& option, bool isRequired = false)
    { parsers.insert(std::make_pair(option.getID(), new MultipleParser<T>(option, isRequired))); }

    void parseCommandLine(int argc, char** argv);

    bool isOptionSet(const OptionBase& option) const
    { return (getParser(option) != NULL); }

    template <class T> T getOptionValue(const Option<T>& option) const
    {
        SingleParser<T>* p = static_cast<SingleParser<T>*>(getParser(option));
        if (p == NULL) throw MissingOptionExn(option);
        else return p->getValue();
    }

    template <class T> void getOptionValue(const Option<T>& option, T& out) const
    {
        SingleParser<T>* p = static_cast<SingleParser<T>*>(getParser(option));
        if (p == NULL) throw MissingOptionExn(option);
        else p->getValue(out);
    }

    template <class T, class OutputContainer>
    void getOptionValue(const MultipleOption<T>& option, OutputContainer& output) const
    {
        MultipleParser<T>* p = static_cast<MultipleParser<T>*>(getParser(option));
        if (p == NULL) throw MissingOptionExn(option);
        else p->getValues(output);
    }

    unsigned int getParameterCount() const { return parameters.size(); }

    template <class T> T getParameter(const unsigned int index) const
    {
        if (index >= parameters.size())
            throw MissingParameterExn(index);
        T temp;
        std::istringstream iss(parameters[index]);
        if (!(iss >> temp))
            throw BadParameterExn(index, parameters[index]);
        else
            return temp;
    }

    template <class T> void getParameter(const unsigned int index, T& out) const
    {
        if (index >= parameters.size())
            throw MissingParameterExn(index);
        std::istringstream iss(parameters[index]);
        if (!(iss >> out))
            throw BadParameterExn(index, parameters[index]);
    }

    template <class T, class OutputIterator>
    void getParameters(OutputIterator output, const unsigned int begin = 0) const
    { getParameters<T, OutputIterator>(output, begin, parameters.size()); }

    template <class T, class OutputIterator> void getParameters(OutputIterator output,
                                                                const unsigned int begin,
                                                                const unsigned int end) const
    {
        for (int i = begin; i < end; ++i) {
            if (i >= parameters.size())
                throw MissingParameterExn(i);
            std::istringstream iss(parameters[i]);
            T temp;
            if (!(iss >> temp))
                throw BadParameterExn(i, parameters[i]);
            else
                *output++ = temp;
        }
    }

    void doUsage(std::ostream&) const;
private:
    class ParserBase {
    public:
        ParserBase(const OptionBase& _option, const bool _isRequired, const std::string& _defaultStr) :
            option(_option),
            isSet(false),
            isRequired(_isRequired),
            defaultStr(_defaultStr)
        {
        }
        virtual ~ParserBase() {}

        virtual void parse(const char* str) = 0;

        virtual bool isFlag() const { return false; }
        virtual bool isMultiple() const { return false; }

        const OptionBase& getOption() const { return option; }

        struct option getLongOption() const;
        std::string getShortOption() const;

        bool getIsSet() const { return isSet; }
        bool getIsRequired() const { return isRequired; }

        std::string getDefault() const { return defaultStr; }
    protected:
        template <class T> static std::string toStr(const T& val)
        { std::ostringstream temp; temp << val; return temp.str(); }

        const OptionBase option;
        bool isSet;
        const bool isRequired;
        const std::string defaultStr;
    };

    class FlagParser : public ParserBase {
    public:
        FlagParser(const FlagOption& _option): ParserBase(_option, false, "") {}
        virtual ~FlagParser() {}

        virtual void parse(const char* str) { isSet = true; }

        virtual bool isFlag() const { return true; }
    };

    template <class T> class SingleParser : public ParserBase {
    public:
        SingleParser(const Option<T>& _option, bool isRequired):
            ParserBase(_option, isRequired, "")
        {
        }
        SingleParser(const Option<T>& _option, const T& defaultVal):
            ParserBase(_option, false, toStr(defaultVal)),
            value(defaultVal)
        {
            isSet = true;
        }

        virtual ~SingleParser() {}

        virtual void parse(const char* str)
        {
            std::istringstream iss(str);
            if (!(iss >> value))
                throw BadOptionValueExn(option, str);
            isSet = true;
        }

        T getValue() const { return value; }
        void getValue(T& outVal) const { outVal = value; }
    private:
        T value;
    };

    template <class T> class MultipleParser : public ParserBase {
    public:
        MultipleParser(const MultipleOption<T>& _option, bool isRequired):
            ParserBase(_option, isRequired, "") {}
        virtual ~MultipleParser() {}

        virtual void parse(const char* input)
        {
            std::string str = input;
            std::string::size_type lastPos = str.find_first_not_of(',', 0);
            std::string::size_type pos = str.find_first_of(',', lastPos);
            while (std::string::npos != pos || std::string::npos != lastPos) {
                insert(str.substr(lastPos, pos-lastPos));
                lastPos = str.find_first_not_of(',', pos);
                pos = str.find_first_of(',', lastPos);
            }
        }

        virtual bool isMultiple() const { return true; }

        template <class OutputContainer> void getValues(OutputContainer& output) const {
            typename std::list<T>::const_iterator first = list.begin();
            typename std::list<T>::const_iterator last = list.end();
            while (first!=last) {
                output.push_back(*first);
                ++first;
            }
        }
    private:
        void insert(const std::string& str)
        {
            T temp;
            std::istringstream iss(str);
            if (!(iss >> temp))
                throw BadOptionValueExn(option, str);
            list.push_back(temp);
            isSet = true;
        }

        std::list<T> list;
    };

    ParserBase* getParser(const OptionBase&) const;

    typedef std::map<int, ParserBase*> ParserMap;
    ParserMap parsers;

    typedef std::vector<std::string> ParameterList;
    ParameterList parameters;

    const std::string name;
    const std::string parameterDescription;
    const std::string info;

    static int currentID;
    static int getCurrentID();

    static FlagOption helpOption;
};

#endif
