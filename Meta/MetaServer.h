#ifndef META_SERVER_H
#define META_SERVER_H

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
#include <vector>
#include "../Shared.h"

class OriginServer;

using namespace std;

typedef struct Address Address;

class MetaServer {
public:
	//basic functions
	MetaServer(string file, OriginServer* origin);
	~MetaServer();
	void startListening();
	void endListening();
	int setFssAddr(Address fss);
	int addCdnAddr(Address cdn);
	vector<Address> getCdnList() { return m_cdnAddrList; }

	//helper functions
	void sortFileList(vector< pair<string, string> >& fileListFromOrigin);
	static Address parseAddress(const string& line);
	double calculateDistance(Address addr1, Address addr2);

	//functions for communication with Origin
	Address getClosestCDN(const vector<Address>& cdnAddrList, Address clientAddr);
	vector<Address> getCdnsThatContainFile(string fileName);
	bool isCDN_closerThanFSS(Address cdnAddr, Address clientAddr);
	bool CDN_load_OK(Address cdnAddr);
	vector< pair<string, Address> > processListFromOriginDownload(const vector< pair<string, string> >& clientFileList, Address clientAddr);
	vector< pair<string, Address> > processListFromOriginUpload(const vector< pair<string, string> >& clientFileList, Address clientAddr);

	//functions for communication with CDN
	int deleteMetaEntry(string fileName);
	int addNewMetaEntry(string fileName, const string& fileHash, const vector<Address>& CdnAddrList);
	int updateMetaEntry(string fileName, const string& fileHash, const vector<Address>& CdnAddrList);
	int addCdnToMetaEntry(string fileName, Address cdnAddr);
	int deleteCdnFromMetaEntry(string fileName, Address cdnAddr);

private:
	void updateVersion();
	string m_file; //currently MetaServer writes everyghing to one file; we can improve the performance later (by indexing...etc)
	int m_version;
	int m_nextCDNId;
	vector<Address> m_cdnAddrList;
	Address m_FssAddr;
	OriginServer* m_origin;				// communicating point to and from OriginServer
};

#endif
