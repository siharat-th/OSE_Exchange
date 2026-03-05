//============================================================================
// Name        	: AutoHedgeOB2Params.cpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Apr 20, 2023 5:47:44 PM
//============================================================================

#include <AlgoParams/SkewerParams.hpp>


SkewerParams SkewerParams::parse(const std::string &json_str)
{
	SkewerParams cp;
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


	for (Value::ConstValueIterator itr = d.Begin(); itr !=	 d.End(); ++itr)
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
		cp.meta.StratId = atoi(cp.meta.Strategy.c_str());
		cp.meta.TemplateName = (*itr)["TemplateName"].GetString();
		cp.meta.Source = (*itr)["Source"].GetString();
		cp.meta.Tag = (*itr)["Tag"].GetString();

		break;
	}

	return cp;

}


