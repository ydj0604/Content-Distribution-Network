#include "cpprest/http_listener.h"
#include "OriginClientReceiver.h"
#include <iostream>

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;

OriginClientReceiver* OriginClientReceiver::m_instance = NULL;

OriginClientReceiver::OriginClientReceiver(utility::string_t url) : m_listener(url) {
	m_listener.support(methods::GET, std::bind(&OriginClientReceiver::handle_get, this, std::placeholders::_1));
}

void OriginClientReceiver::intialize(const string_t& address) {
	OriginClientReceiver* instance = OriginClientReceiver::getInstance();
	uri_builder uri(address);
	uri.append_path(U("client/syncdown"));
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
	ucout <<  message.to_string() << endl;
	message.reply(status_codes::OK, U("Hello, World!"));
	return;
}
