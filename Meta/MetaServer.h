#ifndef META_SERVER_H
#define META_SERVER_H

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <string>
#include <vector>

class OriginServer;

using namespace std;

struct Address {
	pair<double, double> latLng;
	string ipAddr;
	Address() {
		latLng = make_pair(0.0, 0.0);
		ipAddr = "";
	}
	Address(pair<double, double> p, string s) {
		latLng = p;
		ipAddr = s;
	}
};

typedef struct Address Address;

class MetaServer {
public:
	//basic functions
	MetaServer(string file, OriginServer* origin);
	~MetaServer();
	void initiateMetaServer();
	void endMetaServer();
	void setFssAddr(Address fss);
	void addCdnAddr(Address cdn);
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
	void deleteMetaEntry(string fileName);
	void addNewMetaEntry(string fileName, const string& fileHash, const vector<Address>& CdnAddrList);
	void updateMetaEntry(string fileName, const string& fileHash, const vector<Address>& CdnAddrList);
	void addCdnToMetaEntry(string fileName, Address cdnAddr);

private:
	string m_file; //currently MetaServer writes everyghing to one file; we can improve the performance later (by indexing...etc)
	int m_version;
	vector<Address> m_cdnAddrList;
	Address m_FssAddr;
	OriginServer* m_origin;				// communicating point to and from OriginServer
};

#endif
