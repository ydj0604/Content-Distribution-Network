#include "OriginServer.h"
#include "../Meta/MetaServer.h"
#include <iostream>
using namespace std;

OriginServer::OriginServer(MetaServer* meta) {
	//set meta server
	//initialize listener and sender objects
	//initialize other necessary private variables
}

OriginServer::~OriginServer() { //clean up

}


void OriginServer::startListeningForClientApp() {
	/*
	 there are two rest services to be deployed
	 	 1. initialContact
	 	 	 -this lets client app to pass list of file names and file hashes to determine
	 	 	  which files have to be actually downloaded/uploaded
	 	 2. actualFileRequest
	 	 	 -this lets client app to pass one file name to process download/upload request
	*/
	//the two rest services start listening
}

void OriginServer::endListeningForClientApp() {
	//it terminates the two rest services
}

//returns a vector of (file name, CDN address)
vector<pair<string, string>> OriginServer::getListOfFilesDownload(const vector<pair<string, string>>& listFromClientApp, string clientIpAddr) {
	vector<pair<string, string>> result = m_meta->processListFromOriginDownload(listFromClientApp, clientIpAddr);
	return result;
}

vector<pair<string, string>> OriginServer::getListOfFilesUpload(const vector<pair<string, string>>& listFromClientApp, string clientIpAddr) {
	vector<pair<string, string>> result = m_meta->processListFromOriginUpload(listFromClientApp, clientIpAddr);
	return result;
}
