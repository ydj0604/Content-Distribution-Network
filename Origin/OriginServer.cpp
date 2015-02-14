#include "OriginServer.h"
#include <iostream>
using namespace std;

OriginServer::OriginServer() {
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


vector<pair<string, string>> OriginServer::getListOfFilesAndAddrToUploadOrDownload(const vector<pair<string, string>>& listFromClientApp) {
	vector<string> result = m_meta->processListFromOrigin(listFromClientApp);
	return result;
}

string OriginServer::getClosestCDNAddr(string fileId, string clientAddr) {
	//used for upload request
	//needs to ask Meta Server to get the closest CDN Node from client
}

string OriginServer::getSelectedCDNAddr(string fileId, string clientAddr) {
	//used for download request
	//needs to ask Meta Server, which will then use Selection Algorithm to choose which CDN Node to use
}

void OriginServer::makeDownloadRequestToCDN(string CDNAddr, string clientAddr, string fileId) {
	//make a request to CDN Node so that CDN Node can send the file to client app
}

void OriginServer::makeUploadRequestToCDN(string CDNAddr, string clientAddr, string fileId) {
	//make a request to CDN Node so that CDN Node can request a file to be uploaded from client app
}
