#include "OriginServer.h"
#include "OriginClientReceiver.h"
#include "../Meta/MetaServer.h"
#include "../Shared.h"
#include "cpprest/http_listener.h"
#include <iostream>
#include <unordered_map>
using namespace std;

OriginServer::OriginServer(string originIpAddrPort, MetaServer* meta) {
	m_meta = meta;
	m_originIpAddrPort = originIpAddrPort;
}

OriginServer::~OriginServer() {
	//clean up
}

void OriginServer::setMeta(MetaServer* meta) {
	m_meta = meta;
}

void OriginServer::startListening() {
	//utility::string_t port = U("5000");
	//utility::string_t address = U("http://localhost:");
	//address.append(port);
	OriginClientReceiver::initialize("http://"+m_originIpAddrPort, this);
}

void OriginServer::endListening() {
	OriginClientReceiver::shutDown();
}

//vector<<file name, file hash>> for input list
vector< pair<string, Address> > OriginServer::getListOfFilesDownload(const vector< pair<string, string> >& listFromClientApp, Address clientAddr, bool sharedOnly) {
	vector< pair<string, Address> > result = m_meta->processListFromOriginDownload(listFromClientApp, clientAddr, sharedOnly);
	return result;
}

vector< pair<string, Address> > OriginServer::getListOfFilesUpload(const vector< pair<string, string> >& listFromClientApp, Address clientAddr, bool sharedOnly) {
	vector< pair<string, Address> > result = m_meta->processListFromOriginUpload(listFromClientApp, clientAddr, sharedOnly);
	return result;
}


//vector<<file name, file time stamp>> for input list
int OriginServer::getListForSync(const vector< pair<string, string> >& clientFileList, vector<string>& uploadList, vector<string>& downloadList, unordered_map<string, string>& nameToTsMap) {
	int result = m_meta->processSyncWithTimeStamp(clientFileList, uploadList, downloadList, nameToTsMap);
	return result;
}
