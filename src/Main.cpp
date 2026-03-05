
#include <Raptor2.hpp>
#include <external/cmdline/cxxopts.hpp>

#include <KT01/Core/Log.hpp>
#include <KT01/Core/Singleton.hpp>
#include <Rest/RestApi.hpp>
#include <Orders/OrderNums.hpp>
#include <Orders/OrderRecoveryService.hpp>
#include <KT01/SecDefs/CmeSecMasterV2.hpp>
#include <KT01/SecDefs/CfeSecMaster.hpp>
#include <KT01/SecDefs/EquitySecMaster.hpp>

using namespace KT01::SECDEF::CFE;
using namespace KT01::SECDEF::EQT;
using namespace KTN;



void Banner()
{
	 for (int i = 0; i < 4; i++)
		KT01_LOG_INFO("MAIN", "*********************************************");
	for (int i = 0; i < 1; i++)
		KT01_LOG_INFO("MAIN", "**********       AKL EDGE 1.0      **********");
	for (int i = 0; i < 4; i++)
		KT01_LOG_INFO("MAIN", "*********************************************");

}

int main(int argc, char* argv[])
{
	// Define options and help message.
	 try {
		// Define options and help message.
		cxxopts::Options options("Raptor", "Start order handler with either reset or negotiate mode");
		options.positional_help("settings_file");

		// Define options.
		options.add_options()
			("s,settings", "Settings file", cxxopts::value<std::string>())
			("h,help", "Show help");

		// Make "settings" a positional argument.
		options.parse_positional({"settings"});

		// Parse the command-line arguments.
		auto result = options.parse(argc, argv);

		// If help was requested or the settings file wasn't provided, show help.
		if (result.count("help") || !result.count("settings")) {
			std::cout << options.help() << std::endl;
			return 0;
		}

		// Retrieve the settings file name.
		std::string settingsFile = result["settings"].as<std::string>();

		Banner();


		// Initialize RestApi by creating its singleton instance.
		// The constructor automatically reads "Settings.txt" (or another file if specified).
		RestApi& api = Singleton<RestApi>::instance();
		(void)api; // Use api if needed; endpoints are static.
		KT01_LOG_INFO("MAIN", "REST API SERVER=" + RestApi::API_SERVER);
		
		// Initialize CmeSecMaster by creating its singleton instance.
		KT01_LOG_INFO("MAIN","Loading Global CFE Security Master Data.");
		CfeSecMaster& cfeSecMaster = CfeSecMaster::instance();

		KT01_LOG_INFO("MAIN", "Loading Global EQT Security Master Data.");
		EquitySecMaster& eqtSecMaster = EquitySecMaster::instance();

		ostringstream oss;
		oss << PRICE_PRECISION;
		KT01_LOG_WARN("MAIN", "Price precision is set to: ", oss.str());
    
		// Create a sample price to verify
		akl::Price price = akl::Price::FromUnshifted(123.456789);
		
		KT01_LOG_WARN("MAIN", "Original value: 123.456789");
		KT01_LOG_WARN("MAIN", "Stored as shifted: ", price.AsShifted());
		KT01_LOG_WARN("MAIN", "Retrieved as unshifted: ",price.AsUnshifted());

		// Initialize OrderNums by creating its singleton instance.
		// The constructor automatically loads order numbers.
		/*bool y =*/ OrderNums::instance().isLoaded();
		KT01_LOG_INFO("MAIN", "Order Ids Generated=" + std::to_string(OrderNums::instance().OrdIdSize()));
		KT01_LOG_INFO("MAIN", "OrderRequestIDs Generated=" + std::to_string(OrderNums::instance().ReqIdSize()));

		//Initialize the OrderRecoveryService so its now centralized
		//auto& ors = OrderRecoveryService::instance();
		OrderRecoveryService::instance().Init(settingsFile);
		OrderRecoveryService::instance().LoadFromApi();

		tbb::concurrent_unordered_map<uint64_t, KTN::Order> &apiOrders = OrderRecoveryService::instance().GetApiOrders();
		for (tbb::concurrent_unordered_map<uint64_t, KTN::Order>::iterator it = apiOrders.begin(); it != apiOrders.end(); ++it)
		{
			KTN::Order &ord = it->second;
			if (ord.OrdExch == KOrderExchange::CFE) // OrderReqId for CME orders should be set from order status response sent by the exchange
			{
				ord.orderReqId = OrderNums::instance().GenOrderReqId(ord.ordernum);
				LOGINFO("[MAIN] Setting reqId to {} for API order: ExchOrdId={} ClOrdId={}", ord.orderReqId, ord.exchordid, ord.ordernum);
			}
		}

		// Initialize CmeSecMaster by creating its singleton instance.
		KT01_LOG_INFO("MAIN", "Loading Global CME Security Master Data.");

		//CmeSecMaster& cmeSecMaster = CmeSecMaster::instance();
		//CmeSecDef x = CmeSecMaster::instance().getSecDef(0);

		

		KT01_LOG_INFO("MAIN", "SETTINGS FILE=" + settingsFile);

		KT01_LOG_INFO("MAIN", "SETTINGS FILE={}", settingsFile);
		Raptor2 raptor(settingsFile);
		raptor.WaitForShutdown();

	 }
	catch (const cxxopts::exceptions::exception& e) {
		std::cerr << "Error parsing options: " << e.what() << std::endl;
		return 1;
	}
    catch (const std::exception& ex)
    {
        std::cerr << "\nERROR: " << ex.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "\nUNKNOWN ERROR: " << std::endl;
        return 2;
    }
    return 0;
}
