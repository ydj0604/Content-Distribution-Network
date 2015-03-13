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
	MetaServer(string metaIpAddrPort, string file, OriginServer* origin);
	~MetaServer();
	void startListening();
	void endListening();
	int setFssAddr(Address fss);
	int registerCdn(Address cdn);
	int unregisterCdn(int cdnId);
	vector<Address> getCdnAddrs();
	const unordered_map<int, Address>& getCdnIdToAddrMap() { return m_cdnIdToAddrMap; }

	//helper functions
	double calculateDistance(Address addr1, Address addr2);

	//functions for communication with Origin
	int getClosestCDN(const vector<int>& cdnIdList, Address clientAddr);
	vector<int> getCdnsThatContainFile(string fileName);
	bool isCDN_closerThanFSS(int cdnId, Address clientAddr);
	bool CDN_load_OK(int cdnId);

	vector< pair<string, Address> > processListFromOriginDownload(const vector< pair<string, string> >& clientFileList, Address clientAddr, bool sharedOnly=false);
	vector< pair<string, Address> > processListFromOriginUpload(const vector< pair<string, string> >& clientFileList, Address clientAddr, bool sharedOnly=false);

	//functions for communication with CDN
	bool doesExist(string fileName);
	int deleteMetaEntry(string fileName);
	int addNewMetaEntry(string fileName, const string& fileHash, const vector<int>& cdnIdList);
	int updateMetaEntry(string fileName, const string& fileHash, const vector<int>& cdnIdList);
	int addCdnToMetaEntry(string fileName, int cdnId);
	int deleteCdnFromMetaEntry(string fileName, int cdnId);

	//functions for timestamp management
	int processSyncWithTimeStamp(const vector< pair<string, string> >& clientFileList,
								 vector<string>& uploadList, vector<string>& downloadList, unordered_map<string, string>& nameToTsMap);
	int updateTimeStamp(string fileName, string timeStamp);
	int addNewTimeStamp(string fileName, string timeStamp);
	int deleteTimeStamp(string fileName);




private:
	void updateVersion();
	void updateVersion_timestamp();
	string m_metaIpAddrPort; //ex: 1.1.1.1:3000
	string m_file; //currently MetaServer writes everyghing to one file; we can improve the performance later (by indexing...etc)
	string m_file_timestamp;
	int m_version;
	int m_version_timestamp;
	int m_nextCDNId;
	Address m_FssAddr;
	OriginServer* m_origin;
	unordered_map<int, Address> m_cdnIdToAddrMap; //key=id val=ipaddr:port
};

#endif
