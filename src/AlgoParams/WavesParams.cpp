//============================================================================
// Name        	: WavesParams.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2024 Katana Financial
// Date			: Mar 27, 2024 5:44:11 PM
//============================================================================

#include <AlgoParams/WavesParams.hpp>


WavesParams WavesParams::parse(const std::string &json_str)
{
	WavesParams cp = {};
	cp.ValidJson = false;

	cp.init();

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

		cp.meta.Enabled = (*itr)["Enabled"].GetInt();

		cp.meta.UniqueId = (*itr)["UniqueId"].GetString();
		cp.meta.Strategy = (*itr)["Strategy"].GetString();
		cp.meta.TemplateName = (*itr)["TemplateName"].GetString();
		cp.meta.Source = (*itr)["Source"].GetString();
		cp.meta.Tag = (*itr)["Tag"].GetString();

		cp.Product = (*itr)["Product"].GetString();

		cp.StartMonth = (*itr)["StartMonth"].GetString();
		cp.StartYear = (*itr)["StartYear"].GetInt();
		cp.NumMonths = (*itr)["NumMonths"].GetInt();

		cp.TotalQty = (*itr)["TotalQty"].GetDouble();
		cp.OrderQty = (*itr)["OrderQty"].GetDouble();

		break;
	}

	return cp;

}

