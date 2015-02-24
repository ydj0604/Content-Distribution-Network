#include "cpprest/http_listener.h"
#include "CDNMetaSender.h"
#include <iostream>
#include "CDN_Node.h"

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;
using namespace json;

/*
	void setMetaIpAddr(string metaIpAddr) { m_metaIpAddr = metaIpAddr; }
	int sendCacheUpdateMsg(string fileName, Address cdnAddr); //when cdn pulls a file from FSS and stores it into its cache
	int sendFileUpdateMsg(string fileName, string fileHash, Address cdnAddr); //when cdn updates an existing file
	int sendNewFileMsg(string fileName, string fileHash, Address cdnAddr); //when cdn receives a new file from client to store
	int sendCacheDeleteMsg(string fileName, Address cdnAddr); //when cdn deletes a file from its cache
	int sendRegisterMsg(Address cdnAddr); //when cdn comes live for the first time
 */


CDNMetaSender::CDNMetaSender(string metaIpAddr): m_listener(U("m_metaIpAddr")) {
	m_metaIpAddr = metaIpAddr;
}

int CDNMetaSender::sendCacheUpdateMsg(string fileName, Address cdnAddr) {

}

int CDNMetaSender::sendFileUpdateMsg(string fileName, string fileHash, Address cdnAddr) {

}

int CDNMetaSender::sendNewFileMsg(string fileName, string fileHash, Address cdnAddr) {

}

int CDNMetaSender::sendCacheDeleteMsg(string fileName, Address cdnAddr) {

}

int CDNMetaSender::sendRegisterMsg(Address cdnAddr) {

}
