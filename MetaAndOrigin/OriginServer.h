#ifndef ORIGIN_SERVER_H
#define ORIGIN_SERVER_H

#include <vector>

class MetaServer;
class OriginClientReceiver;
class OriginClientSender;

using namespace std;

class OriginServer {
public:
	OriginServer();
	~OriginServer();
	void startListeningForClientApp();
	void endListeningForClientApp();
	vector<pair<string, string>> getListOfFilesAndAddrToUploadOrDownload(const vector<pair<string, string>>& listFromClientApp);
	string getClosestCDNAddr(string fileId, string clientAddr);
	string getSelectedCDNAddr(string fileId, string clientAddr);
	void makeDownloadRequestToCDN(string CDNAddr, string clientAddr, string fileId);
	void makeUploadRequestToCDN(string CDNAddr, string clientAddr, string fileId);

private:
	MetaServer* m_meta;
	OriginClientReceiver* m_client_rcvr;
	OriginClientSender* m_client_sender;
};

#endif
