#include "OriginServer.h"
#include "OriginClientReceiver.h"
#include "OriginClientSender.h"
#include "../Meta/MetaServer.h"
#include "../Shared.h"
#include "cpprest/http_listener.h"
#include <iostream>
using namespace std;

OriginServer::OriginServer(MetaServer* meta) {
	m_meta = meta;
}

OriginServer::~OriginServer() {
	//clean up
}

void OriginServer::setMeta(MetaServer* meta) {
	m_meta = meta;
}

void OriginServer::startListening() {
	utility::string_t port = U("5000");
	utility::string_t address = U("http://localhost:");
	address.append(port);
	OriginClientReceiver::initialize(address, this);
}

void OriginServer::endListening() {
	OriginClientReceiver::shutDown();
}

vector< pair<string, Address> > OriginServer::getListOfFilesDownload(const vector< pair<string, string> >& listFromClientApp, Address clientAddr) {
	vector< pair<string, Address> > result = m_meta->processListFromOriginDownload(listFromClientApp, clientAddr);
	return result;
}

vector< pair<string, Address> > OriginServer::getListOfFilesUpload(const vector< pair<string, string> >& listFromClientApp, Address clientAddr) {
	vector< pair<string, Address> > result = m_meta->processListFromOriginUpload(listFromClientApp, clientAddr);
	return result;
}

void fileList_parser (string json) {
	/*
	// extract filename, filehash from json object
	string file_name = json_to_fileName(json);
	string file_hash = json_to_hashName(json);
	
	ofstream out("a.txt");
	out << file_name + " ";
	
	out << file_hash + " ";
	*/
}
