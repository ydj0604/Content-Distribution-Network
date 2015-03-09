#include "cpprest/http_client.h"
#include <iostream>
#include "CDN_Node.h"
#include "CDNSender.h"
#include "Shared.h"
#include <string>

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace json;
using namespace http::client;

CDNSender::CDNSender(string metaIpAddr, string fssIpAddr): m_meta_client(metaIpAddr), m_fss_client(fssIpAddr) {
	m_fssIpAddr = fssIpAddr;
	m_metaIpAddr = metaIpAddr;
}

/*
 * Sender functions for communication with Meta
 */

int CDNSender::sendCacheUpdateMsgToMeta(string fileName, int cdnId) {
	json::value jsonObj = json::value::object();
	jsonObj[U("Type")] = json::value::number(0);
	jsonObj[U("FileName")] = json::value::string(U(fileName));
	jsonObj[U("CdnId")] = json::value::number(cdnId);
	http_response resp = m_meta_client.request(methods::POST, U("/meta/update/"), jsonObj).get();
	if(resp.status_code() == status_codes::OK)
		return 0;
	else {
		cout<<"CDNSender:sendCacheUpdateMsgToMeta - failed to send"<<endl;
		return -1;
	}
}


//timestamp needed
int CDNSender::sendFileUpdateMsgToMeta(string fileName, string fileHash, int cdnId, string timeStamp) {
	json::value jsonObj = json::value::object();
	jsonObj[U("Type")] = json::value::number(1);
	jsonObj[U("FileName")] = json::value::string(U(fileName));
	jsonObj[U("FileHash")] = json::value::string(U(fileHash));
	jsonObj[U("CdnId")] = json::value::number(cdnId);
	jsonObj[U("TimeStamp")] = json::value::string(U(timeStamp));
	http_response resp = m_meta_client.request(methods::POST, U("/meta/update/"), jsonObj).get();

	if(resp.status_code() == status_codes::OK)
		return 0;
	else {
		cout<<"CDNSender:sendFileUpdateMsgToMeta - failed to send"<<endl;
		return -1;
	}
}

//timestamp needed
int CDNSender::sendNewFileMsgToMeta(string fileName, string fileHash, int cdnId, string timeStamp) {
	json::value jsonObj = json::value::object();
	jsonObj[U("Type")] = json::value::number(2);
	jsonObj[U("FileName")] = json::value::string(U(fileName));
	jsonObj[U("FileHash")] = json::value::string(U(fileHash));
	jsonObj[U("CdnId")] = json::value::number(cdnId);
	jsonObj[U("TimeStamp")] = json::value::string(U(timeStamp));
	http_response resp = m_meta_client.request(methods::POST, U("/meta/update/"), jsonObj).get();

	if(resp.status_code() == status_codes::OK)
		return 0;
	else {
		cout<<"CDNSender:sendNewFileMsgToMeta - failed to send"<<endl;
		return -1;
	}
}

int CDNSender::sendCacheDeleteMsgToMeta(string fileName, int cdnId) {
	json::value jsonObj = json::value::object();
	jsonObj[U("FileName")] = json::value::string(U(fileName));
	jsonObj[U("CdnId")] = json::value::number(cdnId);
	http_response resp = m_meta_client.request(methods::DEL, U("/meta/delete/"), jsonObj).get();
	if(resp.status_code() == status_codes::OK)
		return 0;
	else {
		cout<<"CDNSender:sendCacheDeleteMsgToMeta - failed to send"<<endl;
		return -1;
	}
}

int CDNSender::sendRegisterMsgToMeta(Address cdnAddr, int& assignedId) {
	json::value jsonObj = json::value::object();
	jsonObj[U("Type")] = json::value::number(0);
	jsonObj[U("IP")] = json::value::string(U(cdnAddr.ipAddr));
	jsonObj[U("Lat")] = json::value::number(cdnAddr.latLng.first);
	jsonObj[U("Lng")] = json::value::number(cdnAddr.latLng.second);

	http_response resp = m_meta_client.request(methods::POST, U("/meta/register/"), jsonObj).get();

	if(resp.status_code() == status_codes::OK) {
		json::value jsonObj = resp.extract_json().get();
		try {
			assignedId = jsonObj.at(U("CdnId")).as_integer();
		} catch(json::json_exception &e) {
			return -2; //meta returns an invalid response
		}
		return 0;
	} else {
		cout<<"CDNSender::sendRegisterMsgToMeta - registration failed"<<endl;
		return -1;
	}
}


/*
 * Sender functions for communication with Fss
 */

int CDNSender::getFileFromFSS(string fileName, int cdnId) {
	http_response resp = m_fss_client.request(methods::GET, U("/get"+fileName)).get();
	if(resp.status_code() != status_codes::OK) {
		cout<<"CDNSender:getFileFromFSS - failed to download file"<<endl;
		return -1;
	}
	string contents = resp.extract_string().get();
	vector<string> deletedFiles;
	if(!m_cdn->write_file(contents, fileName, deletedFiles))
		return -2;
	for(int i=0; i<deletedFiles.size(); i++) {
		sendCacheDeleteMsgToMeta(deletedFiles[i], cdnId);
	}
	sendCacheUpdateMsgToMeta(fileName, cdnId);
	return 0;
}

int CDNSender::uploadFileToFSS(string fileName, const string& contents) {
	http_response resp = m_fss_client.request(methods::POST, U("/post"+fileName), contents).get();
	if(resp.status_code() != status_codes::OK) {
		cout<<"CDNSender:uploadFileToFSS - failed to upload file"<<endl;
		return -1;
	}
	return 0;
}


