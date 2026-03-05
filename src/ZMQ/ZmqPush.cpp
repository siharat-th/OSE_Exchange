/*
 * ZmqPush.cpp
 *
 *  Created on: Jan 3, 2017
 *      Author: sgaer
 */

#include "ZmqPush.h"



ZmqPush::ZmqPush(){

}

ZmqPush::~ZmqPush() {

}


void ZmqPush::LogMe(std::string szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

bool ZmqPush::Init(char* szServerAddressPort) {

	ostringstream os;
	os << "ZMQ PUSH INIT: " << szServerAddressPort;
	LogMe(os.str());


	srv_ = szServerAddressPort;

	//ctx = (void *)NULL;
	//s = (void *) NULL;

	/* Initialise 0MQ context*/
	ctx = zmq_ctx_new();
	//int rctx = zmq_ctx_set(ctx,ZMQ_IO_THREADS,1);
	if (ctx == NULL)
	//if (rctx < 0)
	{
		perror("ZMQ PUSH CTX ");
		cout << "SERVER Context=NULL :(" << endl;
		return(true);
	}


	s = zmq_socket(ctx, ZMQ_PUSH);
	if (s == NULL)
	{
		perror("ZMQ PUSH SOCKET ");
		cout << "Socket = NULL :(" << endl;
		return(true);
	}


	int rc = zmq_connect(s, szServerAddressPort);

	if (rc != 0)
	{
		perror("ZMQ PUSH CONNECT ");
		cout << "!!!!!!!!!!!!!!!!!!!!!!PUSH CLIENT CONNECT FAILED :(" << endl;
		return(true);
	}

//	usleep(1000000);
//	ServiceRequest svc;
//		strcpy(svc.hdr,"SERV\0");
//		strcpy(svc.topic,"RISKLIMITS\0");
//		svc.value1 = -1;
//
//		cout << "HDR=" << svc.hdr << " TOPIC=" << svc.topic << "VALUE=" << svc.value1 << endl;
//
//		Push((char*)&svc, sizeof(ServiceRequest));

//	LogMe("ZMQ PUSH INIT ON " + )


	return(false);


}


void ZmqPush::Push(char* msg, int len) {


	 if (zmq_send(s, msg, len, 0) == -1){
		perror("ZMQ PUSH SEND ");
		cout <<"ERROR SENDING ZMQ PUSH" << endl;
		if (s == NULL)
			cout << "SOCK IS NULL ASS." << endl;
		else
			cout << "ERRNO " << errno << endl;

	}
//	 else
//		 cout << "PUSH RESULT OK" << endl;





}


