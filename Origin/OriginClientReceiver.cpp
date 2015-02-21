#include "cpprest/http_listener.h"
#include "OriginClientReceiver.h"
#include <iostream>
#include "OriginServer.h"
#include "../Shared.h"

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;
using namespace json;

OriginClientReceiver* OriginClientReceiver::m_instance = NULL;

OriginClientReceiver::OriginClientReceiver(utility::string_t url) : m_listener(url) {
	m_listener.support(methods::POST, std::bind(&OriginClientReceiver::handle_post, this, std::placeholders::_1));
}

void OriginClientReceiver::initialize(const string_t& address, OriginServer* origin) {
	OriginClientReceiver* instance = OriginClientReceiver::getInstance();
	uri_builder uri(address);
	uri.append_path(U("origin/sync"));
	instance = new OriginClientReceiver(uri.to_uri().to_string());
	instance->setOrigin(origin);
	instance->open().wait();
	ucout << utility::string_t(U("Listening for requests at: ")) << uri.to_uri().to_string() << std::endl;
	return;
}

void OriginClientReceiver::shutDown() {
	OriginClientReceiver* instance = OriginClientReceiver::getInstance();
	if(instance==NULL)
		return;
	instance->close().wait();
	delete instance;
	return;
}

void OriginClientReceiver::handle_post(http_request message) {
	/* JSON Format
	Request
	{
		"Type": 0, //0=syncup 1=syncdown
		"FileList": [{"Name": "a.txt" ,"Hash": "ahash"}, {"Name": "b.txt" ,"Hash": "bhash"}],
		"IP": "1.1.1.1",
		"Lat": 23.00,
		"Lng": 148.12
	}

	Response
	{
		"Type": 0, //0=syncup 1=syncdown
		"FileList": [{"Name": "a.txt" ,"Address": "1.1.1.1"}, {"Name": "b.txt" ,"Address": "2.2.2.2"}]
	}
	*/

	try {
		if(message.headers().content_type()==U("application/json")) {
			json::value jsonObj = message.extract_json().get();
			Address clientAddr(make_pair(jsonObj.at(U("Lat")).as_double(), jsonObj.at(U("Lng")).as_double()), utility::conversions::to_utf8string(jsonObj.at(U("IP")).as_string()));
			vector< pair<string, string> > clientList;

			if(jsonObj.at(U("Type")).as_integer() == 0) { //sync up
				//...
				message.reply(status_codes::OK, U("Good"));
			} else if(jsonObj.at(U("Type")).as_integer() == 1) { //sync down
				json::value& list = jsonObj.at(U("FileList"));
				for(auto& fileObj : list.as_array()) {
					clientList.push_back(make_pair(utility::conversions::to_utf8string(fileObj.at(U("Name")).as_string()), utility::conversions::to_utf8string(fileObj.at(U("Hash")).as_string())));
				}
				vector< pair<string, Address> > resultList = m_origin->getListOfFilesDownload(clientList, clientAddr); //internal computation including meta server
				//now construct json from resultList and reply to the message
				json::value respList = json::value::array();
				for(int i=0; i<resultList.size(); i++) {
					json::value currFileObj = json::value::object();
					currFileObj[U("Name")] = json::value::string(U(resultList[i].first));
					currFileObj[U("Address")] = json::value::string(U(resultList[i].second.ipAddr));
					respList[i] = currFileObj;
				}
				json::value respFinal = json::value::object();
				respFinal[U("Type")] = json::value::number(1);
				respFinal[U("FileList")] = respList;
				message.reply(status_codes::OK, respFinal);
			} else { //undefined
				message.reply(status_codes::Forbidden, U("Undefined type"));
			}
		} else {
			message.reply(status_codes::Forbidden, U("Json object is required"));
		}
	} catch(json::json_exception &e) {
		message.reply(status_codes::Forbidden, U("Invalid json object"));
		return;
	}
	return;
}


