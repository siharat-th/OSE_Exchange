/*
 * Menu.cpp
 *
 *  Created on: Sep 29, 2016
 *      Author: sgaer
 */

#include <Menu/Menu.hpp>

Menu::Menu(MenuCallback & callback, std::map<int, std::string> menuChoices, std::string name)
: m_name(name),m_iChoice(0),m_Callback(callback),m_menuChoices(std::move(menuChoices))
{
	m_menuChoices[999] = "EXIT";  // Ensure exit is always an option
}

Menu::~Menu()
{

}

void Menu::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void Menu::DisplayAndWait()
{
	CreateAndRun(m_name, 1);
}
void Menu::Run()
{
    LogMe("Menus Loaded!");

    int choice = 0;
    bool menushown = false;

    while (true)
    {
        if (!menushown)
            LogMe("Enter 0 to Show Menu or Enter Choice: ", LogLevel::INFO);

        if (std::cin.eof()) {
            LogMe("Input stream ended. Exiting gracefully.", LogLevel::ERROR);
            break;
        }

        while (!(std::cin >> choice) || choice < 0)
        {
            if (std::cin.eof()) {
                LogMe("Input stream ended. Exiting gracefully.", LogLevel::ERROR);
                return;
            }

            cout<< "[Menu] Error: Bad input - try again: " << endl;
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        if (choice == 0)
        {
        	cout << "--------------- Menu ---------------" << endl;
            for (const auto &itt : m_menuChoices)
            {
                std::ostringstream oss;
                oss << "[Menu] " << itt.first << ":" << itt.second;
                cout << oss.str() << endl;
            }

            cout << "------------------------------------" << endl;

            menushown = true;
        }
        else
        {
            m_iChoice = choice;
            m_Callback.onMenuChoice(choice);
            menushown = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}


int Menu::Choice()
{
	return m_iChoice;
}

