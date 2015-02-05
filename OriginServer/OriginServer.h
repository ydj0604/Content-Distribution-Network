#ifndef ORIGIN_SERVER_H
#define ORIGIN_SERVER_H

#include <vector>
#include "OriginServerListener.h"
#include "OriginServerSender.h"

using namespace std;



class OriginServer {
public:
	OriginServer() {
		//initialize listener and sender objects
		//initialize other necessary private variables
	}

	~OriginServer() {
		//clean up
	}

	void startListeningForClientApp() {
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

	void endListeningForClientApp() {
		//it terminates the two rest services
	}

	vector<string> getListOfFilesToUploadOrDownload(const vector<pair<string, string>>& listFromClientApp) {
		//each pair contains (file name, file hash) assuming file hash can be stored as type string

		/*
		 1. send the vector of pairs to Meta Server
		 2. receive the resultant list of file names from Meta Server
		 3. return the list received from Meta Server
		 */
	}

	string getClosestCDNAddr(string fileId, string clientAddr) {
		//used for upload request
		//needs to ask Meta Server to get the closest CDN Node from client
	}

	string getSelectedCDNAddr(string fileId, string clientAddr) {
		//used for download request
		//needs to ask Meta Server, which will then use Selection Algorithm to choose which CDN Node to use
	}

	void makeDownloadRequestToCDN(string CDNAddr, string clientAddr, string fileId) {
		//make a request to CDN Node so that CDN Node can send the file to client app
	}

	void makeUploadRequestToCDN(string CDNAddr, string clientAddr, string fileId) {
		//make a request to CDN Node so that CDN Node can request a file to be uploaded from client app
	}


private:

	OriginServerListener* m_listener; //http library wrapper object for receiving requests
	OriginServerSender* m_sender; //http library wrapper object for sending requests
};

#endif
