//============================================================================
// Name        	: CmeMsgw.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 26, 2021 12:40:03 PM
//============================================================================

#ifndef SRC_CME_CMEMSGW_HPP_
#define SRC_CME_CMEMSGW_HPP_

#include<string>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <KT01/Core/Settings.hpp>
#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>
using namespace std;



namespace KT01{
namespace MSGW{

enum MsgwFields
{
	PrimaryIP,SecondaryIP,SegID,Description
};


class Msgw
{
public:
	int segid;
	string primip;
	string secip;
	string description;

	/**
  * @brief Returns a string representation of the Msgw object.
  * @return The string representation of the Msgw object.
  */
	string Print()
	{
		ostringstream oss;
		oss << "ID=" <<  segid << " DESC=" << description <<" PRIMIP=" << primip << " SECIP=" << secip;
		return oss.str();
	}

	/**
  * @brief Returns the full description of the Msgw object.
  * @return The full description of the Msgw object.
  */
	string FullDesc()
	{
		ostringstream oss;
		oss  << segid << ": " << description;
		return oss.str();
	}

	/**
  * @brief Returns the segment ID of the Msgw object as a string.
  * @return The segment ID of the Msgw object as a string.
  */
	string SegId()
	{
		ostringstream oss;
		oss  << segid ;
		return oss.str();
	}
};

class ProductGw
{
public:
	string product;
	int segid;

	/**
  * @brief Returns a string representation of the ProductGw object.
  * @return The string representation of the ProductGw object.
  */
	string Print()
	{
		ostringstream oss;
		oss << "PRODUCT=" << product << " SEGID=" << segid;
		return oss.str();
	}

	/**
  * @brief Returns the segment ID of the ProductGw object as a string.
  * @return The segment ID of the ProductGw object as a string.
  */
	string SegId()
	{
		ostringstream oss;
		oss << segid;
		return oss.str();
	}
};

/**
 * @brief The CmeMsgw class represents the CME Message Gateway.
 */
class CmeMsgw {
public:
	CmeMsgw();
	virtual ~CmeMsgw();

	/**
  * @brief Loads the CME Message Gateway.
  * @param certnr A boolean indicating whether to load the certificate number.
  */
	void Load();

	/**
  * @brief Loads the product gateway file.
  */
	void LoadProductGatewayFile();

	/**
  * @brief Prints the CME Message Gateway.
  */
	void Print();

	/**
  * @brief Checks if the CME Message Gateway is loaded.
  * @return True if the CME Message Gateway is loaded, false otherwise.
  */
	inline bool IsLoaded()
	{
		return _loaded;
	}

	/**
  * @brief Gets the Msgw object with the specified segment ID.
  * @param segid The segment ID.
  * @return The Msgw object with the specified segment ID.
  */
	inline Msgw getMsgw(int segid)
	{
		if (_gwmap.find(segid) != _gwmap.end())
			return _gwmap[segid];

		Msgw tw = {};
		return tw;
	}

	/**
  * @brief Gets the segment ID from the specified IP address.
  * @param ipaddress The IP address.
  * @return The segment ID corresponding to the IP address.
  */
	inline int getSegIdFromIp(string ipaddress)
	{
		for(auto gw : _gwmap)
		{
			if (gw.second.primip == ipaddress)
				return (int)gw.second.segid;
		}

		return 0;
	}

	/**
  * @brief Gets the product segment ID.
  * @param product The product name.
  * @param futures A boolean indicating whether the product is for futures.
  * @return The product segment ID.
  */
	inline int getProdSegId(string product, bool futures)
	{
		if (futures)
		{
			if (_prodgw.find(product) == _prodgw.end())
				return -1;

			int igw = _prodgw[product];
			return igw;
		}
		else
		{
			if (_opts_prodgw.find(product) == _opts_prodgw.end())
				return -2;

			return _opts_prodgw[product];
		}

		return -999;
	}

private:
	void LogMe(std::string szMsg, LogLevel::Enum level = LogLevel::Enum::INFO);
	void LoadFile(string filename);

private:

	bool _loaded;
	//bool _certnr;


	const string PRODGWFILE = "prodgw.csv";

	std::unordered_map<int, Msgw> _gwmap;
	std::unordered_map<string, int> _prodgw;
	std::unordered_map<string, int> _opts_prodgw;

};

}
}

#endif /* SRC_SYMBOLS_CMEMSGW_HPP_ */
