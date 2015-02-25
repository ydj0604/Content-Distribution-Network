#include "cpprest/http_listener.h"
#include "CDNMetaReceiver.h"
#include <iostream>
#include "CDN_Node.h"

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;
using namespace json;

CDNMetaReceiver* CDNMetaReceiver::m_instance = NULL;

CDNMetaReceiver::CDNMetaReceiver(utility::string_t url) : m_listener(url) {
	m_listener.support(methods::DEL, std::bind(&CDNMetaReceiver::handle_delete, this, std::placeholders::_1));
}

void CDNMetaReceiver::initialize(const string_t& address, CDN_Node* cdn) {
	CDNMetaReceiver* instance = CDNMetaReceiver::getInstance();
	uri_builder uri(address);
	uri.append_path(U("cdn/cache"));
	instance = new CDNMetaReceiver(uri.to_uri().to_string());
	instance->setCDN(cdn);
	instance->open().wait();
	ucout << utility::string_t(U("CDNMetaReceiver is listening for invalidate requests at: ")) << uri.to_uri().to_string() << std::endl;
}

void CDNMetaReceiver::shutDown() {
	CDNMetaReceiver *instance = CDNMetaReceiver::getInstance();
	if(instance == NULL)
		return;
	instance->close().wait();
	delete instance;
}

void CDNMetaReceiver::handle_delete(http_request message) {
	/*
	-when a file is updated, Meta sends messages to CDNs to invalidate/delete the outdated files

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
	*/
	ucout <<  message.to_string() << endl;
	auto paths = uri::split_path(uri::decode(message.relative_uri().path()));
	for(int i=0; i<paths.size(); i++)
		ucout <<  paths[i] << endl;
	//TODO: delete the file which can be located/identified by paths
	message.reply(status_codes::OK, U("testing"));
}
