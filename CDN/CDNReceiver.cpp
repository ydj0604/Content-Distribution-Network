#include "cpprest/http_listener.h"
#include "cpprest/rawptrstream.h"
#include "cpprest/streams.h"
#include "cpprest/filestream.h"
#include "cpprest/containerstream.h"
#include "cpprest/producerconsumerstream.h"
#include "cpprest/asyncrt_utils.h"
#include "Shared.h"
#include "CDN_Node.h"
#include "CDNReceiver.h"
#include <iostream>
#include <string>

using namespace std;
using namespace web;
using namespace utility;
using namespace concurrency;
using namespace web::http;
using namespace web::http::experimental::listener;
using namespace json;

CDNReceiver* CDNReceiver::m_instance = NULL;

CDNReceiver::CDNReceiver(utility::string_t url) : m_listener(url) {
	m_listener.support(methods::DEL, std::bind(&CDNReceiver::handle_delete, this, std::placeholders::_1));
	m_listener.support(methods::GET, std::bind(&CDNReceiver::handle_get, this, std::placeholders::_1));
	m_listener.support(methods::PUT, std::bind(&CDNReceiver::handle_put, this, std::placeholders::_1));
}

void CDNReceiver::initialize(const string_t& address, CDN_Node* cdn) {
	CDNReceiver* instance = CDNMetaReceiver::getInstance();
	uri_builder uri(address);
	uri.append_path(U("cdn/cache"));
	instance = new CDNReceiver(uri.to_uri().to_string());
	instance->setCDN(cdn);
	instance->open().wait();
	ucout << utility::string_t(U("CDNReceiver is listening for requests at: ")) << uri.to_uri().to_string() << std::endl;
}

void CDNReceiver::shutDown() {
	CDNMetaReceiver *instance = CDNMetaReceiver::getInstance();
	if(instance == NULL)
		return;
	instance->close().wait();
	delete instance;
}

void CDNReceiver::handle_delete(http_request message) {
	string filePath = CDN_DIR + message.relative_uri().to_string();
	if(m_cdn==NULL) {
		message.reply(status_codes::NotFound, U("CDN server is not set"));
		return;
	}
	if(m_cdn->look_up_and_remove_storage(filePath ,1)) //remove the file
		message.reply(status_codes::OK, U("delete succeeded"));
	else
		message.reply(status_codes::NotFound, U(filePath + " is not found"));
}

void CDNReceiver::handle_get(http_request message) {
	string filePath = CDN_DIR + message.relative_uri().to_string();

	if(!m_cdn->look_up_and_remove_storage(filePath ,0)) { //cdn doesn't have the file in its cache
		if(!m_cdn->get_file_from_FSS(filePath)) { //fss doesn't have the file either
			message.reply(status_codes::NotFound, U(filePath + " is not found"));
			return;
		}
	}

	//TODO: return file content in message body

	/*
	//set up file stream response
	http_response response(status_codes::OK);
	auto stream = streams::file_stream<uint8_t>::open_istream(U(filePath)).get();
	response.set_body(stream);
	auto length = stream.seek(0, std::ios_base::end);
	stream.seek(0);
	response.headers().set_content_type(U("text/plain; charset=utf-8"));
	response.headers().set_content_length((size_t)length);
    message.reply(response).wait();
    */
}

void CDNReceiver::handle_put(http_request message) {
	string filePath = CDN_DIR + message.relative_uri().to_string();
	/*
	 * 1. extract message body (file contents) into string
	 * 2. write the contents into filPath
	 * 3. send the file to FSS
	 * 4. send the msg to Meta to broadcast invalidate msgs
	 */
}









