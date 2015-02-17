#include "OriginServer.h"
#include "OriginClientReceiver.h"
#include "OriginClientSender.h"
#include "../Meta/MetaServer.h"
#include <iostream>
using namespace std;

OriginServer::OriginServer(MetaServer* meta) {
	m_meta = meta;
	m_client_rcvr = new OriginClientReceiver();
	m_client_sender = new OriginClientSender();
}

OriginServer::~OriginServer() { //clean up
	delete m_client_rcvr;
	delete m_client_sender;
}

void OriginServer::setMeta(MetaServer* meta) {
	m_meta = meta;
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
vector< pair<string, string> > OriginServer::getListOfFilesDownload(const vector< pair<string, string> >& listFromClientApp, string clientIpAddr) {
	vector< pair<string, string> > result = m_meta->processListFromOriginDownload(listFromClientApp, clientIpAddr);
	return result;
}

vector< pair<string, string> > OriginServer::getListOfFilesUpload(const vector< pair<string, string> >& listFromClientApp, string clientIpAddr) {
	vector< pair<string, string> > result = m_meta->processListFromOriginUpload(listFromClientApp, clientIpAddr);
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
