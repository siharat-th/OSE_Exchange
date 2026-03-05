#include "RiskManager.h"
#include <tbb/spin_mutex.h>
using namespace tbb;

spin_mutex smutex;

using namespace KTN;

//ahole
RiskManager * m_pThisRisk;

RiskManager::RiskManager()
	: m_bIsInited(false),m_bGotLimits(false)
{
}

RiskManager::~RiskManager()
{
}

void RiskManager::Init(std::string key, std::string account, std::string source,
		std::string argoserver, int argosubport, int argopushport, int affinity)
{

	m_pThisRisk = this;

	m_affinity = affinity;

	m_bIsInited = true;

	KEY_ = key;
	ACCOUNT_ = account;
	SOURCE_ = source;

	LogMe("KEY=" + KEY_ + " ACCOUNT=" + ACCOUNT_ + " SOURCE=" + SOURCE_, LogLevel::WARN);

	ostringstream oss;


	char buff[255];
	sprintf(buff, "tcp://%s:%d", argoserver.c_str(), argopushport);
	std::string sz(buff);

	ZMQ_RISK_PUSH_SERVER_ = sz;


	oss << "STARTING RISK MANAGER FOR RMS KEY=" << KEY_
			<< " SERVER=" << ZMQ_RISK_PUSH_SERVER_;

	LogMe(oss.str(), LogLevel::INFO);


	LogMe("CREATING RMS CONSUMER THREAD", LogLevel::INFO);
	int rc = 0;
	if ((rc = pthread_create(&tid_, NULL, ConsumerThread, this)) != 0)
	{
		perror("CONSUMER THREAD");
		LogMe("ERROR CREATING CONSUMER THREAD", LogLevel::ERROR);
	}


}

//NEW CODE REPLACES ZMQPP
void* RiskManager::ConsumerThread(void* pValue) {
    RiskManager* pThis = static_cast<RiskManager*>(pValue);

    // Thread attributes for setting name and CPU affinity
    pthread_attr_t attr;
    cpu_set_t cpus;
    pthread_attr_init(&attr);
    pthread_t current_thread = pthread_self();

    // Set thread name
    std::string szName = "RMSConsumer";
    int irc = pthread_setname_np(current_thread, szName.c_str());
    if (irc != 0) {
        perror("NAMING THREAD");
        exit(1);
    }

    // Logging start
    std::ostringstream os;
    os << "**STARTING RMS THREAD ON CORE " << pThis->m_affinity;
    pThis->LogMe(os.str(), LogLevel::INFO);

    // Set CPU affinity
    CPU_ZERO(&cpus);
    CPU_SET(pThis->m_affinity, &cpus);
    pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpus);

    //int BATCH_SIZE = 50;

    // Log ZMQ connection info
    pThis->LogMe("ZMQ PUSH CONNECT ADDRESS:" + pThis->ZMQ_RISK_PUSH_SERVER_, LogLevel::INFO);
    const std::string& endpoint = pThis->ZMQ_RISK_PUSH_SERVER_;

    // Initialize ZeroMQ context and socket
    void* context = zmq_ctx_new();
    void* socket = zmq_socket(context, ZMQ_PUSH);
    zmq_connect(socket, endpoint.c_str());

    // Confirm connection
    std::ostringstream zzz;
    zzz << "Opening connection to " << endpoint << "...";
    pThis->LogMe(zzz.str(), LogLevel::INFO);

    bool active = true;

    // Main loop to process messages
    while (active) {
        std::vector<std::string> json_ready_to_send;
        std::vector<Order> ords;

        // Process RMS queue messages
        while (!pThis->_qRMS.empty()) {
            std::string msg;
            if (pThis->_qRMS.try_pop(msg)) {
                zmq_send(socket, msg.c_str(), msg.size(), 0);
                usleep(100);
            }
        }

        // Process JSON queue messages
        while (!pThis->_qJSON.empty()) {
            std::string szJson;
            if (pThis->_qJSON.try_pop(szJson)) {
                zmq_send(socket, szJson.c_str(), szJson.size(), 0);
                usleep(100);
            }
        }

        // Process Orders queue messages
        while (!pThis->_qOrds.empty()) {
            Order ord;
            if (pThis->_qOrds.try_pop(ord)) {
                std::string szSymb = ord.symbol;
                size_t index = szSymb.find("/");
                if (index != std::string::npos) {
                    ord.product = szSymb.substr(0, index);
                }

                ord.ind = "ARGO";

                // Set order state and report type based on status
                switch (ord.OrdStatus) {
                    case KOrderStatus::NEW:
                        ord.reporttype = "NEWORDER";
                        ord.state = "ACTIVE";
                        break;
                    case KOrderStatus::CANCELED:
                        ord.reporttype = "CXLORDER";
                        ord.state = "COMPLETE";
                        ord.lastqty = 0;
                        break;
                    case KOrderStatus::MODIFIED:
                        ord.reporttype = "MODORDER";
                        ord.state = "ACTIVE";
                        break;
                    case KOrderStatus::PARTIALLY_FILLED:
                        ord.reporttype = "FILLREPORT";
                        ord.state = "ACTIVE";
                        break;
                    case KOrderStatus::FILLED:
                        ord.reporttype = "FILLREPORT";
                        ord.state = "COMPLETE";
                        break;
                    case KOrderStatus::FAILED:
                    case KOrderStatus::REJECTED:
                        ord.reporttype = "NEWORDER";
                        ord.state = "COMPLETE";
                        break;
                }
                ords.push_back(ord);
            }
        }

        // Send Orders in JSON format if available
        if (!ords.empty()) {
            std::vector<Order> ordtemp;
            size_t MAX_ORDS = 100;

            // Convert orders to JSON strings
            for (const auto& ord : ords) {
                ordtemp.push_back(ord);
                if (ordtemp.size() >= MAX_ORDS) {
                    std::string szOrdsJson = OrderFormatter::FormatOrdersJson(ordtemp);
                    json_ready_to_send.push_back(szOrdsJson);
                    ordtemp.clear();
                }
            }
            // Process any remaining orders
            if (!ordtemp.empty()) {
                json_ready_to_send.push_back(OrderFormatter::FormatOrdersJson(ordtemp));
            }

            // Send JSON strings
            for (const auto& json : json_ready_to_send) {
                zmq_send(socket, json.c_str(), json.size(), 0);
                usleep(10);
            }
        }

        usleep(1000);
    }

    std::cout << "EXIT RISK MANAGER THREAD!!!" << std::endl;

    zmq_close(socket);
    zmq_ctx_destroy(context);

    return nullptr;
}



void RiskManager::GetLimits()
{
	LogMe("SENDING REQUEST FOR RISK LIMITS.",LogLevel::WARN);
	_qRMS.push("RISKLIMITS|RISKLIMITS");

}


void RiskManager::SendToRMS(Order   ord, int iStatus, int iReportType, bool bResend, bool bUseJsonArray)
{
	if (!m_bIsInited)
	{
		LogMe("ERROR: RISK MGR ZMQ NOT INITED!", LogLevel::ERROR);
		return;
	}



	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::microseconds>(now);
	auto epoch = now_ms.time_since_epoch();
	auto value = std::chrono::duration_cast<std::chrono::microseconds>(epoch);
	long duration = value.count();
	ord.timestamp = duration;


	if (ord.source.length() < 1)
	{
		ord.source = SOURCE_;
	}

	_qOrds.push(ord);
}


void RiskManager::SendToRMS(std::vector<Order> ords)
{
	for (auto ord : ords)
		_qOrds.push(ord);
}




void RiskManager::UpdateLimitsOnOrder(Order ord)
{
	bool bIsBuy = (ord.OrdSide == KOrderSide::BUY) ? true : false;

	//first is default
	std::string key = _Limits.begin()->first;

	RiskLimits & rl = _Limits[key];

	//cout << "ORD STATUS = " << ord.iStatus << " ORD KEY=" << ord.keyvalue <<  " QTY=" << ord.quantity << endl;

	switch (ord.OrdStatus)
	{
		case KOrderStatus::CANCELED:
		case KOrderStatus::FAILED:
		case KOrderStatus::REJECTED:
			rl.GotOrder(ord.quantity,bIsBuy,true);
			break;

		case KOrderStatus::MODIFIED:
			//hmmm
			rl.GotOrder(ord.quantity,bIsBuy,true);
			rl.GotOrder(ord.quantity,bIsBuy,false);
			break;

		case KOrderStatus::PARTIALLY_FILLED:
		case KOrderStatus::FILLED:
			rl.GotFill(ord.lastqty,bIsBuy);
			break;
		default:
			rl.GotOrder(ord.quantity,bIsBuy,false);
			break;
	}

	_Limits[key] = rl;

}



void RiskManager::SendRaw(std::string szJson)
{
	_qJSON.push(szJson);
}

void RiskManager::HandleRiskLimitsJSON(std::string szJson)
{
	LogMe("*****RISK LIMITS JSON", LogLevel::INFO);

	Document d;
	d.Parse(szJson.c_str());

	for (Value::ConstValueIterator itr = d.Begin(); itr != d.End(); ++itr)
	{
		RiskLimits rl;

		rl.keyvalue = (*itr)["keyvalue"].GetString();

		rl.acronym = (*itr)["acronym"].GetString();
		rl.account = (*itr)["account"].GetString();
		rl.product = (*itr)["product"].GetString();

		//rl.producttype = (*itr)["producttyp~e"].GetString();

		rl.exchange = (*itr)["exchange"].GetString();

		rl.max_order_qty = (*itr)["max_order_qty"].GetInt();
		rl.max_open_qty_buy = (*itr)["max_open_qty_buy"].GetInt();
		rl.max_open_qty_sell =(*itr)["max_open_qty_sell"].GetInt();
		rl.max_pos_long = (*itr)["max_pos_long"].GetInt();
		rl.max_pos_short = (*itr)["max_pos_short"].GetInt();

		//rl = rl;

		if (!rl.hasData)
		{
			rl.curr_open_qty_buy = 0;// (*itr)["curr_open_qty_buy"].GetInt();
			rl.curr_open_qty_sell = 0;//(*itr)["curr_open_qty_sell"].GetInt();
			rl.curr_pos_long = 0;// (*itr)["curr_pos_long"].GetInt();
			rl.curr_pos_short = 0;//(*itr)["curr_pos_short"].GetInt();
		}
		rl.hasData = true;
		rl.enabled = (*itr)["enabled"].GetInt();


		setEnable(rl);
		m_bGotLimits = true;

//		if (!rl.enabled)
//		{
//			LogMe("RISK DISABLEMENT RECEIVED: " + rl.keyvalue, LogLevel::ERROR);
//			setEnable( rl.enabled);
//		}


		string msg = "***RISK LIMITS UPDATED FOR " + rl.keyvalue;
		LogMe(msg, LogLevel::WARN);
		rl.Print();

		_Limits[rl.keyvalue] = rl;
		_ProductLimits[rl.product] = rl;

	}



}




void RiskManager::HandleException(const std::exception &e, std::string sz)
{
	char buff[512];
	sprintf(buff, "RMS %s (std): %s", sz.c_str(), e.what());
	LogMe(buff);
}



void RiskManager::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}


std::string RiskManager::getRMSTime()
{
	char buffer[50];
	time_t tt = time(0);
	tm *tm = localtime(&tt);
	strftime(buffer, sizeof(buffer), "%m-%d-%Y %H:%M:%S.000", tm);
	return buffer;
}

//RMS Actions
void RiskManager::Print()
{

	ostringstream oss;
	LogMe( "ACCT=" + ACCOUNT_ +" KEY=" + KEY_ );


	for(auto kv : _Limits)
	{
		LogMe("RISK LIMITS FOR " + kv.first, LogLevel::WARN);
		kv.second.Print();
	}
}


bool RiskManager::GotLimits()
{
	return m_bGotLimits;
}


void RiskManager::onLimitsUpdate()
{
	//string szkey = rl.keyvalue;
	m_bGotLimits = true;

	for (auto kv: _Limits)
	{
		if (!kv.second.enabled)
		{
			LogMe(kv.first + ": RISK DISABLED!", LogLevel::ERROR);
			setEnable(kv.second);
		}
	}

}



void RiskManager::onPnlUpdate(double sl, double tp)
{
	/*ostringstream os;
	os << "P&L limits update: [Loss: " << sl << ", Profit: " << tp << "]" << std::endl;
	LogMe(os.str());*/
}

void RiskManager::setEnable(RiskLimits & rl)
{
//	ostringstream os;
//	os << KEY_ <<  " Change state: " << (rl.enabled ? "Enabled" : "Disabled") << std::endl;
//	LogMe(os.str());

	rl.SetEnable(rl.enabled);

}

bool RiskManager::OK(std::string key)
{
	if (_Limits.find(key) == _Limits.end())
	{
		LogMe("OK: key not found:" + key + ". Using Default Limits");
		return _Limits.begin()->second.OK();
	}


	return _Limits[key].OK();
}

int RiskManager::MaxOrdQty(std::string key)
{
	if (_Limits.find(key) == _Limits.end())
	{
		LogMe("MaxOrdQty: key not found:" + key + ". Using Default Limits");
		return _Limits.begin()->second.max_order_qty;
	}
	return _Limits[key].max_order_qty;
}

bool RiskManager::OrderSizeOK(std::string key, int ordqty, bool bIsBuy,
		std::string & reason, int oldqty)
{

	ostringstream oss;
	RiskLimits rl = _Limits.begin()->second;
	if (_Limits.find(key) != _Limits.end())
		rl = _Limits[key];

	//LogMe("RMS CHECK: Key=" + key + " MapKey=" + rl.keyvalue, LogLevel::INFO);


	if (ordqty > rl.max_order_qty)
	{
		oss << "RMS REJECTED: ORD QTY OF " << ordqty << " EXCEEDS MAX QTY:" << rl.max_order_qty
				<< " FOR KEY " << rl.keyvalue;
		reason = oss.str();
		LogMe(oss.str(),LogLevel::WARN);
		return false;
	}


	int maxopen = (bIsBuy) ? rl.max_open_qty_buy : rl.max_open_qty_sell;
	int curropen = (bIsBuy) ? rl.curr_open_qty_buy : rl.curr_open_qty_sell;
	std::string side = (bIsBuy) ? "BUY" : "SELL";

	if ((ordqty-oldqty) + curropen > maxopen)
	{
		oss << "RMS REJECTED: ORD QTY OF " << ordqty << " WILL EXCEED MAX OPEN "
			<<	side << " LIMIT OF:" << maxopen << " (Total would be " << curropen + ordqty
			<< ") FOR KEY " << rl.keyvalue;
		reason = oss.str();
		LogMe(oss.str(),LogLevel::WARN);
		return false;
	}


	//Position effect
	int maxpos = (bIsBuy) ? rl.max_pos_long : rl.max_pos_short;
	int currpos = (bIsBuy) ? rl.curr_pos_long : rl.curr_pos_short;


	if (ordqty + currpos > maxpos )
	{
		oss << "RMS REJECTED: IF FILLED, ORD QTY OF " << ordqty << " WILL EXCEED MAX "
			<<	side << " POS LIMIT OF:" << maxopen << " (Total would be " << currpos + curropen + ordqty
			<< ") FOR KEY " << rl.keyvalue;
		reason = oss.str();
		LogMe(oss.str(),LogLevel::WARN);
		return false;
	}



	//oss << "OK: ORDER " <<	side << " QTY:" << ordqty << " OPEN: " << curropen << " FOR KEY " << rl.keyvalue;
	//LogMe(oss.str(),LogLevel::INFO);
	return true;
}


void RiskManager::panicMode(int secsToClose)
{
	/*ostringstream os;
	os << "Panic mode: " << secsToClose << " seconds to close" << std::endl;
	LogMe(os.str());
	m_Callback.RMSPanic();*/

}

void RiskManager::closePositions()
{
	ostringstream os;
	os << "Close positions" << std::endl;
	LogMe(os.str());
}

void RiskManager::closePositionsByGroup(const char* const* groupIds, int groupCount)
{
	ostringstream os;
	os << "-- Close positions for groups --" << std::endl;
	LogMe(os.str());
	for (int i = 0; i < groupCount; ++i)
	{
		os.clear();
		os << "Group: " << groupIds[i] << std::endl;
		LogMe(os.str());
	}
}

void RiskManager::cancelOrders()
{
	/*ostringstream os;
	os << "Cancel orders" << std::endl;
	LogMe(os.str());
	m_Callback.RMSCancelAllOrders();*/

}

void RiskManager::cancelOrders(const char* const* orderIds, int orderCount)
{
	/*ostringstream os;
	os << "-- Cancel specific orders --" << std::endl;
	LogMe(os.str());

	for (int i = 0; i < orderCount; ++i)
	{
	os.clear();
	os << "Order: " << orderIds[i] << std::endl;
	LogMe(os.str());

	int iSeq = atoi(orderIds[i]);

	m_Callback.RMSCancelOrder(iSeq);
	if (p_rma)
	p_rma->onOrderCancelled(orderIds[i], "Required by UI");
	}*/



}

void RiskManager::cancelOrdersByGroup(const char* const* groupIds, int groupCount)
{
	ostringstream os;
	os << "-- Cancel orders for groups --" << std::endl;
	LogMe(os.str());
	for (int i = 0; i < groupCount; ++i)
	{
		os.clear();
		os << "Group: " << groupIds[i] << std::endl;
		LogMe(os.str());
	}
}

void RiskManager::shutdown()
{
	ostringstream os;
	os << "Shutdown" << std::endl;
	LogMe(os.str());
	//m_Callback.RMSShutdown();
}

void RiskManager::onError(const char *text)
{
	ostringstream os;
	os << "Error: " << text << std::endl;
	LogMe(os.str());
	std::string szErr(text);

	//m_Callback.RMSError(szErr);
}

// Local notification from agent core
void RiskManager::onDisconnected()
{
	ostringstream os;
	os << "Strategy disconnected" << std::endl;
	LogMe(os.str());

}


//Intgrated Order Management


