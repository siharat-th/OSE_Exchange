//============================================================================
// Name        	: CmeSettingsHelper.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Oct 27, 2023 9:12:23 AM
//============================================================================

#ifndef SRC_CORE_CMESETTINGSHELPER_HPP_
#define SRC_CORE_CMESETTINGSHELPER_HPP_

#include <string>

#include <KT01/Core/StringSplitter.hpp>
#include <KT01/Core/Settings.hpp>

/************************
 *
 * Q&D Helper Class to quickly retrieve a singluar setting value from the Cme File
 *
 *************************/
class CmeSettingsHelper {
public:
	/**
  * Retrieves a boolean setting value from the Cme File.
  * @param key The key of the setting.
  * @return The boolean value of the setting.
  */
	static bool GetBool(const string& key)
	{
		bool value = false;
		Settings sett = getcmesett();
		value = sett.getBoolean(key);
		return value;
	}

	/**
  * Retrieves an integer setting value from the Cme File.
  * @param key The key of the setting.
  * @return The integer value of the setting.
  */
	static int GetInt(const string& key)
	{
		int value = 0;
		Settings sett = getcmesett();
		value = sett.getInteger(key);
		return value;
	}

	/**
  * Retrieves a string setting value from the Cme File.
  * @param key The key of the setting.
  * @return The string value of the setting.
  */
	static string GetString(const string& key)
	{
		string value = "";
		Settings sett = getcmesett();
		value = sett.getString(key);
		return value;
	}

	/**
  * Retrieves a long setting value from the Cme File.
  * @param key The key of the setting.
  * @return The long value of the setting.
  */
	static long GetLong(const string& key)
	{
		long value = 0;
		Settings sett = getcmesett();
		value = sett.getLong(key);
		return value;
	}


private:
	/**
  * Retrieves the Cme settings from the settings file.
  * @return The Cme settings.
  */
	static Settings getcmesett()
	{
		Settings settExch;

		string mainset = "Settings.txt";
		Settings settings(mainset);

		std::vector<std::string> exchanges;
		std::string exchlist = settings.getString("Exchanges");
		exchanges = StringSplitter::Split(exchlist, ",");

		string settingsfile = "";

		for (auto filename : exchanges)
		{
			Settings temp(filename);

			string exch = temp.getString("Exchange.Name");
			string proto = temp.getString("Exchange.Protocol");

			if (exch == "cme"){
				settExch = temp;
				break;
			}
		}

		return settExch;
	}
};

#endif /* SRC_CORE_SETTINGSHELPER_HPP_ */
