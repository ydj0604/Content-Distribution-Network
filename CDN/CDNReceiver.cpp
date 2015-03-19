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
#include "CDNSender.h"
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
	if(cdn==NULL) {
		cout<<"CDNReceiver::initialize - input CDN is NULL"<<endl;
		return;
	}
	CDNReceiver* instance = CDNReceiver::getInstance();
	uri_builder uri(address);
	uri.append_path(U("cdn/cache"));
	instance = new CDNReceiver(uri.to_uri().to_string());
	instance->setCDN(cdn);
	instance->open().wait();
	ucout << utility::string_t(U("CDNReceiver is listening for requests at: ")) << uri.to_uri().to_string() << std::endl;
}

void CDNReceiver::shutDown() {
	CDNReceiver *instance = CDNReceiver::getInstance();
	if(instance == NULL)
		return;
	instance->close().wait();
	delete instance;
}

void CDNReceiver::handle_delete(http_request message) {
	/*
	 * http://localhost:4000/cdn/cache/filename <DEL>
	 */

	string fileName = message.relative_uri().to_string();

	cout << endl << "---------------"<< endl;
	cout << fileName << endl;
	cout << message.to_string() << endl <<endl;

	if(m_cdn->delete_file(fileName)) {  //look_up_and_remove_storage(fileName ,1)) //remove the file
		CDNSender* sender = m_cdn->getSender();
		sender->sendCacheDeleteMsgToMeta(fileName, m_cdn->get_cdn_id());
		message.reply(status_codes::OK, U("delete succeeded"));
	} else {
		message.reply(status_codes::OK, U(fileName + " is not found in cdn"));
	}
}

void CDNReceiver::handle_get(http_request message) {
	/*
	 * http://localhost:4000/cdn/cache/filename <GET>
	 */

	string fileName = message.relative_uri().to_string();
	CDNSender* sender = m_cdn->getSender();

	cout << endl << "---------------"<< endl;
	cout << fileName << endl;
	cout << message.to_string() << endl <<endl;

	if(!m_cdn->look_up_and_remove_storage(fileName, 0)) { //cdn doesn't have the file in its cache
		cout<<"CDN Cache MISS"<<endl;
		if(sender->getFileFromFSS(fileName, m_cdn->get_cdn_id()) != 0) {
			cout<<"CDNReceiver::handle_get - getFileFromFSS failed for "+fileName<<endl;
			message.reply(status_codes::NotFound, fileName+" does not exist in fss");
			return;
		}
	}

	if(!m_cdn->look_up_and_remove_storage(fileName, 0)) {
		cout<<"CDNReceiver::handle_get - "+fileName+" still doesn't exist"<<endl;
		message.reply(status_codes::NotFound, fileName+" does not exist in cdn");
		return;
	} else {
		message.reply(status_codes::OK, m_cdn->load_file(fileName));
	}

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


//it has to take TIME STAMP for real-time sync
void CDNReceiver::handle_put(http_request message) {
	/*
	 * http://localhost:4000/cdn/cache/filename?filehash&timestamp <PUT>
	 * {
	 * 		Body: contents
	 * }
	 */

	string fileName = message.relative_uri().path();
	string queryStr = message.relative_uri().query();
	string contents = message.extract_string().get(), fileHash="", timeStamp="";

	//retrieve file hash and time stamp from query parameter
	bool firstPart = true;
	for(int i=0; i<queryStr.size(); i++) {
		if(queryStr[i]=='&') {
			firstPart = false;
			continue;
		} else if(firstPart) {
			fileHash += queryStr[i];
		} else {
			timeStamp += queryStr[i];
		}
	}

	cout << endl << "---------------"<< endl;
	cout << fileName << endl;
	cout << message.to_string() << endl <<endl;

	CDNSender* sender = m_cdn->getSender();
	vector<string> deletedFiles;
	if(!m_cdn->write_file(contents, fileName, deletedFiles)) {
		message.reply(status_codes::NotFound, "failed to write the file to cdn");
		return;
	}

	for(int i=0; i<deletedFiles.size(); i++) { //send msgs to meta to notify that this cdn no longer contains deletedFiles
		sender->sendCacheDeleteMsgToMeta(deletedFiles[i], m_cdn->get_cdn_id());
	}

	if(sender->uploadFileToFSS(fileName, contents) != 0) { //need to roll back
		m_cdn->delete_file(fileName);
		message.reply(status_codes::NotFound, "failed to write the file to fss");
		return;
	}

	//update meta server when everything is done properly
	sender->sendFileUpdateMsgToMeta(fileName, fileHash, m_cdn->get_cdn_id(), timeStamp);
	//sender->sendCacheUpdateMsgToMeta(fileName, m_cdn->get_cdn_id());
	message.reply(status_codes::OK, fileName + ": " + fileHash + ", " + timeStamp); //testing purpose
}









