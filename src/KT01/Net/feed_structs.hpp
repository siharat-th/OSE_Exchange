/*
 * feed_structs.hpp
 *
 *  Created on: Feb 27, 2017
 *      Author: sgaer
 */

#ifndef FEED_STRUCTS_HPP_
#define FEED_STRUCTS_HPP_

#include <sys/epoll.h>

namespace KTN {
namespace EFVI {
/* Struct to hold descriptors and
 * epoll event structs and thread id, affinity etc.
 * for each type of thread/feed we want to decode
 */
struct pollbase
{
	int s;
	int efd;
	struct epoll_event event;
	struct epoll_event *events;
	pthread_t tid;
	int affinity;
	int timeout;
};

struct feeddata
{
	int fd;
	int feedtype;
	//std::string name;
	int source;
	int bytes;
	//for tcp direct; zfur is opaque zocket ref
	//zfur* ur;
};



struct latholder
{
	int nanos;
	int source;
	int seq;
};

struct csmlatholder
{
	int feedtype;
	int source;
	int msgseqnum;
	int msgtype;
	int msgnum;
	int totmsgs;
};


}
}

#endif /* FEED_STRUCTS_HPP_ */
