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
	ucout << utility::string_t(U("CDNMetaReceiver is listening for requests at: ")) << uri.to_uri().to_string() << std::endl;
}

void CDNMetaReceiver::shutDown() {
	CDNMetaReceiver *instance = CDNMetaReceiver::getInstance();
	if(instance == NULL)
		return;
	instance->close().wait();
	delete instance;
}

void CDNMetaReceiver::handle_delete(http_request message) {
	auto paths = uri::split_path(uri::decode(message.relative_uri().path()));
	string filePath = "";
	for(int i=0; i<paths.size(); i++) {
		if(i==paths.size()-1)
			filePath += paths[i];
		else
			filePath += paths[i]+"/";
	}

	if(m_cdn==NULL) {
		message.reply(status_codes::NotFound, U("CDN server is not set"));
		return;
	}
	//if(m_cdn->look_up_and_remove_storage(filePath ,1)) //remove the file
	//	message.reply(status_codes::OK, U("delete succeeded"));
	//else
		message.reply(status_codes::NotFound, U(filePath + " is not found"));
}
