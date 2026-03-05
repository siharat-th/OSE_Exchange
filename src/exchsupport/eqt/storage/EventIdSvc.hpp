#ifndef SRC_EQT_EVENT_ID_SVC_HPP_  
#define SRC_EQT_EVENT_ID_SVC_HPP_  

#include <iostream>  
#include <fstream>  
#include <string>  
#include <unistd.h> // for usleep  
#include <string>  
#include <stdio.h>  
#include <atomic> // Include this header to fix the incomplete type error  

using namespace std;  

#include <KT01/Core/NamedThread.hpp>  
#include <tbb/concurrent_queue.h>  

class EventIdSvc : public NamedThread {  
public:  
   EventIdSvc(string dir, string filename);  

   virtual ~EventIdSvc();  

   void Persist(uint64_t eventId);  

private:  
   void Run() override;  

   tbb::concurrent_queue<uint64_t> _MSGS;  
   std::atomic<bool> _ACTIVE;  
   string _FILENAME;  
   string _DIR;  
   int _keepalive;  
};  

#endif /* SRC_EQT_EVENT_ID_SVC_HPP_ */
