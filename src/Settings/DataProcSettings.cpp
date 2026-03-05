#include "DataProcSettings.hpp"
#include <KT01/Core/StringSplitter.hpp>
#include <iostream>
using namespace std;

DataProcSettings::DataProcSettings()
{
}

DataProcSettings::~DataProcSettings()
{
}

void DataProcSettings::Init()
{
	std::string szSettingsFile = "DataSettings.txt";
	Load(szSettingsFile,"DATASETTINGS");

	try
	{
		std::string roots = getString("Data.RootSymbols");
		std::string channels = getString("Data.Channels");
		std::string feeds = getString("Data.Feeds");

		RootSymbols = StringSplitter::Split(roots, ",");
		Channels = StringSplitter::Split(channels, ",");
		Feeds = StringSplitter::Split(feeds, ",");

		// EQT Connection Settings
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Connection.Server", getString, ConnectionServer, std::string(""));
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Connection.Username", getString, ConnectionUsername, std::string(""));
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Connection.Password", getString, ConnectionPassword, std::string(""));
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Connection.QuoteSource", getString, ConnectionQuoteSource, std::string("INET"));

		// EQT Feature Flags
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Features.Express", getBoolean, FeatureExpress, false);
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Features.BlockingLogin", getBoolean, FeatureBlockingLogin, true);
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Features.TopOfBook", getBoolean, FeatureTopOfBook, false);
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Features.AggregateByPrice", getBoolean, FeatureAggregateByPrice, false);
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Features.ExtendedQuote", getBoolean, FeatureExtendedQuote, false);
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Features.TradesOnly", getBoolean, FeatureTradesOnly, false);
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Features.SnapshotBook", getBoolean, FeatureSnapshotBook, false);
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Features.SubscribeAll", getBoolean, FeatureSubscribeAll, false);
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Features.SubscribeFromSecdef", getBoolean, FeatureSubscribeFromSecdef, false);

		// EQT Run Settings
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Run.Interval", getInteger, RunInterval, -1);
		LOAD_TYPED_SETTING_OR_DEFAULT(*this, "Run.PrintData", getBoolean, RunPrintData, true);
	}
	catch (std::exception& e)
	{
		std::string szWhat(e.what());
		cout << "DATAPROC SETTINGS ERROR: " << e.what() << endl;
		exit(1);

		return;
	}
}

