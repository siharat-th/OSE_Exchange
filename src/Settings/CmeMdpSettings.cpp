
#include <Settings/CmeMdpSettings.h>


CmeMdpSettings::CmeMdpSettings()
{

}

CmeMdpSettings::~CmeMdpSettings()
{
}

void CmeMdpSettings::Init(string File)
{
	std::string szSettingsFile = File;

	Load(szSettingsFile,"CmeMdpConfig");

	try
	{
	    ConfigXmlFileDir= getString("ConfigXmlFileDir");
	    ConfigXmlFile 	= getString("ConfigXmlFile");

		MsgBuffSize		= getInteger("MsgBuffSize");

		InterfaceA		= getString("InterfaceA");
		InterfaceB		= getString("InterfaceB");
		InterfaceNameA	= getString("InterfaceNameA");
		InterfaceNameB	= getString("InterfaceNameB");

		ThreadAffinity	= getInteger("ThreadAffinity");
		TimeoutMS 		= getInteger("TimeoutMS");
		PollWaitNs  	= getInteger("PollWaitNs");
		RxCapacity 	= getInteger("RxCapacity");
		RxBatchSize = getInteger("RxBatchSize");
	}
	catch (std::exception& e)
	{
		std::string szWhat(e.what());
		cout << "MDP SETTINGS ERROR: " << e.what() << endl;
		exit(1);

		return;
	}
}



