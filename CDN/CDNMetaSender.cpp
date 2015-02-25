#include "cpprest/http_client.h"
#include "CDNMetaSender.h"
#include <iostream>
#include "CDN_Node.h"

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace json;
using namespace http::client;

/*
	void setMetaIpAddr(string metaIpAddr) { m_metaIpAddr = metaIpAddr; }
	int sendCacheUpdateMsg(string fileName, Address cdnAddr); //when cdn pulls a file from FSS and stores it into its cache
	int sendFileUpdateMsg(string fileName, string fileHash, Address cdnAddr); //when cdn updates an existing file
	int sendNewFileMsg(string fileName, string fileHash, Address cdnAddr); //when cdn receives a new file from client to store
	int sendCacheDeleteMsg(string fileName, Address cdnAddr); //when cdn deletes a file from its cache
	int sendRegisterMsg(Address cdnAddr); //when cdn comes live for the first time
 */


CDNMetaSender::CDNMetaSender(string metaIpAddr): m_client(U(metaIpAddr)) {
	m_metaIpAddr = metaIpAddr;
}

int CDNMetaSender::sendCacheUpdateMsg(string fileName, Address cdnAddr) {
	json::value jsonObj = json::value::object();
	jsonObj[U("Type")] = json::value::number(0);
	jsonObj[U("FileName")] = json::value::string(U(fileName));
	jsonObj[U("IP")] = json::value::string(U(cdnAddr.ipAddr));
	jsonObj[U("Lat")] = json::value::number(cdnAddr.latLng.first);
	jsonObj[U("Lng")] = json::value::number(cdnAddr.latLng.second);
	http_response resp = m_client.request(methods::POST, U("/meta/update/"), jsonObj).get();
	if(resp.status_code() == status_codes::OK)
		return 0;
	else
		return -1;
}

int CDNMetaSender::sendFileUpdateMsg(string fileName, string fileHash, Address cdnAddr) {
	json::value jsonObj = json::value::object();
	jsonObj[U("Type")] = json::value::number(1);
	jsonObj[U("FileName")] = json::value::string(U(fileName));
	jsonObj[U("FileHash")] = json::value::string(U(fileHash));
	jsonObj[U("IP")] = json::value::string(U(cdnAddr.ipAddr));
	jsonObj[U("Lat")] = json::value::number(cdnAddr.latLng.first);
	jsonObj[U("Lng")] = json::value::number(cdnAddr.latLng.second);
	http_response resp = m_client.request(methods::POST, U("/meta/update/"), jsonObj).get();
	if(resp.status_code() == status_codes::OK)
		return 0;
	else
		return -1;
}

int CDNMetaSender::sendNewFileMsg(string fileName, string fileHash, Address cdnAddr) {
	json::value jsonObj = json::value::object();
	jsonObj[U("Type")] = json::value::number(2);
	jsonObj[U("FileName")] = json::value::string(U(fileName));
	jsonObj[U("FileHash")] = json::value::string(U(fileHash));
	jsonObj[U("IP")] = json::value::string(U(cdnAddr.ipAddr));
	jsonObj[U("Lat")] = json::value::number(cdnAddr.latLng.first);
	jsonObj[U("Lng")] = json::value::number(cdnAddr.latLng.second);
	http_response resp = m_client.request(methods::POST, U("/meta/update/"), jsonObj).get();
	if(resp.status_code() == status_codes::OK)
		return 0;
	else
		return -1;
}

int CDNMetaSender::sendCacheDeleteMsg(string fileName, Address cdnAddr) {
	json::value jsonObj = json::value::object();
	jsonObj[U("FileName")] = json::value::string(U(fileName));
	jsonObj[U("IP")] = json::value::string(U(cdnAddr.ipAddr));
	jsonObj[U("Lat")] = json::value::number(cdnAddr.latLng.first);
	jsonObj[U("Lng")] = json::value::number(cdnAddr.latLng.second);
	http_response resp = m_client.request(methods::DEL, U("/meta/delete/"), jsonObj).get();
	if(resp.status_code() == status_codes::OK)
		return 0;
	else
		return -1;
}

int CDNMetaSender::sendRegisterMsg(Address cdnAddr) {
	json::value jsonObj = json::value::object();
	jsonObj[U("Type")] = json::value::number(0);
	jsonObj[U("IP")] = json::value::string(U(cdnAddr.ipAddr));
	jsonObj[U("Lat")] = json::value::number(cdnAddr.latLng.first);
	jsonObj[U("Lng")] = json::value::number(cdnAddr.latLng.second);
	http_response resp = m_client.request(methods::POST, U("/meta/register/"), jsonObj).get();
	if(resp.status_code() == status_codes::OK)
		return 0;
	else
		return -1;
}
