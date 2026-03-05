/*
 * KtnCmeMdpV5.cpp
 *
 *  Created on: March 5, 2025
 *      Author: sgaer
 */

#include <exchsupport/cme/mktdata/KtnCmeMdpV5.hpp>

using namespace KT01::NET::UDP;

KtnCmeMdpV5::KtnCmeMdpV5( string name, int core, bool handletrades)
			: EFVIUdpV4("A", false, name), 
			_LAST_SEQa (0),
			_LAST_SEQb (0),
			_NAME(name),
			_PREFIX_NAME("MDPV5-"),
			_CORE(core)
{
	ostringstream oss;
	oss <<"STARTING CME MDP **V5** HANDLER WRAPPER ON CORE "<< _CORE ;
	LogMe(oss.str(),LogLevel::INFO);

	//We are going to break this up here -- moving to a more logical place
	Settings settings;
	settings.Load("Settings.txt", "Settings");
	settings.Load("Settings.txt", "Settings");

	string xmldir = settings.getString("Cme.ConfigXmlFileDir");
	string xmlfile = settings.getString("Cme.ConfigXmlFile");

	_DEBUGMODE = settings.getBoolean("Cme.DebugMdp");

	if (_DEBUGMODE)
		cout << "[KtnCmeMdpV5] DEBUG MODE ENABLED" << endl;

	if (!xmldir.empty() && xmldir.back() != '/')
		xmldir += '/';
	else
	{
		LogMe("NO ENTRY FOR Cme.ConfigXmlFileDir FOUND in Settings.txt",LogLevel::ERROR);
		return;
	}

	string fqfile = xmldir + xmlfile;
	LogMe("MDP CONFIG FILE:" + fqfile, LogLevel::INFO);

	_Channels = {};
	_Channels.Load(fqfile);
	_Channels.Info();

	_handleMbo = false;

	_handleVolume = false;
	_handleTrades = handletrades;


	if (_handleTrades)
		LogMe("******* WATCHING TRADES MKT DATA MSGS!!!! *************", LogLevel::INFO);
	
	_FILTERS.resize(_MAXSECS);
	//_books.resize(_MAXSECS);
	Obm.Resize(_MAXSECS);
	updatedIndices.reserve(_MAXSECS);
}

KtnCmeMdpV5::~KtnCmeMdpV5() 
{
	_ACTIVE.store(false, std::memory_order_relaxed);
}

void KtnCmeMdpV5::Start()
{
	std::string szName = _NAME + "-efmd";
	CreateAndRun(szName, _CORE);
}

void KtnCmeMdpV5::Run()
{
	_ACTIVE.store(true, std::memory_order_relaxed);
	while(_ACTIVE.load(std::memory_order_relaxed) == true){
		poll();
	}
}

void KtnCmeMdpV5::Stop()
{
	_ACTIVE.store(false, std::memory_order_relaxed);
}

void KtnCmeMdpV5::LogMe(const std::string& szMsg, LogLevel::Enum level)
{
	KT01_LOG_INFO(__CLASS__, szMsg);
}

void KtnCmeMdpV5::Subscribe(AlgoMktDataCallback* subscriber)
{
	_callback = subscriber;
}


void KtnCmeMdpV5::on_rx_cb(const char * buf, const int len, const uint32_t address, const uint64_t recvTime)
{
	int ilooper = 0;
	int bytesread = 0;

	auto hdr = reinterpret_cast<const PacketHeader *>(buf);

	bytesread += sizeof(PacketHeader);
	buf += sizeof(PacketHeader);

	_LAST_SEQa = hdr->MsgSeqNum;

	if (address == seqTracker[0].first)
	{
		const uint32_t expectedSeq = seqTracker[0].second + 1;
		if (_LAST_SEQa != expectedSeq)
		{
			LOGWARN("Expected sequence number {} but got {} for {}", expectedSeq, _LAST_SEQa, address);
		}
		seqTracker[0].second = _LAST_SEQa;
	}
	// Index 1 is hardcoded to snapshots right now, don't care about drops for that
	// else if (address == seqTracker[1].first)
	// {
	// 	const uint32_t expectedSeq = seqTracker[1].second + 1;
	// 	if (_LAST_SEQa != expectedSeq)
	// 	{
	// 		//LOGWARN("Expected sequence number {} but got {} for {}", expectedSeq, _LAST_SEQa, address);
	// 	}
	// 	seqTracker[1].second = _LAST_SEQa;
	// }

	 while (bytesread < len)
	 {
		 auto msghdr = reinterpret_cast<const MsgHeader*>(buf);
		 buf +=  sizeof(MsgHeader);
		 bytesread += sizeof(MsgHeader);

		 //MsgSize	uInt16 Length of entire message, including binary header in number of bytes
		 int iMsgLen = msghdr->MsgSize;
		 //int imsglen_tst = msghdr->MsgSize;
		 int iTemplateid = static_cast<int>(msghdr->templateid);

		//  if (_DEBUGMODE)
		//  	cout << "[KtnCmeMdpV5 DEBUG] TEMPLATE=" << iTemplateid << " | SEQ=" << _LAST_SEQa  << " | MsgLen=" << iMsgLen << " LOOP=" << ilooper << " START=" << bytesread << endl;

		 if (iTemplateid <= 0 || iMsgLen <= 0)
			 break;

		 switch (iTemplateid)
		 {
		 	 case Template::TemplateEnum::MDIncrementalRefreshBook46:
		 		bytesread  += ProcessMDIncrementalRefreshPrice46(buf, iMsgLen, recvTime);
				// if (_DEBUGMODE)
				//  	cout << "[KtnCmeMdpV5 DEBUG] MD46 iMsgLen=" << iMsgLen << " bytesread=" << bytesread << " len=" << len << endl;
				break;

			 case Template::TemplateEnum::MDIncrementalRefreshTradeSummary48:
				if (_handleTrades)
					bytesread += ProcessMDIncrementalRefreshTradeSummary48(buf, iMsgLen, recvTime);
				else
					bytesread += (iMsgLen - sizeof(MsgHeader));
				break;

			
			 case Template::TemplateEnum::SnapshotFullRefresh52:
			 		// if (_DEBUGMODE)
				 	// 	cout << "[KtnCmeMdpV5 DEBUG] SnapshotFullRefresh52" << endl;
					bytesread += ProcessSnapshotFullRefresh52(buf, len);
				break;
			
			case Template::TemplateEnum::SecurityStatus30:
				ProcessSecurityStatus30(buf, iMsgLen, recvTime);
				bytesread += (iMsgLen - sizeof(MsgHeader));
				break;

			 default:{
				 bytesread += (iMsgLen - sizeof(MsgHeader));
			 }break;
		 }
		
		 if (bytesread >= len)
		 {
			//cout << "   BREAK: BYTES READ=" << bytesread << " OVERALL LEN=" << len << " THIS MSG=" << iMsgLen << " LOOP=" << ilooper << endl;
			 break;
		 }
		 ilooper ++;
		 //now just move the buffer to the next message in packet- you don't need to move from beginning.
		 //from MsgLen def: uint16_t	MsgSize; // Length of entire message, including binary header in nbr of bytes
		 // so we've alreay moved it sizeof(MsgHeader) to get to the message...
		 buf += (msghdr->MsgSize - sizeof(MsgHeader));
		
	 }
}

int KtnCmeMdpV5::ProcessMDIncrementalRefreshPrice46(const char* buf, const int iMsgLen, const uint64_t recvTime)
{
    //auto msg = reinterpret_cast<MDIncrementalRefreshBook46*>(buf);
    buf += sizeof(MDIncrementalRefreshBook46);
    int bytesread = 11; //MDIncrementalRefreshBook46 blocksize = 11

    if (bytesread >= iMsgLen)
        return bytesread;

    auto grp = reinterpret_cast<const groupSize*>(buf);
    buf += 3;// sizeof(groupSize);
    bytesread += 3;//sizeof(groupSize);

	if (bytesread >= iMsgLen)
		return bytesread;

	int lastIndex = -1;

    for (uint8_t i = 0; i < grp->numInGroup; i++)
    {
        auto entry = reinterpret_cast<const MDIncrementalRefreshBook46Entry*>(buf);

		int index = Obm.IndexOf(entry->SecurityID);

		// if (_DEBUGMODE)
		// 	cout << "[KtnCmeMdpV5 DEBUG] MD46 secid=" << entry->SecurityID << " index=" << index << endl;

		if (index >= 0)
		{
			if (index != lastIndex)
			{
				lastIndex = index;
				bool found = false;
				for (size_t i = 0; i < updatedIndices.size(); ++i)
				{
					if (updatedIndices[i] == index)
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
					updatedIndices.push_back(index);
				}
			}

			pxl p;
			if (entry->MDUpdateAction == EnumMDUpdateAction::MdNew)
				p.act = BookAction::New;
			if (entry->MDUpdateAction == EnumMDUpdateAction::MdChange)
				p.act = BookAction::Change;
			if (entry->MDUpdateAction == EnumMDUpdateAction::MdDelete)
				p.act = BookAction::Del;
	
			p.lvl = entry->MDPriceLevel;
			p.px = PriceConverters::FromMantissa9ToPrice(entry->MDEntryPx.mantissa);// PriceConverters::FromMantissa9(entry->MDEntryPx.mantissa);
	
			if (entry->MDEntryType == EnumMDEntryType::Bid)
				p.side = BookSide::Bid;
			if (entry->MDEntryType == EnumMDEntryType::Offer)
				p.side = BookSide::Ask;
	
			if (entry->MDEntryType == EnumMDEntryType::ImpliedBid)
				p.side = BookSide::ImpBid;
			if (entry->MDEntryType == EnumMDEntryType::ImpliedOffer)
				p.side = BookSide::ImpAsk;
	
			p.size = entry->MDEntrySize;
			p.seq = entry->RptSeq;
			p.secid = entry->SecurityID;
			
			Obm.processPxl(p, index);

			if (_DEBUGMODE)
			{
				cout << "[KtnCmeMdpV5 DEBUG] MD46 secid=" << entry->SecurityID << " index=" << index << endl;
				Obm.books[index].print();
			
				cout << "[KtnCmeMdpV5 DEBUG] 46 secid=" << Obm.books[index].SecId 
					<<" Bid=" <<  Obm.books[index].Bid 
					<<" Ask=" <<  Obm.books[index].Ask 
					<< endl;
			}			
		}

        buf += grp->blockLength;
        bytesread += grp->blockLength;
    }

	for (size_t i = 0; i < updatedIndices.size(); ++i)
	{
		const int index = updatedIndices[i];
		Obm.books[index].recvTime = recvTime;
		_callback->onMktDataBidAsk(Obm.books[index]);
	}

	updatedIndices.clear();

    if (bytesread >= iMsgLen)
    	return bytesread;

    auto grp2 =  reinterpret_cast<const groupSize8Byte*>(buf);
    buf += sizeof(groupSize8Byte);

    bytesread += sizeof(groupSize8Byte);

    if (grp2->numInGroup > 0)
    {
    	 bytesread += (grp2->numInGroup * 24);
    	 buf += (grp2->numInGroup * 24);
    }

    return bytesread;
}

int KtnCmeMdpV5::ProcessMDIncrementalRefreshTradeSummary48(const char* buf, const int iMsgLen, const uint64_t recvTime)
{
	buf += sizeof(MDIncrementalRefreshTradeSummary48);
	int bytesread = 11; //MDIncrementalRefreshTradeSummary48 blocksize = 11

	auto grp = reinterpret_cast<const groupSize*>(buf);
	buf += sizeof(groupSize);
	bytesread += sizeof(groupSize);

	 for (uint8_t i = 0; i < grp->numInGroup; i++)
	 {
		auto entry = reinterpret_cast<const MDTradeEntry *>(buf);
		buf +=  grp->blockLength;

		if (Obm.IndexOf(entry->SecurityID) >= 0)
		{
			int index = Obm.IndexOf(entry->SecurityID);
			Obm.books[index].Last = PriceConverters::FromMantissa9ToPrice(entry->MDEntryPx.mantissa);
			Obm.books[index].LastSize = entry->MDEntrySize;
			Obm.books[index].LastAgressor = entry->AggressorSide;
			Obm.books[index].RptSeq = entry->RptSeq;
			Obm.books[index].recvTime = recvTime;
	
			//_trk.startMeasurement("cbOnMktData");
			_callback->onMktDataTrade(Obm.books[index]);
		}
		
		bytesread += 32; //grp->blockLength;

//		ostringstream oss;
//		oss <<"TRADE " << i << "/" << nbr << " px=" << entry->MDEntryPx.mantissa << " tradesize=" << entry->MDEntrySize << " secid=" << entry->SecurityID ;
//		LogMe(oss.str(),LogLevel::INFO);
	}

	auto grp2 =  reinterpret_cast<const groupSize8Byte*>(buf);
	buf += sizeof(groupSize8Byte);
	bytesread += sizeof(groupSize8Byte);

	//16 is blocklength of trade id group
	bytesread += (grp2->numInGroup * 16);
	buf += (grp2->numInGroup * 16);

	return bytesread;
}


void KtnCmeMdpV5::ProcessSecurityStatus30(const char* buf, const int iMsgLen, const uint64_t recvTime)
{
	//preopen = PreOpen = 21,
    auto msg = reinterpret_cast<const SecurityStatus30*>(buf);

    // string szStatus = Mdp3MessagePrinter::PrintStatus(msg->SecurityTradingStatus);

    // ostringstream oss;
    // oss << "STATUS FOR GRP=" << msg->SecurityGroup << " STATUS=" << szStatus << " SECID=" << msg->SecurityID;

	//LogLevel::Enum level = LogLevel::INFO;

    // if (msg->SecurityTradingStatus == EnumSecurityTradingStatus::PreOpen)
    // {
    // 	string szgrp = msg->SecurityGroup;
    // 	level = LogLevel::WARN;
    // }

	//********NOTE: You should comment this out for max performance. This is here for convenience
    // if (msg->SecurityTradingStatus == EnumSecurityTradingStatus::TradingHalt)
	// 	level = LogLevel::ERROR;
	// LogMe(oss.str(), level);
	//************************** */

	BookSecurityStatus bs;
	bs = static_cast<BookSecurityStatus>(msg->SecurityTradingStatus);

	BookSecurityTradingEvent bte;
	bte = static_cast<BookSecurityTradingEvent>(msg->SecurityTradingEvent);

	if ((msg->SecurityID < Mdp3::INT32_NULL) && (msg->SecurityID > 0))
	{
		_callback->onMktDataSecurityStatus(msg->SecurityID, bs, bte, recvTime);
		return;
	}

	for(int i = 0; i < Obm.Size(); i++)
	{
		if (strncmp(Obm.productGroups[i].c_str(), msg->SecurityGroup, sizeof(msg->SecurityGroup)) == 0)
		{
			_callback->onMktDataSecurityStatus(Obm[i].SecId, bs, bte, recvTime);
			//break;
		}
	}

	// if (Obm.IndexOf(msg->SecurityID) >= 0)
	// {
	// 	int index = Obm.IndexOf(msg->SecurityID);

	// 	BookSecurityStatus bs;
	// 	bs = static_cast<BookSecurityStatus>(msg->SecurityTradingStatus);

	// 	BookSecurityTradingEvent bte;
	// 	bte = static_cast<BookSecurityTradingEvent>(msg->SecurityTradingEvent);

	// 	_callback->onMktDataSecurityStatus(msg->SecurityID, bs, bte);
	// }
}

/*
 * This is a top of book snapshot. We will use this to build our book.
	Note that the snapshot is the initial phase of bookbuilding and is not as latency sensitive as the incremental updates.
	We will use this to build our book and then switch to incremental updates.
 */
int KtnCmeMdpV5::ProcessSnapshotFullRefresh52(const char* buf, const int iMsgLen)
{
	int bytesread = 0;

	int blocklen = 59;
    auto msg = reinterpret_cast<const SnapshotFullRefresh52*>(buf);
    buf += blocklen;
    bytesread += blocklen;


    auto grp = reinterpret_cast<const groupSize*>(buf);
    buf += 3;//sizeof(groupSize);
    bytesread += 3;//sizeof(groupSize);

    int grpBlockLen = 22;

	int index = Obm.IndexOf(msg->SecurityID);
	//bool replace = false;

	// if (_DEBUGMODE && index >= 0)
	// 	cout << "[KtnCmeMdpV5 DEBUG] " << msg->SecurityID << ": SnapshotFullRefresh52" 
	// 		<<" index=" << index << " numsecs=" << Obm.Size() << " rptseq=" << msg->RptSeq << endl;

	
	if (index >= 0 && (msg->RptSeq > Obm.books[index].RptSeq))
	{
		if (_DEBUGMODE)
		{
			cout << "[KtnCmeMdpV5 DEBUG] REPLACE " << Obm.books[index].SecId << ": SnapshotFullRefresh52" 
			<< " secid=" << Obm.books[index].SecId << " index=" << index << " numsecs=" << Obm.Size() << " rptseq=" << Obm.books[index].RptSeq << endl;
		}

		DepthBook book;
		book.Index = index;
		book.SecId = msg->SecurityID;
		book.RptSeq = msg->RptSeq;
		book.SecStatus = msg->SecurityTradingStatus;

		for (uint8_t i = 0; i < grp->numInGroup; i++)
		{
			auto entry = reinterpret_cast<const SnapshotFullRefreshEntry*>(buf);

			ProcessSnapshotEntry(entry, msg->SecurityID, msg->RptSeq, book);

			buf += grpBlockLen;// grp->blockLength;
			bytesread += grpBlockLen;
		}

		Obm.SnapshotReceived(book);
		//Obm.SnapshotReceived(book.SecId, book.Bids, book.Asks, book.iBids, book.iAsks, book.RptSeq);
		
		if (_DEBUGMODE)
		{
			cout << "[KtnCmeMdpV5 DEBUG] Printing index=" << index << " secid=" << book.SecId 
			<< " bids=" << book.Bids.size() << " asks=" << book.Asks.size() << " iBids=" << book.iBids.size() << " iAsks=" << book.iAsks.size() << endl;

			Obm.books[index].print();

			cout << "[KtnCmeMdpV5 DEBUG] 52 secid=" << Obm.books[index].SecId 
				<<" Bid=" <<  Obm.books[index].Bid 
				<<" Ask=" <<  Obm.books[index].Ask 
				<< endl;
		}

		
		_callback->onMktDataBidAsk(Obm.books[index]);
	}
	else
	{
		//just need to increment bytes here:
		for (uint8_t i = 0; i < grp->numInGroup; i++)
		{
			buf += grpBlockLen;// grp->blockLength;
			bytesread += grpBlockLen;
		}
	}
	
    return bytesread;
}

void KtnCmeMdpV5::ProcessSnapshotEntry(const SnapshotFullRefreshEntry* entry,
		int32_t secid, uint32_t rptseq, DepthBook& book)
{
	pxl p;
	p.act = BookAction::Change;
	p.lvl = entry->MDPriceLevel;
	p.px = PriceConverters::FromMantissa9ToPrice(entry->MDEntryPx.mantissa);
	p.size = entry->MDEntrySize;
	p.seq = rptseq;
	p.secid = secid;

	switch (entry->MDEntryType)
    {
        case EnumMDEntryType::Bid:
			p.side = BookSide::Bid;
			book.Bids[entry->MDPriceLevel-1] = p;
			//cout << "BOOK BIDS lvl=" << (int)entry->MDPriceLevel << " px=" << p.px << " size=" << book.Bids.size() << endl;
            break;
        case EnumMDEntryType::Offer:
			p.side = BookSide::Ask;
			book.Asks[entry->MDPriceLevel-1] = p;
            break;
        case EnumMDEntryType::ImpliedBid:
			p.side = BookSide::ImpBid;
			book.iBids[entry->MDPriceLevel-1] = p;
            break;
        case EnumMDEntryType::ImpliedOffer:
			p.side = BookSide::ImpAsk;
			book.iAsks[entry->MDPriceLevel-1] = p;
            break;
		
        default:
            // Handle unknown type
            break;
	}

}

void KtnCmeMdpV5::PrintStats() {

	LogMe("*****KTN CME MDP3 LATENCY STATS*****",LogLevel::WARN);
	ostringstream oss;
	oss << "SEQNUMS: A=" << _LAST_SEQa << " B=" << _LAST_SEQb ;
	LogMe(oss.str(), LogLevel::INFO);

	_trk.printMeasurements();

	EfPrintStats();

}

void KtnCmeMdpV5::AddSymbols(vector<string> symbols)
{
	// int i = 0;
	// for (auto symb : symbols)
	// {
	// 	AddSymbol(symb, i);
	// 	i++;
	// }
}

void KtnCmeMdpV5::AddSymbols(vector<instrument> legs)
{
	for (auto leg : legs)
	{
		AddSymbol(leg.symbol, leg.index, leg.secid);
	}
}

void KtnCmeMdpV5::AddSymbol(const string &symb, int index, uint32_t secId)
{
	LogMe("ADDING SYMBOL FOR FULL DEPTH & TOP: " + symb + " INDEX=" + std::to_string(index), LogLevel::INFO);
	CmeSecDef secdef = CmeSecMaster::instance().getSecDef(symb);

	if (secdef.secid == 0)
	{
		for (int i = 0; i < 10; i++)
			LogMe("ERROR!!!!! SECDEF NOT FOUND FOR "+ symb, LogLevel::ERROR);
		return;
	}

	if (Obm.IndexOf(secdef.secid) >= 0)
	{
		LogMe("SECDEF ALREADY REGISTERED: " + std::to_string(secdef.secid), LogLevel::WARN);
		return;
	}
	
	if (_DEBUGMODE)
	{
		cout << "[KtnCmeMdpV5 DEBUG] ADDING SYMBOL: " << secdef.symbol << " SECID=" << secdef.secid << " INDEX=" << index << endl;
	}

	Obm.Add(secdef.secid, index, secdef.secgroup);
	
	int index_check = Obm.IndexOf(secdef.secid);

	ostringstream oss;
	oss << "  ALGOMD INDEX " << index_check << ": SYMB=" << secdef.symbol << " SECID=" << Obm[index_check].SecId << " TOTAL SYMBS=" << Obm.Size();
	LogMe(oss.str(), LogLevel::INFO);

#if !AKL_TEST_MODE
	AddChannelFilter(secdef, "IA");
	AddChannelFilter(secdef,"SA");
#endif
}

void KtnCmeMdpV5::AddChannelFilter(const CmeSecDef& secdef, const string& feedtype)
{
	string chid = _Channels.GetChid(secdef.product);
	int iid = atoi(chid.c_str());
	std::pair<int, string> feed_pair = std::make_pair(iid, feedtype);
	auto it = std::find(_chids.begin(), _chids.end(), feed_pair);

	if (it != _chids.end())
	{
		ostringstream oss;
		oss <<"FILTER ALREADY FOR CHID " << chid << " FOR PRODUCT " << secdef.product;
		LogMe(oss.str(),LogLevel::INFO);
		return;
	}
	_chids.push_back(feed_pair);

	Channel chan = _Channels.GetChannel(chid);

	bool isA = true;
	string connid = chan.id + feedtype;// + ab;

	LogMe("PRODUCT " + secdef.product + ": CREATING UDP FILTER: CONNID=" + connid + " CHID=" + chid + " FEED=" + feedtype,LogLevel::INFO );

	Connection conn = chan.connections[connid];

	//this goes right to the efvi feed inherited from the base class
	apply_filter(connid, conn.ip, atoi(conn.port.c_str()), isA);

	uint32_t multicastAddr;
	if (inet_pton(AF_INET, conn.ip.c_str(), &multicastAddr) == 1)
	{
		if (index >= 0 && index < 2)
		{
			seqTracker[index].first = multicastAddr;
			seqTracker[index].second = 0; // Reset sequence number for this multicast address
			LOGINFO("Adding multicast address {} {} at index {}", conn.ip, multicastAddr, index);
			++index;
		}
		else
		{
			LOGWARN("Invalid index {} for multicast address {}", index, conn.ip);
		}
	}
	else
	{
		LOGWARN("Invalid IP address format: {}", conn.ip);
	}
}


