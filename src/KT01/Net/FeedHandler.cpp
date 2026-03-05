/*
 * FeedHandler.cpp
 *
 *  Created on: Feb 28, 2017
 *      Author: sgaer
 */

#include <KT01/Net/FeedHandler.hpp>

namespace KTN {
namespace EFVI {

FeedHandler::FeedHandler(const std::string& name, UdpCallback& callback)
	: m_Me(name)
	, m_callback(callback)
{

	LogMe("INITIALIZING FEED HANDLER " + m_Me);

	LAST_L1_SEQa = 0;
	LAST_L2_SEQa = 0;
	m_iPre0 = -1;
}

FeedHandler::~FeedHandler() {

}




void FeedHandler::PrintStats() {

	printstats(vLatsL1, "L1");


	char buff[222];

	sprintf(buff,"SEQ NUMS: L1=%d| L2=%d",LAST_L1_SEQ, LAST_L2_SEQ);
	LogMe(buff);

	ostringstream os;
	os << "ALL SEQ: ";
	for (auto tt : seqMap)
	{
		os << tt.first << "=" << tt.second << "|";
	}
	LogMe(os.str());

	LAST_L1_SEQ = 0;
	LAST_L2_SEQ = 0;

	sprintf(buff,"SEQ NUM RESET: L1=%d L2=%d", LAST_L1_SEQ, LAST_L2_SEQ);
	LogMe(buff);

	vLatsL1.clear();
	vLatsL2.clear();
}

void FeedHandler::LogMe(std::string szMsg) {
	KT01_LOG_INFO(__CLASS__, szMsg);
}

int FeedHandler::make_socket_non_blocking(int sfd) {
	  int flags, s;

	  flags = fcntl (sfd, F_GETFL, 0);
	  if (flags == -1)
	    {
	      perror ("fcntl");
	      return -1;
	    }

	  flags |= O_NONBLOCK;
	  s = fcntl (sfd, F_SETFL, flags);
	  if (s == -1)
	    {
	      perror ("fcntl");
	      return -1;
	    }

	  return 0;
}

int FeedHandler::handle_einprogress(int sfd) {
	fd_set scanSet;
	FD_ZERO(&scanSet);
	FD_SET(sfd,&scanSet);

	struct timeval waitTime;
	waitTime.tv_sec = 4;
	waitTime.tv_usec = 0;

	int tmp;
	tmp = select(sfd +1, (fd_set*)0, &scanSet, (fd_set*)0,&waitTime);
	if(tmp == -1 || !FD_ISSET(sfd,&scanSet))
	{
		int savedErrorNo = errno;
		char buff[222];
		sprintf(buff,"Connect to CFE Host failed after select, cause %d, error %s",savedErrorNo,strerror(savedErrorNo));
		LogMe(buff);
		close(sfd);
		return -1;
	}


	return 0;
}

void FeedHandler::outputStatInfo(std::string name, int r,
		struct onload_stat* stat) {
	if( r == 0 )
	  {
		LogMe(name + ": SOCKET NOT ACCELERATED");
		/* no memory needs to be freed */
	  }
	  else if( r == 1 )
	  {
		/* Output from onload_stackdump will show the sockets as:
		 *    <protocol> <stack_id>:<endpoint_id>
		 */
		 char buff[222];
		 sprintf(buff,"%s SOCKET ACCELERATED: STACK_NAME:%s STACK_ID=%d  ENDPOINT=%d",name.c_str(), stat->stack_name, stat->stack_id, stat->endpoint_id);
		 LogMe(buff);
		/* free memory allocated by onload_fd_stack() call */
		free(stat->stack_name);
	  }
	  else
		printf("onload_fd_stat call failed (rc=%d)\n", r);
}

void FeedHandler::CheckOnload() {
	int res = onload_is_present();
	std::string szRes = (res ==1) ? "APPLICATION RUNNING ONLOAD!" : "APPLICATION NOT RUNNING ONLOAD";
	LogMe(szRes);

	if (res == 1)
	{
		int irc = onload_thread_set_spin(ONLOAD_SPIN_ALL, 1);

		if (irc != 0)
		{
			perror("ONLOAD SPIN LOCK");
			exit(1);
		}

	}
}

void FeedHandler::printstats(std::vector<int>& vec, std::string title) {
	double dSum = 0;
	int iMin = 1000000;
	int iMax = 0;

	for(size_t i = 0; i < vec.size(); i++)
	{
		if (vec[i] > iMax) iMax = vec[i];
		if (vec[i] < iMin) iMin = vec[i];
		dSum += vec[i];
	}

	double dAvg = dSum/vec.size();


	char buff[222];
	sprintf(buff,"%s MIN:%d MAX:%d| AVG:%.2f| CNT:%d",title.c_str(), iMin,iMax,dAvg,(int)vec.size());
	LogMe(buff);
}


void FeedHandler::WriteCsmLatLog(int source, std::string feedtype, int templateid, int msgseqnum, int msgnum, int num)
{

	PerformanceCounter::current(&lat_);
	ostringstream os;
	os << lat_.tv_sec << "," << lat_.tv_nsec << ","<< feedtype << "," << source << "," << templateid << "," << msgseqnum;
//	m_csmLatLog->writeLog(os.str());

}




} /* namespace core */
} /* namespace LWT */
