/*
 * Menu.hpp
 *
 *  Created on: Sep 29, 2016 / 21st century revision Dec 25, 2023
 *      Author: sgaer
 */

#ifndef STANDARDMENU_H_
#define STANDARDMENU_H_

#pragma once
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <map>
#include <limits.h>
#include <pthread.h>

#include <algorithm>
#include <KT01/Core/StringSplitter.hpp>
#include <KT01/Core/NamedThread.hpp>

#include <KT01/Core/Log.hpp>

using namespace std;

#include <Menu/MenuCallback.h>

using namespace std;

/**
 * @brief The StandardMenu class represents a standard menu implementation.
 * 
 * This class inherits from the NamedThread class.
 * It provides functionality to display and interact with a menu.
 */
class Menu : public NamedThread
{
public:
    /**
     * @brief Constructs a new StandardMenu object.
     * 
     * @param callback The MenuCallback object to handle menu events.
     * @param menuChoices Map of choices for the menu to display
     * @param name The name of the menu.
     */
	Menu(MenuCallback& callback,std::map<int, std::string> menuChoices, std::string name);

    /**
     * @brief Destroys the StandardMenu object.
     */
    virtual ~Menu();

    /**
     * @brief Displays the menu and waits for user input.
     */
    void DisplayAndWait();

    /**
     * @brief Runs the menu in a separate thread.
     */
    void Run() override;

private:
    /**
     * @brief Logs a message with an optional color.
     * 
     * @param szMsg The message to log.
     * @param iColor The color of the log message (default: LOG_WHITE).
     */
    void LogMe(std::string szMsg, LogLevel::Enum loglevel = LogLevel::INFO);

    /**
     * @brief Prompts the user for a choice and returns the selected choice.
     * 
     * @return The selected choice.
     */
    int Choice();

    /**
     * @brief Prints the menu.
     */
    void print();

private:
    std::string m_name;             ///< The name of the menu.
    int m_iChoice;                  ///< The selected choice.
    pthread_t m_tid;                ///< The thread ID.
    MenuCallback& m_Callback;       ///< The MenuCallback object.
    std::map<int, std::string> m_menuChoices;  ///< Store menu choices
};

#endif /* STANDARDMENU_H_ */
