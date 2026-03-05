#pragma once

#include <KT01/Core/Settings.hpp>

#include <string>
#include <vector>

#include <KT01/Core/StringSplitter.hpp>

/**
 * @brief The DataProcSettings class represents the settings for data processing.
 */
class DataProcSettings : public Settings
{
public:
    /**
     * @brief Constructs a DataProcSettings object.
     */
    DataProcSettings();

    /**
     * @brief Destroys the DataProcSettings object.
     */
    ~DataProcSettings();

    /**
     * @brief Initializes the DataProcSettings object.
     */
    void Init();

public:
    std::vector<std::string> RootSymbols; /**< The root symbols. */
    std::vector<std::string> Channels; /**< The channels. */
    std::vector<std::string> Feeds; /**< The feeds. */

    // EQT Connection Settings
    std::string ConnectionServer;
    std::string ConnectionUsername;
    std::string ConnectionPassword;
    std::string ConnectionQuoteSource;

    // EQT Feature Flags
    bool FeatureExpress = false;
    bool FeatureBlockingLogin = true;
    bool FeatureTopOfBook = false;
    bool FeatureAggregateByPrice = false;
    bool FeatureExtendedQuote = false;
    bool FeatureTradesOnly = false;
    bool FeatureSnapshotBook = false;
    bool FeatureSubscribeAll = false;
    bool FeatureSubscribeFromSecdef = false;

    // EQT Run Settings
    int RunInterval = -1;
    bool RunPrintData = true;
};
