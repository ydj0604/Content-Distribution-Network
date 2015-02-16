#ifndef ORIGIN_SERVER_H
#define ORIGIN_SERVER_H

#include <vector>

class MetaServer;
class OriginClientReceiver;
class OriginClientSender;

using namespace std;

class OriginServer {
public:
	OriginServer(MetaServer* meta);
	~OriginServer();
	void startListeningForClientApp();
	void endListeningForClientApp();
	vector<pair<string, string>> getListOfFilesDownload(const vector<pair<string, string>>& listFromClientApp, string clientIpAddr);
	vector<pair<string, string>> getListOfFilesUpload(const vector<pair<string, string>>& listFromClientApp, string clientIpAddr);

private:
	MetaServer* m_meta;
	OriginClientReceiver* m_client_rcvr;
	OriginClientSender* m_client_sender;
};

#endif
