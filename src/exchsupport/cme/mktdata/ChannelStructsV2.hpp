//============================================================================
// Name        	: ChannelStructsV2.hpp
// Author      	: centos
// Version     	:
// Copyright   	: Copyright (C) 2021 Katana Financial
// Date			: Dec 7, 2024 4:40:14 PM
//============================================================================


#ifndef SRC_CME_CHANNELSTRUCTSV2_HPP_
#define SRC_CME_CHANNELSTRUCTSV2_HPP_

#include <cstddef>
#include <cassert>
#include <external/rapidxml/rapidxml.hpp>
#include <KT01/Core/Log.hpp>
#include <KT01/Core/StringSplitter.hpp>
#include <sstream>
#include <fstream>
#include <vector>

using namespace rapidxml;
using namespace std;

namespace KTN {
namespace Feeds {
namespace CME {

struct ProductGroup {
    string code;
};

struct Connection {
    string id;
    string feed_type;
    string desc;
    string ip;
    string host_ip;
    string port;
    string feed;
    string protocol;

    string Print() const {
        ostringstream oss;
        oss << "id=" << id << "| "
            << "feed-type=" << feed_type << "| "
            << "desc=" << desc << "| "
            << "ip=" << ip << "| "
            << "host-ip=" << host_ip << "| "
            << "port=" << port << "| "
            << "feed=" << feed << "| "
            << "protocol=" << protocol;
        return oss.str();
    }
};

struct Product {
    string code;
    ProductGroup group;
};

struct Channel {
    string id;
    string label;
    std::unordered_map<string, Product> products;
    std::unordered_map<string, Connection> connections;
};

struct Configuration {
    string environment;
    string updated;
    std::unordered_map<string, Channel> channels;

    void LogMe(const string& msg, LogLevel::Enum level = LogLevel::INFO) const 
    {
        KT01_LOG_INFO(__CLASS__, msg);
    }

    void Info() const {
        ostringstream oss;
        oss << "INFO: ENV=" << environment << " FILEDATE=" << updated
            << " CHANNELS=" << channels.size();
        LogMe(oss.str(), LogLevel::INFO);
    }

    void Print(const string& chid) const {
        auto it = channels.find(chid);
        if (it == channels.end()) {
            LogMe("CANNOT FIND CHANNEL " + chid + " IN CONFIG. ENV=" + environment, LogLevel::ERROR);
            return;
        }

        LogMe("************** CHANNEL " + chid + " **************", LogLevel::INFO);
        Info();

        const auto& channel = it->second;
        LogMe("PRODUCTS:", LogLevel::WARN);
        for (const auto& [_, product] : channel.products) {
            LogMe("  code=" + product.code + " group=" + product.group.code);
        }

        LogMe("CONNECTIONS:", LogLevel::WARN);
        for (const auto& [_, connection] : channel.connections) {
            LogMe(" " + connection.Print());
        }
    }

    Channel GetChannel(const string& chid) const {
        auto it = channels.find(chid);
        if (it != channels.end()) {
            return it->second;
        }
        return {};
    }

    string GetChid(string root)//, KOrderProdType::Enum prodtype)
	{
		ostringstream oss;
		oss << "GETTING CHANNEL FOR ROOT " << root;
		LogMe(oss.str(), LogLevel::WARN);

		for (auto chan : channels )
		{
			//KOrderProdType::Enum thisprodtype = chan.second.prodtype;
			//thisprodtype = chan.second.prodtype.substr(0,  chan.second.prodtype.length());
			for (auto prod : chan.second.products)
			{
				string thiscode = "";
				thiscode = prod.second.code.substr(0,  prod.second.code.length());

				if (root.compare(thiscode)== 0)
				{
					//new: check if futs or options..
					//if (prodtype == thisprodtype)// .compare(thisprodtype) == 0)
					{
						ostringstream oyy;
						oyy << "Channel for product " << root << " found. CHID=" << " " << chan.second.id ;//<< " PRODTYPE=" << thisprodtype;
						LogMe(oyy.str(), LogLevel::INFO);

						string chid = chan.second.id;
						return chid;
					}
				}
			}
		}

		LogMe("ERROR!! ROOT " + root + " NOT FOUND: ",LogLevel::ERROR);
		return "0";
	}

    void AddChannel(const string& id, const string& label) {
        channels[id] = {id, label};
        LogMe("CHANNEL " + id + " ADDED.");
    }

    void AddProduct(const string& channel_id, const string& code, const ProductGroup& pg) {
        auto it = channels.find(channel_id);
        if (it == channels.end()) {
            LogMe("ADD PROD: CHANNEL ID DOES NOT EXIST: " + channel_id, LogLevel::ERROR);
            return;
        }

        it->second.products[code] = {code, pg};
    }

    void AddConnection(const string& channel_id, const Connection& conn) {
        auto it = channels.find(channel_id);
        if (it == channels.end()) {
            LogMe("ADD CONN: CHANNEL ID DOES NOT EXIST: " + channel_id, LogLevel::ERROR);
            return;
        }

        LogMe("ADDING CONNECTION WITH ID=" + conn.id, LogLevel::INFO);
        it->second.connections[conn.id] = conn;
    }

    void Load(const string& filename) {
        ifstream file(filename);
        vector<char> buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        buffer.push_back('\0');

        xml_document<> doc;
        doc.parse<0>(buffer.data());

        walk(doc.first_node());
        //LogMe("CHANNEL LOAD COMPLETE: " + environment, LogLevel::INFO);
        Info();
    }

private:
    void walk(const xml_node<>* node, string chid = "") {
        if (!node) return;

        string nodename = node->name();
        if (nodename == "configuration") {
            ParseConfigNode(node->first_attribute());
        } else if (nodename == "channel") {
            chid = ParseChannelNode(node->first_attribute());
        } else if (nodename == "product") {
            ParseProductNode(node->first_attribute(), chid);
        } else if (nodename == "connection") {
            ParseConnectionNode(node, chid);
        }

        for (auto* child = node->first_node(); child; child = child->next_sibling()) {
            walk(child, chid);
        }
    }

    void ParseConfigNode(const xml_attribute<>* attr) {
        while (attr) {
            string name = attr->name();
            string value = attr->value();

            if (name == "environment") environment = value;
            if (name == "updated") updated = value;

            attr = attr->next_attribute();
        }

        LogMe("LOADING: " + environment + " UPDATED: " + updated, LogLevel::INFO);
    }

    string ParseChannelNode(const xml_attribute<>* attr) {
        Channel channel;
        while (attr) {
            string name = attr->name();
            string value = attr->value();

            if (name == "id") channel.id = value;
            if (name == "label") channel.label = value;

            attr = attr->next_attribute();
        }

        channels[channel.id] = channel;
        //LogMe("CHANNEL " + channel.id + ": " + channel.label, LogLevel::INFO);
        return channel.id;
    }

    void ParseProductNode(const xml_attribute<>* attr, const string& chid) {
        auto it = channels.find(chid);
        if (it == channels.end()) {
            LogMe("NO CHANNEL ID!", LogLevel::ERROR);
            return;
        }

        Product product;
        while (attr) {
            string name = attr->name();
            string value = attr->value();

            if (name == "code") product.code = value;

            attr = attr->next_attribute();
        }

        it->second.products[product.code] = product;
    }

    void ParseConnectionNode(const xml_node<>* node, const string& chid) {
        auto it = channels.find(chid);
        if (it == channels.end()) {
            LogMe("NO CHANNEL ID!", LogLevel::ERROR);
            return;
        }

        Connection conn;
        for (auto* attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
            string name = attr->name();
            string value = attr->value();

            if (name == "id") conn.id = value;
        }

        for (auto* child = node->first_node(); child; child = child->next_sibling()) {
            string name = child->name();
            string value = child->value();

            if (name == "type") conn.feed_type = value;
            if (name == "ip") conn.ip = value;
            if (name == "host-ip") conn.host_ip = value;
            if (name == "port") conn.port = value;
            if (name == "feed") conn.feed = value;
            if (name == "protocol") conn.protocol = value;
        }

        it->second.connections[conn.id] = conn;
    }
};

} // namespace CME
} // namespace Feeds
} // namespace KTN



#endif /* SRC_CME_CHANNELSTRUCTS_HPP_ */
