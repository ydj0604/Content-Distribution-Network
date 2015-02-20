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
	m_listener.support(methods::GET, std::bind(&OriginClientReceiver::handle_get, this, std::placeholders::_1));
}

void OriginClientReceiver::initialize(const string_t& address) {
	OriginClientReceiver* instance = OriginClientReceiver::getInstance();
	uri_builder uri(address);
	uri.append_path(U("origin/sync"));
	instance = new OriginClientReceiver(uri.to_uri().to_string());
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

void OriginClientReceiver::handle_get(http_request message) {
	//ucout <<  message.to_string() << endl;
	//message.reply(status_codes::OK, U("Hello, World!"));

	/* JSON Format
	{
		"Type": 0, //0=syncup 1=syncdown
		"List": [{"Name": "a.txt" ,"Hash": "ahash"}, {"Name": "b.txt" ,"Hash": "bhash"}],
		"IP": "1.1.1.1",
		"Lat": 23.00,
		"Lng": 148.12
	}
	*/

	try {
		if(message.headers().content_type()==U("application/json")) {
			json::value jsonObj = message.extract_json().get();
			json::value& type = jsonObj.at(U("Type"));
			json::value& ip = jsonObj.at(U("IP"));
			json::value& lat = jsonObj.at(U("Lat"));
			json::value& lng = jsonObj.at(U("Lng"));

			//std::string str = utility::conversions::to_utf8string(fileHashJson.as_string());
			Address clientAddr(make_pair(lat.as_double(), lng.as_double()), utility::conversions::to_utf8string(ip.as_string()));
			vector< pair<string, string> > clientList;

			if(type.as_integer() == 0) { //sync up
				//...
			} else if(type.as_integer() == 1) { //sync down
				json::value& list = jsonObj.at(U("List"));
				for(auto itr = list.begin(); itr!=list.end(); ++itr) {
					json::value& currVal = *itr;
					json::value& fileNameJson = currVal.at(U("Name"));
					json::value& fileHashJson = currVal.at(U("Hash"));
					clientList.push_back(make_pair(utility::conversions::to_utf8string(fileNameJson.as_string()), utility::conversions::to_utf8string(fileHashJson.as_string())));
				}
			} else { //undefined
				message.reply(status_codes::Forbidden, U("Undefined type"));
				return;
			}
			vector< pair<string, Address> > resultList = m_origin->getListOfFilesDownload(clientList, clientAddr);

			//construct json from result List and reply to the message
		}
	} catch(json::json_exception &e) {
		message.reply(status_codes::Forbidden, U("Invalid json object"));
		return;
	}
}
