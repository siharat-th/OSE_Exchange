#include "ZmqSubscriber.h"

ZmqSubscriber::ZmqSubscriber()
{
}

ZmqSubscriber::~ZmqSubscriber()
{
}

//pass in a settings file name and a key for the list of topics for this Zmq Subscriber
void ZmqSubscriber::LoadTopics(const std::string& filename, const std::string& key)
{
	Settings settings = Settings(filename);

	vector<string> topics = settings.getListCsv(key);

	topics_.clear();

	topics_ = topics;

	for (auto topic : topics_)
		LogMe("LOADED TOPIC: " + topic, LogLevel::INFO);

}

vector<string> ZmqSubscriber::Init(std::string szServerPort,  std::string filename, std::string key, std::string name, void*(*fpSubCallback)(string, string))
{
	Settings settings = Settings(filename);
	vector<string> topics = settings.getListCsv(key);

	for (auto topic : topics)
		LogMe("LOADED TOPIC: " + topic, LogLevel::INFO);

	Init(szServerPort, topics, name, fpSubCallback);

	return topics_;
}

bool ZmqSubscriber::Init(std::string szServerAddressPort, std::vector<string> vTopics, std::string name, void*(*fpCallback)(std::string, std::string))
{
	int rc = 0;
//	listener_ = listener;
	m_fpCallback = fpCallback;
	m_serverport = szServerAddressPort;
	_NAME = name;
	
	ostringstream oss;
	oss << "ZMQ SUBSCRIBER INIT: " << m_serverport << " sz=" << szServerAddressPort;
	LogMe(oss.str());


	for (auto sz : vTopics)
		topics_.push_back(sz);

	if ((rc = pthread_create(&m_tidSub, NULL, ZmqSubscriber::EntryPoint, this)) != 0)
	{
		perror("ZMQ SUB START");
		return(true);
	}

	return(false);

}

void ZmqSubscriber::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}


void * ZmqSubscriber::EntryPoint(void* pValue)
{
	void *ctx, *s;

	ZmqSubscriber *pThisZ = (ZmqSubscriber *)pValue;

	pthread_t current_thread = pthread_self();
	std::string thread_name = pThisZ->_NAME;

	int irc = pthread_setname_np(current_thread,
			thread_name.c_str());
	if (irc != 0) {
		cout << "ERROR NAMING THREAD: " << thread_name << endl;
		exit(1);
	}

	int core = 1;
	cpu_set_t cpus;
	CPU_ZERO(&cpus);
	CPU_SET(core, &cpus);
	irc = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpus);
	if (irc!=0)
	{
		cout << "ERROR SETTING THREAD AFFINITY: " << core << endl;
		exit(1);
	}

//	ostringstream oss;
//	oss << "ZMQ INITIALIZING SUBSCRIBER CLIENT THREAD4";
//	pThisZ->LogMe(oss.str(),LogLevel::INFO);

	int rc=0;

	ctx = zmq_ctx_new();

	if (ctx == NULL)
	{
		cout << "ZMQ CONTEXT ERROR" << endl;
		return NULL;
	}


	s = zmq_socket(ctx, ZMQ_SUB);
	if (s == NULL)
	{
		perror("ZMQ SUB SOCKET CREATE");
		cout << "ZMQ SUB SOCKET CREATE ERROR: " << rc << endl;
		return NULL;
	}


	pThisZ->LogMe("ZMQ CONNECTING TO " + pThisZ->m_serverport, LogLevel::INFO);

//	cout << "ZMQ TOPICS: " << endl;
//	for (auto xx : pThisZ->topics_)
//		cout << "TOPIC:" << xx << endl;


	rc = zmq_connect(s, pThisZ->m_serverport.c_str());

	if (rc != 0)
	{
		perror("ZMQ SUB CONNECT");
		cout << "ZMQ SUB CONNECT ERROR: " << rc << endl;
		std::exit(1);
		return NULL;
	}
	//add topics
	for (auto sz : pThisZ->topics_)
	{
		char *cpy = new char[sz.size() + 1];
		strcpy(cpy, sz.c_str());
		int i = strlen(cpy);

		if (zmq_setsockopt(s, ZMQ_SUBSCRIBE, cpy, i) != 0)
		{
			perror("ZMQ SUB ADD TOPIC");
			cout << "ZMQ SUB TOPIC ADD ERROR: " << rc << endl;
			return NULL;
		}
		else{
			ostringstream oss;
			oss << "SUBSCRIBER TOPIC ADDED: " << sz ;
			pThisZ->LogMe(oss.str(), LogLevel::INFO);
		}
	}

	while (true)
	{
	    zmq_msg_t message;
	    zmq_msg_init(&message);
	    int more;
	    size_t more_size = sizeof(more);

	    // Receive the first part (topic)
	    if (zmq_msg_recv(&message, s, 0) == -1) {
	        cout << "Error receiving message. Exiting thread." << endl;
	        break;
	    }
	    string topic((char *)zmq_msg_data(&message), zmq_msg_size(&message));
	    zmq_msg_close(&message);

	    // Check if there are more parts (payload)
	    zmq_getsockopt(s, ZMQ_RCVMORE, &more, &more_size);

	    string payload;
	    if (more) {
	        zmq_msg_init(&message);
	        if (zmq_msg_recv(&message, s, 0) == -1) {
	            cout << "Error receiving payload. Exiting thread." << endl;
	            break;
	        }
	        payload.assign((char *)zmq_msg_data(&message), zmq_msg_size(&message));
	        zmq_msg_close(&message);
	    }
	    else
	    {
	    	//cout << "ZMQ SUB GOT MSG WITH NO HEADER: " << topic <<  endl;
	    	continue;
	    }

	   //cout << "ZMQ SUB GOT MULTIPART MSG: Topic: " << topic << " Payload: " << payload << endl;

	    pThisZ->m_fpCallback(topic, payload);

	}

	cout << "EXITING ZMQ RECEIVER THREAD!!!" << endl;
	//zmq_msg_close(&zmsg);
	return(NULL);
}


