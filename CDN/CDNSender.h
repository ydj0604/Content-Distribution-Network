#ifndef CDN_SENDER_H
#define CDN_SENDER_H

#include "cpprest/http_client.h"
#include "Shared.h"
#include <string>

class CDN_Node;

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::client;

class CDNSender {
public:
	CDNSender(string metaIpAddr, string fssIpAddr);
	void setCDN(CDN_Node* cdn) { m_cdn = cdn; }

	//for meta
	int sendCacheUpdateMsgToMeta(string fileName, int cdnId); //when cdn pulls a file from FSS and stores it into its cache
	int sendFileUpdateMsgToMeta(string fileName, string fileHash, int cdnId, string timeStamp); //when cdn updates an existing file
	int sendNewFileMsgToMeta(string fileName, string fileHash, int cdnId, string timeStamp); //when cdn receives a new file from client to store
	int sendCacheDeleteMsgToMeta(string fileName, int cdnId); //when cdn deletes a file from its cache
	int sendRegisterMsgToMeta(Address cdnAddr, int& assignedId); //when cdn comes live for the first time

	//for fss
	int getFileFromFSS(string fileName, int cdnId);
	int uploadFileToFSS(string fileName, const string& contents);

private:
	CDN_Node* m_cdn;
	string m_metaIpAddr;
	string m_fssIpAddr;
	http_client m_meta_client;
	http_client m_fss_client;
};

#endif
