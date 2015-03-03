#ifndef ORIGIN_SERVER_H
#define ORIGIN_SERVER_H

#include <vector>
#include <string>
#include "../Shared.h"

class MetaServer;
class OriginClientReceiver;
class OriginClientSender;

using namespace std;

class OriginServer {
public:
	OriginServer(string originIpAddrPort, MetaServer* meta=NULL);
	~OriginServer();
	void setMeta(MetaServer* meta);
	void startListening();
	void endListening();
	vector< pair<string, Address> > getListOfFilesDownload(const vector< pair<string, string> >& listFromClientApp, Address clientAddr);
	vector< pair<string, Address> > getListOfFilesUpload(const vector< pair<string, string> >& listFromClientApp, Address clientAddr);
	int getListForSync(const vector< pair<string, long long> >& clientFileList, vector<string>& uploadList, vector<string>& downloadList, vector<string>& deleteList);

private:
	MetaServer* m_meta;
	string m_originIpAddrPort;
};

#endif
