#ifndef CSMSETTINGS_H_
#define CSMSETTINGS_H_

#pragma once
#include <KT01/Core/Settings.hpp>


using namespace std;

/**
 * @brief The CmeMdpSettings class represents the settings specific to CME Market Data and network settings
 * 
 * This class inherits from the base Settings class.
 */
class CmeMdpSettings : public Settings
{
public:
    /**
     * @brief Default constructor for CmeMdpSettings.
     */
    CmeMdpSettings();

    /**
     * @brief Destructor for CmeMdpSettings.
     */
    ~CmeMdpSettings();

    /**
     * @brief Initializes the CmeMdpSettings object with the specified file.
     * 
     * @param File The file to initialize the settings from.
     */
    void Init(string File);

public:
    int MsgBuffSize; /**< The size of the message buffer. */

    string ConfigXmlFileDir;/**< The directory where the configuration xml file is located. */
    string ConfigXmlFile;/**< The name of the configuration xml file */

    std::string InterfaceA; /**< The name of interface A. */
    std::string InterfaceB; /**< The name of interface B. */
    std::string InterfaceNameA; /**< The name of interface A. */
    std::string InterfaceNameB; /**< The name of interface B. */
    
    int ThreadAffinity; /**< The thread affinity. */
    int TimeoutMS; /**< The timeout in milliseconds. */

    int PollWaitNs; /**< The poll wait time in nanoseconds. */

    int RxCapacity; /**< The capacity of the receive buffer. */
    int RxBatchSize; /**< The batch size for receiving messages. */

};

#endif /*CSMSETTINGS_H_*/
