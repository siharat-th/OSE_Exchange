//============================================================================
// Name        	: PLWatcherParams.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: May 24, 2024 1:15:57 PM
//============================================================================

#include <AlgoParams/PLWatcherParams.hpp>


PLWatcherParams PLWatcherParams::parse(const std::string &json_str)
{
	PLWatcherParams cp;
	cp.ValidJson = false;

	Document d;

	if (d.Parse(json_str.c_str()).HasParseError()) {
		int e = (unsigned)d.GetErrorOffset();
		ostringstream oss;
		cout << "JSON OFFSET ERROR AT " << e << endl;
		return cp;
	}

	if (!d.IsArray())
	{
		cout << "ALGO JSON OBJECT MUST BE AN ARRAY!!!!" << endl;
		return cp;
	}

	cp.ValidJson = true;


	for (Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr)
	{
		if (!(*itr).HasMember("GroupName") || !(*itr).HasMember("Org") || !(*itr).HasMember("User"))
		{
			cout << "ERROR: ALGO MUST CONTAIN USER/GROUPNAME/ORG!" << endl;
			return cp;
		}
		cp.meta.userinfo.groupname = (*itr)["GroupName"].GetString();
		cp.meta.userinfo.org = (*itr)["Org"].GetString();
		cp.meta.userinfo.user = (*itr)["User"].GetString();

		cp.meta.Enabled = true;

		cp.meta.UniqueId = (*itr)["UniqueId"].GetString();
		cp.meta.Strategy = (*itr)["Strategy"].GetString();
		cp.meta.TemplateName = (*itr)["TemplateName"].GetString();
		cp.meta.Source = (*itr)["Source"].GetString();
		cp.meta.Tag = (*itr)["Tag"].GetString();

		/*cp.MaxPl = (*itr)["MaxPl"].GetDouble();
		cp.MinPl = (*itr)["MinPl"].GetDouble();
		cp.TimeToLiveSecs = (*itr)["TimeToLiveSecs"].GetInt();
		cp.PrimaryLeg = (*itr)["TimeToLiveSecs"].GetInt();
		cp.SqueezeQty = (*itr)["SqueezeQty"].GetInt();
		cp.UseBarriers = true;// (*itr)["TimeToLiveSecs"].GetBool();
		cp.UseSqueeze = (*itr)["UseSqueeze"].GetBool();*/


		break;
	}

	return cp;

}

