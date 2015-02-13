#ifndef META_SERVER_H
#define META_SERVER_H

#include <string>
#include <vector>

class CDNReceiver;
class CDNSender;

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

private:
	OriginServer* m_origin;
	CDNReceiver* m_CDN_rcvr;					// receives HTTP request from CDN
	CDNSender* m_CDN_sender;					// sends HTTP request to CDN
};

#endif
