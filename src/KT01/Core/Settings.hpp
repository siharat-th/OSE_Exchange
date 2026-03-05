#ifndef SRC_SETTINGS_HPP_
#define SRC_SETTINGS_HPP_
#pragma once

#include <sys/time.h>
#include <string>
#include <map>
#include <algorithm>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string.h>
#include <iostream>

#include <unistd.h>   // for _exit
#include <stdio.h>    // for fprintf, stderr, fflush

using namespace std;

#include <KT01/Core/StringSplitter.hpp>

#define LOAD_TYPED_SETTING_OR_DEFAULT(settings, key, getterFunc, var, defaultVal) \
    var = (settings).hasKey(key) ? (settings).getterFunc(key) : (defaultVal);

/// Settings collection.
class Settings
{
public:
    /**
     * Default constructor for the Settings class.
     */
    Settings();

    /**
     * Constructor for the Settings class that takes a filename.
     * 
     * @param filename The name of the file to load settings from.
     */
    Settings(const std::string filename);

    /**
     * Destructor for the Settings class.
     */
    ~Settings(void);

    /**
     * Load settings from a file.
     * 
     * @param filePath The path of the file to read settings from.
     * @param szMe The key to retrieve the value for.
     */
    void Load(const std::string& filePath, std::string szMe);

    /**
     * Print all the settings to the console.
     */
    void Print();


    /**
     * Get the value associated with the given key.
     * 
     * @param key The key to retrieve the value for.
     * @return The value associated with the key.
     */
    std::string getString(const std::string& key) const;


    /**
     * Get the integer value associated with the given key.
     * 
     * @param key The key to retrieve the value for.
     * @return The integer value associated with the key.
     */
    int getInteger(const std::string& key) const;

  
    /**
     * Get the long value associated with the given key.
     * 
     * @param key The key to retrieve the value for.
     * @return The long value associated with the key.
     */
    long getLong(const std::string& key) const;

    /**
     * Get the boolean value associated with the given key.
     * 
     * @param key The key to retrieve the value for.
     * @return The boolean value associated with the key.
     */
    bool getBoolean(const std::string& key) const;

    /**
     * Get the boolean value associated with the given key.
     * 
     * @param key The key to retrieve the value for.
     * @return The boolean value associated with the key, or false if the key is not found.
     */
	bool getBooleanSafe(const std::string& key) const;

    /**
     * Get the double value associated with the given key.
     * 
     * @param key The key to retrieve the value for.
     * @return The double value associated with the key.
     */
    double getDouble(const std::string& key) const;

    /**
     * Get the list of values associated with the given key, separated by commas.
     * 
     * @param key The key to retrieve the list for.
     * @return A vector of strings containing the values in the list.
     */
    vector<string> getListCsv(const std::string& key) const;

    /**
     * Get the map of all settings.
     * 
     * @return A map containing all the settings.
     */
    inline map<std::string, std::string> GetMap() { return map_; }

    /**
     * Trim leading and trailing spaces from the given string.
     * 
     * @param str The string to trim.
     */
    static void trimSpaces(std::string* str);

    /**
     * Check if the given key exists in the settings.
     *
     * @param key The key to check for.
     * @return True if the key exists, false otherwise.
     */
    bool hasKey(const std::string& key) const;

private:
[[noreturn]] void MissingFatal(const std::string& key) const;
    
protected:
    typedef std::map<std::string /* key */, std::string /* value */> Key2ValueMap;
    Key2ValueMap map_;

    std::string m_Me;
    std::string m_FilePath;
};

#endif
