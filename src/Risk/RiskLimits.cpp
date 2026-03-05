
#include "RiskLimits.h"
#include <iostream>
using namespace std;


RiskLimits::RiskLimits() : max_order_qty(0),
	max_open_qty_buy(0),
	max_open_qty_sell(0),
	max_pos_long(0),
	max_pos_short(0),
	curr_open_qty_buy(0),
	curr_open_qty_sell(0),
	curr_pos_long(0),
	curr_pos_short(0),
	enabled(1),
	hasData(false),
	m_bIsEnabled(true),
	m_iLogLevel(0)
{
}


RiskLimits::~RiskLimits()
{
}

void RiskLimits::Print(bool current_risk_only)
{

	ostringstream oss;

	if (!current_risk_only)
	{
		oss <<"KEY=" << keyvalue;
		oss << " ENABLED=" << enabled;
		oss << " ACRO=" << acronym ;
		oss << " PROD=" <<product ;
		oss << " EXCH=" << exchange;

		//now update the limits;
		oss << " MAX ORDQTY=" << max_order_qty ;
		oss << " MAX_OP_BUY=" << max_open_qty_buy ;
		oss << " MAX_OP_SELL=" << max_open_qty_sell ;
		oss << " MAX_POS_LONG=" << max_pos_long ;
		oss << " MAX_POS_SHORT=" << max_pos_short;

		LogMe(oss.str());


		oss.clear();
		oss.str("");

	}

	oss <<"KEY=" << keyvalue;
	oss << " LONG=" << curr_pos_long;
	oss << " SHORT=" << curr_pos_short;
	oss << " NET=" << NetPos();

	LogMe(oss.str(),LogLevel::INFO);

	oss.clear();
	oss.str("");

	oss <<"KEY=" << keyvalue;
	oss << " OPEN BUYS=" << curr_open_qty_buy;
	oss << " OPEN SELLS=" << curr_open_qty_sell;

	LogMe(oss.str(),LogLevel::WARN);
}


void RiskLimits::LogMe(std::string szMsg, LogLevel::Enum loglevel)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void RiskLimits::LogLevel(int iLogLevel)
{
	m_iLogLevel = iLogLevel;
	char buff[244];
	sprintf(buff, "RISK LIMIT LOG LEVEL SET TO %d", m_iLogLevel);
	LogMe(buff);
}

void RiskLimits::Update(int enable, std::string acro, std::string prod,std::string prodtype, std::string exch, int iMaxOrderSize, int iMaxLong, int iMaxShort, int iMaxBuyOpen, int iMaxSellOpen, int iLogLevel)
{
	acronym = acro;
	product = prod;
	exchange = exch;
	enabled = enable;
	producttype = prodtype;

	m_iLogLevel = iLogLevel;

	LogMe("RISK LIMITS UPDATE FOR " + acronym + "." + product + "." + exchange);

	//now update the limits;
	max_order_qty = iMaxOrderSize;
	max_open_qty_buy = iMaxBuyOpen;
	max_open_qty_sell = iMaxSellOpen;
	max_pos_long = iMaxLong;
	max_pos_short = iMaxShort;

	//cout << "OP BUYS=" <<  curr_open_qty_buy << endl;

	if (enabled == 1)
		m_bIsEnabled = true;
	else
		m_bIsEnabled = false;

	Print();


}

void RiskLimits::SetEnable(bool enable)
{
	m_bIsEnabled = enable;
	enabled = (enable) ? 1 : 0;
}

bool RiskLimits::OK()
{
	if (enabled == 1)
		m_bIsEnabled = true;
	else
		m_bIsEnabled = false;

	return m_bIsEnabled;
}

bool RiskLimits::LegalSize(int iOrderQty)
{
	return (max_order_qty >= iOrderQty);
}
int RiskLimits::GetMaxContracts(bool bIsBid)
{
	return (bIsBid) ? max_open_qty_buy : max_open_qty_sell;;
}
int RiskLimits::GetOrderCount(bool bIsBid)
{
	return (bIsBid) ? curr_open_qty_buy : curr_open_qty_sell;
}
int RiskLimits::GetPosition(bool bIsBid)
{
	return (bIsBid) ? curr_pos_long : curr_pos_short;
}
int RiskLimits::GetMaxOrderSize()
{
	return max_order_qty;
}

void RiskLimits::GotOrder(int iSize, bool bIsBid, bool bIsCxl)
{
	int iMult = (bIsCxl) ? -1 : 1;

	iSize *= iMult;

	//cout << "GOTORDER: SIZE=" << iSize << " ISBID=" << bIsBid << " ISCXL=" << bIsCxl;

	if (bIsBid)
		curr_open_qty_buy += iSize;
	else
		curr_open_qty_sell += iSize;

//	cout << "1. OP BUY=" << curr_open_qty_buy << " OP SELL=" << curr_open_qty_sell << endl;

	if (curr_open_qty_buy < 0) curr_open_qty_buy = 0;
	if (curr_open_qty_sell < 0) curr_open_qty_sell = 0;


	//cout << "2. OP BUY=" << curr_open_qty_buy << " OP SELL=" << curr_open_qty_sell << endl;


}
void RiskLimits::GotFill(int iSize, bool bIsBuy)
{
//	int iMult = (bIsBuy) ? 1 : -1;

	if (bIsBuy){
		curr_pos_long += iSize;
		curr_open_qty_buy -= iSize;
	}
	else{
		curr_pos_short += iSize;
		curr_open_qty_sell -= iSize;
	}

}

int RiskLimits::NetPos()
{
	return (curr_pos_long - curr_pos_short);
}

bool RiskLimits::CheckPos()
{
	bool bRes = true;
	if ((curr_pos_long > max_pos_long) || (curr_pos_short > max_pos_short))
		bRes = false;

	return bRes;
}
bool RiskLimits::CheckOrds(bool bIsBid)
{
	bool bRes = true;

	if (bIsBid && ((curr_open_qty_buy + curr_pos_long) > max_open_qty_buy))
		bRes = false;

	if (!bIsBid && ((curr_open_qty_sell + curr_pos_short) > max_open_qty_sell))
		bRes = false;

	return bRes;

}

bool RiskLimits::WillMaxOut(bool bIsBid, int iAdd)
{
	bool bRes = true;

	if (bIsBid && ((curr_open_qty_buy + curr_pos_long + iAdd) > max_open_qty_buy))
		bRes = false;

	if (!bIsBid && ((curr_open_qty_sell + curr_pos_short + iAdd) > max_open_qty_sell))
		bRes = false;

	return bRes;
}

