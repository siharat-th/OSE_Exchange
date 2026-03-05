#include "ZmqSubscriberV5.hpp"

ZmqSubscriberV5::ZmqSubscriberV5(IZmqSubscriber* sub, const string& server, int port, const string& name,
		vector<string> topics)
	:_callback(sub),_NAME(name),topics_(topics),_SERVER(server),_PORT(port)
{
	_NAME = CheckName(_NAME);
	CreateAndRun(_NAME, 1);

}

ZmqSubscriberV5::ZmqSubscriberV5(IZmqSubscriber* sub, const string& server, int port, const string& name,
		const string& filename, const string& key)
	:_callback(sub),_NAME(name),_SERVER(server),_PORT(port)
{
	LoadTopics(filename, key);

	_NAME = CheckName(_NAME);
	CreateAndRun(_NAME, 1);

}

ZmqSubscriberV5::~ZmqSubscriberV5()
{
	if (_ACTIVE.load(std::memory_order_relaxed) == true)
	{
		_ACTIVE.store(false);
	}
}

/*
 * Load topics from a settings file <Settings/Settings.h>
 * Gives us tons of flexibility here
 */
void ZmqSubscriberV5::LoadTopics(const std::string& filename, const std::string& key)
{
	Settings settings = Settings(filename);
	vector<string> topics = settings.getListCsv(key);

	topics_.clear();
	topics_ = topics;

	for (auto topic : topics_)
		LogMe("LOADED TOPIC: " + topic, LogLevel::INFO);
}

void ZmqSubscriberV5::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
	KT01_LOG_INFO(__CLASS__, szMsg);
}


void ZmqSubscriberV5::Run()
{
	void *ctx, *s;
	int rc=0;

	ctx = zmq_ctx_new();

	if (ctx == NULL)
	{
		cout << "ZMQ CONTEXT ERROR" << endl;
		return;
	}

	s = zmq_socket(ctx, ZMQ_SUB);
	if (s == NULL)
	{
		perror("ZMQ SUB SOCKET CREATE");
		cout << "ZMQ SUB SOCKET CREATE ERROR: " << rc << endl;
		return;
	}

	string m_serverport = "tcp://" + _SERVER + ":" + std::to_string(_PORT);
	rc = zmq_connect(s, m_serverport.c_str());

	LogMe("ZMQ CONNECTING TO " + m_serverport, LogLevel::INFO);

	if (rc != 0)
	{
		perror("ZMQ SUB CONNECT");
		cout << "ZMQ SUB CONNECT ERROR: " << rc << endl;
		std::exit(1);
		return;
	}
	//add topics
	for (auto sz : topics_)
	{
		char *cpy = new char[sz.size() + 1];
		strcpy(cpy, sz.c_str());
		int i = strlen(cpy);

		if (zmq_setsockopt(s, ZMQ_SUBSCRIBE, cpy, i) != 0)
		{
			perror("ZMQ SUB ADD TOPIC");
			cout << "ZMQ SUB TOPIC ADD ERROR: " << rc << endl;
			return;
		}
		else{
			ostringstream oss;
			oss << "TOPIC ADDED TO SUB: " << sz ;
			LogMe(oss.str(), LogLevel::INFO);
		}
	}

	_ACTIVE.store(true);
	while(_ACTIVE.load(std::memory_order_relaxed))
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
	    	continue;
	    }

	   //cout << "ZMQ SUB GOT MULTIPART MSG: Topic: " << topic << " Payload: " << payload << endl;

	    _callback->onPubMsg(topic, payload);

	}

	cout << "EXITING ZMQ SUBV5 THREAD!!!" << endl;

}


