#include "cpprest/http_listener.h"
#include "MetaCDNReceiver.h"
#include <iostream>
#include "MetaServer.h"
#include "../Shared.h"

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;
using namespace json;

MetaCDNReceiver* MetaCDNReceiver::m_updateInstance = NULL;
MetaCDNReceiver* MetaCDNReceiver::m_deleteInstance = NULL;
MetaCDNReceiver* MetaCDNReceiver::m_registerInstance = NULL;

MetaCDNReceiver::MetaCDNReceiver(utility::string_t url, int type) : m_listener(url) { //1=update, 2=delete, 3=register
	if(type==1)
		m_listener.support(methods::POST, std::bind(&MetaCDNReceiver::handle_post, this, std::placeholders::_1));
	else if(type==2)
		m_listener.support(methods::DEL, std::bind(&MetaCDNReceiver::handle_delete, this, std::placeholders::_1));
	else if(type==3)
		m_listener.support(methods::POST, std::bind(&MetaCDNReceiver::handle_register, this, std::placeholders::_1));
}

void MetaCDNReceiver::initialize(const string_t& address, MetaServer* meta) {
	MetaCDNReceiver* updateInstance = MetaCDNReceiver::getUpdateInstance();
	uri_builder updateUri(address);
	updateUri.append_path(U("meta/update"));
	updateInstance = new MetaCDNReceiver(updateUri.to_uri().to_string(), 1);
	updateInstance->setMeta(meta);
	updateInstance->open().wait();
	ucout << utility::string_t(U("MetaCDNReceiver is listening for update requests at: ")) << updateUri.to_uri().to_string() << std::endl;

	MetaCDNReceiver* deleteInstance = MetaCDNReceiver::getDeleteInstance();
	uri_builder deleteUri(address);
	deleteUri.append_path(U("meta/delete"));
	deleteInstance = new MetaCDNReceiver(deleteUri.to_uri().to_string(), 2);
	deleteInstance->setMeta(meta);
	deleteInstance->open().wait();
	ucout << utility::string_t(U("MetaCDNReceiver is listening for delete requests at: ")) << deleteUri.to_uri().to_string() << std::endl;

	MetaCDNReceiver* registerInstance = MetaCDNReceiver::getRegisterInstance();
	uri_builder registerUri(address);
	registerUri.append_path(U("meta/register"));
	registerInstance = new MetaCDNReceiver(registerUri.to_uri().to_string(), 3);
	registerInstance->setMeta(meta);
	registerInstance->open().wait();
	ucout << utility::string_t(U("MetaCDNReceiver is listening for register requests at: ")) << registerUri.to_uri().to_string() << std::endl;
}

void MetaCDNReceiver::shutDown() {
	MetaCDNReceiver *updateInstance = MetaCDNReceiver::getUpdateInstance(),
					*deleteInstance = MetaCDNReceiver::getDeleteInstance(),
					*registerInstance = MetaCDNReceiver::getRegisterInstance();
	if(updateInstance==NULL || deleteInstance==NULL || registerInstance==NULL)
		return;
	updateInstance->close().wait();
	deleteInstance->close().wait();
	registerInstance->close().wait();
	delete updateInstance;
	delete deleteInstance;
	delete registerInstance;
}

void MetaCDNReceiver::handle_register(http_request message) {
	/*
	-when a new CDN joins, it has to register itself to Meta Server

	JSON Format
	Request
	{
		"Type": 0,
		"IP": "1.1.1.1", //the sender CDN's IP address
		"Lat": 23.00, //the sender CDN's location
		"Lng": 148.12
	}
	Response
	{
		"ID": 1 //the assigned id for the cdn
	}

	*/
	try {
		int assignedId = -1;
		if(message.headers().content_type()==U("application/json")) {
			json::value jsonObj = message.extract_json().get();
			string ipAddr = utility::conversions::to_utf8string(jsonObj.at(U("IP")).as_string());
			//TODO: validate ip address
			Address cdnAddr(make_pair(jsonObj.at(U("Lat")).as_double(), jsonObj.at(U("Lng")).as_double()), ipAddr);
			assignedId = m_meta->addCdnAddr(cdnAddr);
			json::value respFinal = json::value::object();
			respFinal[U("ID")] = json::value::number(assignedId);
			message.reply(assignedId!=-1? status_codes::OK : status_codes::NotFound, respFinal);
		} else {
			message.reply(status_codes::Forbidden, U("Json object is required"));
		}
	} catch(json::json_exception &e) {
		message.reply(status_codes::Forbidden, U("Invalid json object"));
		return;
	}
	return;
}

void MetaCDNReceiver::handle_post(http_request message) {
	/*
	Use cases:
	1. when CDN pulls a file from FSS (syncdown flow)
	2. when CDN updates an existing file (syncup flow; need invalidation process)
	3. when CDN creates a new file and stores in FSS and itself

	JSON Format
	Request
	{
		"Type": 0, // 0=CDN pulls a file from FSS, 1=CDN updates a file (+invalidation process), 2=CDN creates a new file and stores in FSS
		"FileName": "a.txt",
		"FileHash": "ahash", //could be empty string when Type=0
		"IP": "1.1.1.1", //the sender CDN's IP address
		"Lat": 23.00, //the sender CDN's location
		"Lng": 148.12
	}

	Response: status OK or Forbidden (no json object included)
	*/

	try {
		if(message.headers().content_type()==U("application/json")) {
			json::value jsonObj = message.extract_json().get();
			string ipAddr = utility::conversions::to_utf8string(jsonObj.at(U("IP")).as_string());
			//TODO: validate ip address
			Address cdnAddr(make_pair(jsonObj.at(U("Lat")).as_double(), jsonObj.at(U("Lng")).as_double()), ipAddr);
			string fileName = utility::conversions::to_utf8string(jsonObj.at(U("FileName")).as_string());
			string fileHash = utility::conversions::to_utf8string(jsonObj.at(U("FileHash")).as_string());
			int result;
			if(jsonObj.at(U("Type")).as_integer() == 0) {
				//add cdn to the meta entry
				//file hash is not needed here
				result = m_meta->addCdnToMetaEntry(fileName, cdnAddr);
			} else if(jsonObj.at(U("Type")).as_integer() == 1) {
				//renew the meta entry
				//TODO: validate file hash
				vector<Address> newCdnList;
				newCdnList.push_back(cdnAddr);
				result = m_meta->updateMetaEntry(fileName, fileHash, newCdnList);
				//TODO : now send requests to rest of CDNS to invalidate !!
			} else if(jsonObj.at(U("Type")).as_integer() == 2) {
				//add a new meta entry
				//TODO: validate file hash
				vector<Address> newCdnList;
				newCdnList.push_back(cdnAddr);
				result = m_meta->addNewMetaEntry(fileName, fileHash, newCdnList);
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
	return;
}

void MetaCDNReceiver::handle_delete(http_request message) {
	/*
	Use cases:
	1. when CDN deletes a file from itself to store some other file (syndown+syncup)

	JSON Format
	Request
	{
		"FileName": "a.txt",
		"IP": "1.1.1.1", //the sender CDN's IP address
		"Lat": 23.00, //the sender CDN's location
		"Lng": 148.12
	}

	Response: status OK or Forbidden (no json object included)
	*/
	try {
		int result;
		if(message.headers().content_type()==U("application/json")) {
			json::value jsonObj = message.extract_json().get();
			string ipAddr = utility::conversions::to_utf8string(jsonObj.at(U("IP")).as_string());
			//TODO: validate ip address
			Address cdnAddr(make_pair(jsonObj.at(U("Lat")).as_double(), jsonObj.at(U("Lng")).as_double()), ipAddr);
			string fileName = utility::conversions::to_utf8string(jsonObj.at(U("FileName")).as_string());
			result = m_meta->deleteCdnFromMetaEntry(fileName, cdnAddr);
			message.reply(result==0? status_codes::OK : status_codes::NotFound, result==0? U("Deleted successfully") : U("Delete failed"));
		} else {
			message.reply(status_codes::Forbidden, U("Json object is required"));
		}
	} catch(json::json_exception &e) {
		message.reply(status_codes::Forbidden, U("Invalid json object"));
		return;
	}
	return;
}
