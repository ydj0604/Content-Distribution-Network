#include "cpprest/http_listener.h"
#include "cpprest/rawptrstream.h"
#include "cpprest/streams.h"
#include "cpprest/filestream.h"
#include "CDNClientReceiver.h"
#include <iostream>
#include "CDN_Node.h"

using namespace std;
using namespace web;
using namespace utility;
using namespace concurrency;
using namespace web::http;
using namespace web::http::experimental::listener;

CDNClientReceiver* CDNClientReceiver::m_instance = NULL;

CDNClientReceiver::CDNClientReceiver(utility::string_t url) : m_listener(url) {
    m_listener.support([&](http_request request)
    {
    	http_response response(status_codes::OK);
    	auto stream = streams::file_stream<uint8_t>::open_istream(U("testfile")).get();
    	cout<<"HELLO"<<endl;
    	response.set_body(stream);
    	auto length = stream.seek(0, std::ios_base::end);
    	stream.seek(0);
    	response.headers().set_content_type(U("text/plain; charset=utf-8"));
   	    response.headers().set_content_length((size_t)length);

        request.reply(response).wait();
    });
}

void CDNClientReceiver::initialize(const string_t& address, CDN_Node* cdn) {
	CDNClientReceiver* instance = CDNClientReceiver::getInstance();
	uri_builder uri(address);
	uri.append_path(U("cdn/cache"));
	instance = new CDNClientReceiver(uri.to_uri().to_string());
	instance->setCDN(cdn);
	instance->open().wait();
	ucout << utility::string_t(U("CDNClientReceiver is listening for requests at: ")) << uri.to_uri().to_string() << std::endl;
}

void CDNClientReceiver::shutDown() {
	CDNClientReceiver *instance = CDNClientReceiver::getInstance();
	if(instance == NULL)
		return;
	instance->close().wait();
	delete instance;
}

void CDNClientReceiver::handle_get(http_request message) {
	auto paths = uri::split_path(uri::decode(message.relative_uri().path()));
	string filePath = "";
	for(int i=0; i<paths.size(); i++) {
		if(i==paths.size()-1)
			filePath += paths[i];
		else
			filePath += paths[i]+"/";
	}
	message.reply(status_codes::OK, U("Hello World"));
}
