#include "cpprest/http_listener.h"
#include "cpprest/http_client.h"
#include "MetaCDNReceiver.h"
#include <iostream>
#include "MetaServer.h"
#include "../Shared.h"
#include <string>

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;
using namespace http::client;
using namespace json;

MetaCDNReceiver* MetaCDNReceiver::m_instance = NULL;

MetaCDNReceiver::MetaCDNReceiver(utility::string_t update_url, utility::string_t delete_url, utility::string_t register_url) :
		m_update_listener(update_url), m_delete_listener(delete_url), m_register_listener(register_url) {
	m_update_listener.support(methods::POST, std::bind(&MetaCDNReceiver::handle_update, this, std::placeholders::_1));
	m_delete_listener.support(methods::DEL, std::bind(&MetaCDNReceiver::handle_delete, this, std::placeholders::_1));
	m_register_listener.support(methods::POST, std::bind(&MetaCDNReceiver::handle_register, this, std::placeholders::_1));
}

void MetaCDNReceiver::initialize(const string_t& address, MetaServer* meta) {
	MetaCDNReceiver* instance = MetaCDNReceiver::getInstance();
	uri_builder updateUri(address);
	updateUri.append_path(U("meta/update"));
	uri_builder deleteUri(address);
	deleteUri.append_path(U("meta/delete"));
	uri_builder registerUri(address);
	registerUri.append_path(U("meta/register"));
	instance = new MetaCDNReceiver(updateUri.to_uri().to_string(), deleteUri.to_uri().to_string(), registerUri.to_uri().to_string());
	instance->setMeta(meta);
	instance->updateOpen().wait();
	instance->deleteOpen().wait();
	instance->registerOpen().wait();
	ucout << utility::string_t(U("MetaCDNReceiver is listening for update requests at: ")) << updateUri.to_uri().to_string() << std::endl;
	ucout << utility::string_t(U("MetaCDNReceiver is listening for delete requests at: ")) << deleteUri.to_uri().to_string() << std::endl;
	ucout << utility::string_t(U("MetaCDNReceiver is listening for register requests at: ")) << registerUri.to_uri().to_string() << std::endl;
}

void MetaCDNReceiver::shutDown() {
	MetaCDNReceiver *instance = MetaCDNReceiver::getInstance();
	if(instance == NULL)
		return;
	instance->updateClose().wait();
	instance->deleteClose().wait();
	instance->registerClose().wait();
	delete instance;
}

void MetaCDNReceiver::handle_update(http_request message) {
	/*
	Use cases:
	0. when CDN pulls a file from FSS (syncdown flow)
	1. when CDN updates an existing file (syncup flow; need invalidation process)
	2. when CDN creates a new file and stores in FSS and itself

	JSON Format
	Request
	{
		"Type": 0, // 0=CDN pulls a file from FSS, 1=CDN updates a file (+invalidation process), 2=CDN creates a new file and stores in FSS
		"FileName": "a.txt",
		"FileHash": "ahash", //could be empty string when Type=0 //only for type 1,2
		"CdnId": 1
		"TimeStamp": "12312312312" //REQUIRED for use case 1 and 2
	}

	Response: status OK or Forbidden (no json object included)
	*/

	try {
		if(message.headers().content_type()==U("application/json")) {
			cout << endl << "---------------"<< endl;
			cout << message.to_string() << endl <<endl;

			json::value jsonObj = message.extract_json().get();
			int cdnId = jsonObj.at(U("CdnId")).as_integer();
			string fileName = utility::conversions::to_utf8string(jsonObj.at(U("FileName")).as_string());
			
			int result;
			if(jsonObj.at(U("Type")).as_integer() == 0) {
				result = m_meta->addCdnToMetaEntry(fileName, cdnId);
			} else if(jsonObj.at(U("Type")).as_integer() == 1) {
				string fileHash = utility::conversions::to_utf8string(jsonObj.at(U("FileHash")).as_string());
				vector<int> newCdnList;
				newCdnList.push_back(cdnId);
				result = m_meta->updateMetaEntry(fileName, fileHash, newCdnList);
				if(result == 0) {
					result = m_meta->updateTimeStamp(fileName, jsonObj.at(U("TimeStamp")).as_string());
				} else {
					cout<<"MetaCDNReceiver::handle_update() - failed to update meta entry"<<endl;
					return;
				}

				//now, send invalidation msgs to other cdns
				unordered_map<int, Address>::const_iterator itr = m_meta->getCdnIdToAddrMap().begin();
				while(itr != m_meta->getCdnIdToAddrMap().end()) {
					if(itr->first == cdnId) {
						++itr;
						continue;
					}
					http_client cdn_client = http_client("http://" + itr->second.ipAddr);
					http_response resp = cdn_client.request(methods::DEL, "cdn/cache"+fileName).get();
					if (resp.status_code() != status_codes::OK) {
						cout<<"MetaCDNReceiver::handle_update() - failed to send invalidation message to "+itr->second.ipAddr<<endl;
					}
					++itr;
				}

			} else if(jsonObj.at(U("Type")).as_integer() == 2) {
				string fileHash = utility::conversions::to_utf8string(jsonObj.at(U("FileHash")).as_string());
				vector<int> newCdnList;
				newCdnList.push_back(cdnId);
				result = m_meta->addNewMetaEntry(fileName, fileHash, newCdnList);
				if(result == 0)
					result = m_meta->addNewTimeStamp(fileName, jsonObj.at(U("TimeStamp")).as_string());
			} else {
				message.reply(status_codes::Forbidden, U("Undefined Type"));
				return;
			}
			message.reply(result==0? status_codes::OK : status_codes::NotFound, result==0? U("Updated successfully") : U("Update failed"));

		} else {
			message.reply(status_codes::Forbidden, U("Json object is required"));
		}
	} catch(json::json_exception &e) {
		message.reply(status_codes::Forbidden, U("Invalid json object"));
		return;
	}
}

void MetaCDNReceiver::handle_delete(http_request message) {
	/*
	Use cases:
	1. when CDN deletes a file from itself to store some other file because of its limited capacity

	JSON Format
	Request
	{
		"FileName": "a.txt",
		"CdnId" : 2
	}

	Response: status OK or Forbidden (no json object included)
	*/
	try {
		int result;
		if(message.headers().content_type()==U("application/json")) {
			cout << endl << "---------------"<< endl;
			cout << message.to_string() << endl <<endl;

			json::value jsonObj = message.extract_json().get();
			int cdnId = jsonObj.at(U("CdnId")).as_integer();
			string fileName = utility::conversions::to_utf8string(jsonObj.at(U("FileName")).as_string());
			result = m_meta->deleteCdnFromMetaEntry(fileName, cdnId);
			message.reply(status_codes::OK, result==0? U("Deleted successfully") : U("Delete failed"));
		} else {
			message.reply(status_codes::Forbidden, U("Json object is required"));
		}
	} catch(json::json_exception &e) {
		message.reply(status_codes::Forbidden, U("Invalid json object"));
		return;
	}
}

void MetaCDNReceiver::handle_register(http_request message) {
	/*
	-when a new CDN joins, it has to register itself to Meta Server

	JSON Format
	Request
	{
		"Type": 0, //0=for cdn, 1=for fss
		"IP": "1.1.1.1:4000", //the sender CDN's IP address + port(listening to incoming requests)
		"Lat": 23.00, //the sender CDN's location
		"Lng": 148.12
	}
	Response
	{
		"CdnId": 1 //the assigned id for the cdn
	}

	*/
	try {

		int assignedId = -1;
		if(message.headers().content_type()==U("application/json")) {
			cout << endl << "---------------"<< endl;
			cout << message.to_string() << endl <<endl;

			json::value jsonObj = message.extract_json().get();
			if(jsonObj.at(U("Type")).as_integer() == 0) {
				string ipAddr = utility::conversions::to_utf8string(jsonObj.at(U("IP")).as_string());
				//TODO: validate ip address
				Address cdnAddr(make_pair(jsonObj.at(U("Lat")).as_double(), jsonObj.at(U("Lng")).as_double()), ipAddr);
				assignedId = m_meta->registerCdn(cdnAddr);
				json::value respFinal = json::value::object();
				respFinal[U("CdnId")] = json::value::number(assignedId);
				message.reply(assignedId!=-1? status_codes::OK : status_codes::NotFound, respFinal);
			} else if(jsonObj.at(U("Type")).as_integer() == 1){
				string ipAddr = utility::conversions::to_utf8string(jsonObj.at(U("IP")).as_string());
				//TODO: validate ip address
				Address fssAddr(make_pair(jsonObj.at(U("Lat")).as_double(), jsonObj.at(U("Lng")).as_double()), ipAddr);
				m_meta->setFssAddr(fssAddr);
				message.reply(status_codes::OK, "FSS registration complete");
			} else {
				message.reply(status_codes::Forbidden, U("Invalid type"));
			}
		} else {
			message.reply(status_codes::Forbidden, U("Json object is required"));
		}
	} catch(json::json_exception &e) {
		message.reply(status_codes::Forbidden, U("Invalid json object"));
		return;
	}
}
