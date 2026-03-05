
#include <KT01/Core/Settings.hpp>

Settings::Settings()
{
}

Settings::Settings(const std::string filename)
{
	m_Me = filename;
	m_FilePath = filename;

	Load(filename, filename);
}

Settings::~Settings(void)
{
}

[[noreturn]] void Settings::MissingFatal(const std::string &key) const
{
	const char* format = "[ERROR][FATAL] ****Missing required setting: '%s' in file '%s'\n";
	for(int i = 0; i < 10; ++i)
	{
		fprintf(stderr, format, key.c_str(), m_FilePath.c_str());
		fflush(stderr);
	}

	// Sleep a beat just in case pipes need time
	usleep(100 * 1000);  // 100ms

	_exit(1);  // skip destructors, avoids double free
}

void Settings::trimSpaces(string *str)
{
	const char WHITE_SPACE[] = " \t";
	size_t found = str->find_last_not_of(WHITE_SPACE);
	if (string::npos == found)
	{
		str->resize(0);
	}
	else
	{
		str->resize(found + 1);
	}
	found = str->find_first_not_of(WHITE_SPACE);
	if (found != string::npos && found != 0)
	{
		str->erase(0, found);
	}

	str->erase(std::remove(str->begin(), str->end(), '\r'), str->end());
}

void Settings::Load(const std::string &filePath, std::string szMe)
{
	m_Me = szMe;
	m_FilePath = filePath;

	ifstream in(filePath.c_str());
	if (!in)
	{
		std::cerr << "[FATAL] Cannot open the settings file: '" << filePath << endl;
		std::cerr.flush();  // force it out before abrupt exit
		std::exit(1);
	}

	string line;
	while (getline(in, line))
	{
		string::size_type comment = line.find('#');
		if (string::npos != comment)
		{
			line.resize(comment);
		}

		string::size_type separator = line.find('=');
		if (string::npos == separator)
		{
			continue;
		}
		string key = line.substr(0, separator);
		trimSpaces(&key);

		string value = line.substr(separator + 1);
		trimSpaces(&value);

		map_.insert(make_pair(key, value));
	}
}

void Settings::Print()
{
	std::cout << "Settings: " << m_Me << std::endl;
	for (Key2ValueMap::const_iterator it = map_.begin(); it != map_.end(); ++it)
	{
		std::cout << it->first << "=" << it->second << std::endl;
	}
}

std::string Settings::getString(const std::string &key) const
{
	Key2ValueMap::const_iterator it = map_.find(key);
	if (map_.end() != it)
	{
		return it->second;
	}

	MissingFatal(key);
}

int Settings::getInteger(const std::string &key) const
{
	Key2ValueMap::const_iterator it = map_.find(key);
	if (map_.end() != it)
	{
		return std::atoi(it->second.c_str());
	}

	MissingFatal(key);
}

long Settings::getLong(const std::string &key) const
{
	Key2ValueMap::const_iterator it = map_.find(key);
	if (map_.end() != it)
	{
		return std::atol(it->second.c_str());
	}

	MissingFatal(key);
}

double Settings::getDouble(const std::string &key) const
{
	Key2ValueMap::const_iterator it = map_.find(key);
	if (map_.end() != it)
	{
		return std::atof(it->second.c_str());
	}

	MissingFatal(key);
}

bool Settings::getBoolean(const std::string &key) const
{
	Key2ValueMap::const_iterator it = map_.find(key);
	if (map_.end() != it)
	{
		return strcasecmp("true", it->second.c_str()) == 0;
	}

	MissingFatal(key);
}

bool Settings::getBooleanSafe(const std::string &key) const
{
	Key2ValueMap::const_iterator it = map_.find(key);
	if (map_.end() != it)
	{
		return strcasecmp("true", it->second.c_str()) == 0;
	}

	return false;
}

vector<string> Settings::getListCsv(const std::string &key) const
{
	Key2ValueMap::const_iterator it = map_.find(key);
	if (map_.end() != it)
	{
		return StringSplitter::Split(it->second, ",");
	}

	MissingFatal(key);
}

bool Settings::hasKey(const std::string& key) const
{
	return map_.find(key) != map_.end();
}