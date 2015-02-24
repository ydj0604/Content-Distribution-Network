#ifndef CDN_META_RECEIVER_H
#define CDN_META_RECEIVER_H

#include "cpprest/http_listener.h"
#include <string>

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

struct Address {
	pair<double, double> latLng; //always keep 2 decimal digits for latitude and longitude
	string ipAddr;
	Address() {
		latLng = make_pair(0.0, 0.0);
		ipAddr = "";
	}
	Address(pair<double, double> p, string s) {
		latLng = p;
		ipAddr = s;
	}
};

typedef struct Address Address;

class CDNMetaSender {
public:
	CDNMetaReceiver(string metaIpAddr);
	int sendCacheUpdateMsg(string fileName, Address cdnAddr); //when cdn pulls a file from FSS and stores it into its cache
	int sendFileUpdateMsg(string fileName, string fileHash, Address cdnAddr); //when cdn updates an existing file
	int sendNewFileMsg(string fileName, string fileHash, Address cdnAddr); //when cdn receives a new file from client to store
	int sendCacheDeleteMsg(string fileName, Address cdnAddr); //when cdn deletes a file from its cache
	int sendRegisterMsg(Address cdnAddr); //when cdn comes live for the first time
private:
	string m_metaIpAddr;
	http_client m_client;
};

#endif
