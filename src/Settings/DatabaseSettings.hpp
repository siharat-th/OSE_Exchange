#ifndef DATABASESETTINGS_H_
#define DATABASESETTINGS_H_

#pragma once
#include <KT01/Core/Settings.hpp>

#include <iostream>
#include <string>

using namespace std;

class DatabaseSettings : public Settings
{
public:
    DatabaseSettings() {}
    ~DatabaseSettings() {}

    void Init(string File)
    {
        Load(File, "DBSETT");
        try
        {
            Environment = getString("Environment");
            Host = getString("Host");
            User = getString("User");
            Password = getString("Password");
            Db = getString("Db");
        }
        catch (std::exception& e)
        {
            cout << "DB SETTINGS ERROR: " << e.what() << endl;
            exit(1);
        }
    }

public:
    std::string Environment;
    std::string Host;
    std::string User;
    std::string Password;
    std::string Db;
};

#endif /* DATABASESETTINGS_H_ */
