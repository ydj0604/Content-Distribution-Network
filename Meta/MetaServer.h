#ifndef META_SERVER_H
#define META_SERVER_H

#include <string>
#include <vector>

class MetaCDNReceiver;
class MetaCDNSender;

using namespace std;

class MetaServer {
public:
	MetaServer();
	~MetaServer();
	void initiateMetaServer();
	void endMetaServer();
	void fileList_parser( string json );
	vector< pair<string, string> > runSelectionAlgorithm( const vector< pair<string, string> >& fileListFromOrigin, int type );
	void sortFileList( vector< pair<string, string> >& fileListFromOrigin );
	string getClosestCDN( const vector<string> cdn_list );
	bool isCDN_closer( const string CDN_addr );
	bool CDN_load_OK( const string CDN_addr );

	//to communicate with origin
	vector<string> processListFromOrigin(const vector<pair<string, string>>& listFromClientApp);

private:
	OriginServer* m_origin;
	MetaCDNReceiver* m_CDN_rcvr;					// receives HTTP request from CDN
	MetaCDNSender* m_CDN_sender;					// sends HTTP request to CDN
};

#endif
