#ifndef META_SERVER_H
#define META_SERVER_H

/*
#ifdef _WIN32
	#include <unordered_map>
	#include <unordered_set>
#else
	#include <tr1/unordered_map>
	#include <tr1/unordered_set>
#endif
*/

#include <unordered_map>
#include <unordered_set>

#include <string>
#include <vector>

class OriginServer;
class MetaCDNReceiver;
class MetaCDNSender;

using namespace std;
//using namespace std::tr1;

class MetaServer {
public:
	//basic functions
	MetaServer(string file, OriginServer* origin);
	~MetaServer();
	void initiateMetaServer();
	void endMetaServer();
	void setFssIpAddr(string fss);
	void addCdnIpAddr(string cdn);
	void addLatLngWithIpAddr(string ipAddr, double lat, double lng);

	//helper functions
	void sortFileList(vector< pair<string, string> >& fileListFromOrigin);
	double calculateDistance(string ipAddr1, string ipAddr2);
	pair<double, double> getLatLng(string ipAddr);

	//functions for communication with Origin
	string getClosestCDN(const vector<string>& cdnAddrList, string clientIpAddr);
	vector<string> getCdnsThatContainFile(string fileName);
	bool isCDN_closerThanFSS(string cdnIpAddr, string clientIpAddr);
	bool CDN_load_OK(string CDN_IpAddr);
	vector< pair<string, string> > processListFromOriginDownload(const vector< pair<string, string> >& clientFileList, string clientIpAddr); //returns vector of (name, CDN addr)
	vector< pair<string, string> > processListFromOriginUpload(const vector< pair<string, string> >& clientFileList, string clientIpAddr);

	//functions for communication with CDN
	void deleteMetaEntry(string fileName);
	void addNewMetaEntry(string fileName, const string& fileHash, const vector<string>& CdnAddrList);
	void updateMetaEntry(string fileName, const string& fileHash, const vector<string>& CdnAddrList);
	void addCdnToMetaEntry(string fileName, string cdnAddr);

private:
	string m_file; //currently MetaServer writes everyghing to one file; we can improve the performance later (by indexing...etc)
	int m_version;
	unordered_set<string> m_setOfCdnIpAddr;
	string m_FssIpAddr;
	unordered_map< string, pair<double, double> > m_ipAddrToLatLngMap; //maps an IP address to geographic location

	OriginServer* m_origin;				// communicating point to and from OriginServer
	MetaCDNReceiver* m_CDN_rcvr;		// receives HTTP request from CDN
	MetaCDNSender* m_CDN_sender;		// sends HTTP request to CDN
};

#endif
