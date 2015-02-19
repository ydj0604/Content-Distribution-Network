
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

#include "MetaServer.h"
#include "MetaCDNReceiver.h"
#include "MetaCDNSender.h"
#include "../Origin/OriginServer.h"
#include <string>
#include <vector>
#include <math.h>
#include <sys/stat.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ios>

using namespace std;
//using namespace std::tr1;

MetaServer::MetaServer(string file, OriginServer* origin) {
	for(int i=0; i<file.size(); i++)
		if(file[i]==' ') {
			cout<<"MetaServer::MetaServer - file name can't contain a space"<<endl;
			exit(0);
		}
	string dirName = "./MetaData";
	mkdir(dirName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	m_file = "./MetaData/"+file;
	m_version = 0;

	m_origin = origin;
	m_CDN_rcvr = new MetaCDNReceiver();
	m_CDN_sender = new MetaCDNSender();
}

MetaServer::~MetaServer() {
	delete m_CDN_rcvr;
	delete m_CDN_sender;
}

void MetaServer::initiateMetaServer() {
		/*
			1. For each private object in MetaServer, open the connection to both Origin Server and FSS
			  - FSS will only send its file list when an update/creation of file(s) from the Client App is done

			2. Synchronize file list originally stored in FSS (should be served in a separate function?)

			origin_rcvr {
				recvfrom() {

					// syncFSS()
				}

			}

		*/
}

void MetaServer::endMetaServer() {
	// terminates the Meta Server's connection with others
}

void MetaServer::setFssIpAddr(string fss) {
	m_FssIpAddr = fss;
}
void MetaServer::addCdnIpAddr(string cdn) {
	m_setOfCdnIpAddr.insert(cdn);
}
void MetaServer::addLatLngWithIpAddr(string ipAddr, double lat, double lng) { //overwrites the existing map entry
	m_ipAddrToLatLngMap[ipAddr] = make_pair(lat, lng);
}


/*------------------*/
/* helper functions */
/*------------------*/

bool compare(const pair<string, string>& a, const pair<string, string>& b) {
		return a.first < b.first;
}

void MetaServer::sortFileList(vector< pair<string, string> >& fileListFromOrigin) {
		sort(fileListFromOrigin.begin(), fileListFromOrigin.end(), compare);
}

double rad2deg(double rad) {
	return (rad * 180 / 3.14159265358979323846);
}

double deg2rad(double deg) {
	return (deg * 3.14159265358979323846 / 180);
}

pair<double, double> MetaServer::getLatLng(string ipAddr) {
	if(m_ipAddrToLatLngMap.count(ipAddr)==0) {
		cout<<"MetaServer::getLatLng - ip address does not exist"<<endl;
		exit(0);
	}
	return m_ipAddrToLatLngMap[ipAddr];
}

double MetaServer::calculateDistance(string ipAddr1, string ipAddr2) { //in miles
	pair<double, double> loc1 = getLatLng(ipAddr1), loc2 = getLatLng(ipAddr2);
	double theta, dist;
	theta = loc1.second - loc2.second;
	dist = sin(deg2rad(loc1.first)) * sin(deg2rad(loc2.first)) + cos(deg2rad(loc1.first)) * cos(deg2rad(loc2.first)) * cos(deg2rad(theta));
	dist = acos(dist);
	dist = rad2deg(dist);
	dist = dist * 60 * 1.1515;
	return dist;
}


/*-------------------------------*/
/* for communication with Origin */
/*-------------------------------*/

void parseLine(const string& line, string& fileName, string& fileHash, vector<string>& cdnList) {
	string currWord = "";
	int wordCount = 0;
	for(int i=0; i<line.size(); i++) {
		if(line[i] != ' ') {
			currWord += line[i];
		} else {
			if(wordCount==0)
				fileName = currWord;
			else if(wordCount==1)
				fileHash = currWord;
			else
				cdnList.push_back(currWord);
			wordCount++;
			currWord = "";
		}
	}
	if(wordCount==1) //when the cdnList is empty
		fileHash = currWord;
	else if(wordCount>1)
		cdnList.push_back(currWord);
}

vector<string> MetaServer::getCdnsThatContainFile(string fileName) {
	ifstream file;
	string currFileName = m_file+"_v" + char('0'+m_version);
	file.open(currFileName.c_str());
	if(!file.is_open()) {
		cout<<"MetaServer::getCdnsThatContainFile - file not opened"<<endl;
		exit(0);
	}

	string currLine;
	while(getline(file, currLine)) {
		if(currLine.substr(0, fileName.size())==fileName && currLine[fileName.size()]==' ') {
			string fileName="", fileHash="";
			vector<string> cdnsThatContainFile;
			parseLine(currLine, fileName, fileHash, cdnsThatContainFile);
			return cdnsThatContainFile;
		}
	}
	vector<string> emptyVec;
	return emptyVec;
}

string MetaServer::getClosestCDN(const vector<string>& cdnAddrList, string clientIpAddr) { //also checks load inside
	double minDist = -1;
	int minIdx = -1;

	for(int i=0; i<cdnAddrList.size(); i++) {
		if(!CDN_load_OK(cdnAddrList[i]))
			continue;
		double currDist = calculateDistance(cdnAddrList[i], clientIpAddr);
		if(minDist < 0 || currDist<minDist) {
			minDist = currDist;
			minIdx = i;
		}
	}

	if(minIdx==-1) //all the CDNs are heavily loaded or cdn list is empty
		return "";

	return cdnAddrList[minIdx];
}

bool MetaServer::CDN_load_OK( const string CDN_addr ) {
	return true; //always true for now
}

bool MetaServer::isCDN_closerThanFSS(string cdnIpAddr, string clientIpAddr) {
	double distToCDN = calculateDistance(cdnIpAddr, clientIpAddr);
	double distToFSS = calculateDistance(m_FssIpAddr, clientIpAddr);
	return distToCDN <= distToFSS;
}

vector< pair<string, string> > MetaServer::processListFromOriginDownload(const vector< pair<string, string> >& listFromClientApp, string clientIpAddr) {
	vector< pair<string, string> > result;
	ifstream file;
	string fileName = m_file+"_v" + char('0'+m_version);
	file.open(fileName.c_str());
	if(!file.is_open()) {
		cout<<"MetaServer::processListFromOriginDownload - file not opened"<<endl;
		exit(0);
	}
	unordered_map<string, string> clientNameToHashMap;
	for(int i=0; i<listFromClientApp.size(); i++) {
		clientNameToHashMap[listFromClientApp[i].first] = listFromClientApp[i].second;
	}

	string currLine;
	while(getline(file, currLine)) {
		string fileName="", fileHash="";
		vector<string> cdnsThatContainFile;
		parseLine(currLine, fileName, fileHash, cdnsThatContainFile);
		if(clientNameToHashMap.count(fileName)>0 && clientNameToHashMap[fileName]==fileHash) //no need to download since name and hash match
			continue;
		string candidateCdnAddr = getClosestCDN(cdnsThatContainFile, clientIpAddr);
		if(candidateCdnAddr != "" && isCDN_closerThanFSS(candidateCdnAddr, clientIpAddr)) { //cdn is closer than fss
			result.push_back(make_pair(fileName, candidateCdnAddr));
		} else {
			vector<string> allCDNs(m_setOfCdnIpAddr.begin(), m_setOfCdnIpAddr.end());
			string closestCDN = getClosestCDN(allCDNs, clientIpAddr);
			result.push_back(make_pair(fileName, closestCDN));
		}
	}

	return result;
}

vector< pair<string, string> > MetaServer::processListFromOriginUpload(const vector< pair<string, string> >& listFromClientApp, string clientIpAddr) {
	vector< pair<string, string> > result;
	ifstream file;
	string fileName = m_file+"_v" + char('0'+m_version);
	file.open(fileName.c_str());
	if(!file.is_open()) {
		cout<<"MetaServer::processListFromOriginUpload - file not opened"<<endl;
		exit(0);
	}
	unordered_map<string, string> clientNameToHashMap;
	for(int i=0; i<listFromClientApp.size(); i++) {
		clientNameToHashMap[listFromClientApp[i].first] = listFromClientApp[i].second;
	}

	vector<string> allCDNs(m_setOfCdnIpAddr.begin(), m_setOfCdnIpAddr.end());
	string closestCdnAddr = getClosestCDN(allCDNs, clientIpAddr);
	allCDNs.clear();

	//add the files that exist in both FSS and client local and need to be updated
	string currLine;
	while(getline(file, currLine)) {
		string fileName="", fileHash="";
		vector<string> cdnsThatContainFile;
		parseLine(currLine, fileName, fileHash, cdnsThatContainFile);
		if(clientNameToHashMap.count(fileName)==0) {
			continue;
		} else if(clientNameToHashMap[fileName]==fileHash) { //no need to download since name and hash match
			clientNameToHashMap.erase(fileName);
			continue;
		}
		result.push_back(make_pair(fileName, closestCdnAddr));
		clientNameToHashMap.erase(fileName); //erase to find out which client's files are new to FSS
	}

	//add the files that client has but FSS doesn't
	unordered_map<string, string>::iterator itr = clientNameToHashMap.begin();
	while(itr != clientNameToHashMap.end()) {
		result.push_back(make_pair(itr->first, closestCdnAddr));
		itr++;
	}

	return result;
}


/*----------------------------*/
/* for communication with CDN */
/*----------------------------*/

void MetaServer::deleteMetaEntry(string fileName) {
	string currFileName = m_file+"_v" + char('0'+m_version),
		   newFileName = m_file+"_v" + char('0'+(++m_version));
	ifstream fileIn(currFileName.c_str());
	if(!fileIn) {
		cout<<"MetaServer::deleteMetaEntry - file not opened"<<endl;
		exit(0); // NEED TO CHANGE
	}
	ofstream fileOut(newFileName.c_str());
	if(!fileOut) {
		cout<<"MetaServer::deleteMetaEntry - file not opened"<<endl;
		fileIn.close();
		exit(0);
	}
	bool found = false;
	string currLine;
	while(getline(fileIn, currLine)) {
		if(currLine.substr(0, fileName.size())==fileName && currLine[fileName.size()]==' ') {
			found = true;
			continue;
		}
		fileOut << currLine+"\n";
	}

	if(found==false)
		cout<<fileName+" is not found for delete operation"<<endl;
	fileIn.close();
	fileOut.close();
}

string constructLine(string fileName, const string& fileHash, const vector<string>& CdnAddrList) {
	string result = "";
	result += fileName + " " + fileHash;
	for(int i=0; i<CdnAddrList.size(); i++)
		result += " " + CdnAddrList[i];
	result += "\n";
	return result;
}

void MetaServer::addNewMetaEntry(string fileName, const string& fileHash, const vector<string>& CdnAddrList) {
	string currFileName = m_file+"_v" + char('0'+m_version);
	ofstream file(currFileName.c_str(), ios_base::app | ios_base::out);
	if(!file.is_open()) {
		cout<<"MetaServer::addNewMetaEntry - file not opened"<<endl;
		exit(0);
	}
	string lineToAdd = constructLine(fileName, fileHash, CdnAddrList);
	file << lineToAdd;
	//add a cdn ip address
	for(int i=0; i<CdnAddrList.size(); i++)
		addCdnIpAddr(CdnAddrList[i]);
	file.close();
}

void MetaServer::updateMetaEntry(string fileName, const string& fileHash, const vector<string>& CdnAddrList) {
	deleteMetaEntry(fileName);
	addNewMetaEntry(fileName, fileHash, CdnAddrList);
}

void MetaServer::addCdnToMetaEntry(string fileName, string cdnAddr) {
	string currFileName = m_file+"_v" + char('0'+m_version),
		   newFileName = m_file+"_v" + char('0'+(++m_version));
	ifstream fileIn(currFileName.c_str());
	if(!fileIn) {
		cout<<"MetaServer::addCdnToMetaEntry - file not opened"<<endl;
		exit(0);
	}
	ofstream fileOut(newFileName.c_str());
	if(!fileOut) {
		cout<<"MetaServer::addCdnToMetaEntry - file not opened"<<endl;
		fileIn.close();
		exit(0);
	}
	bool found = false;
	string currLine;
	while(getline(fileIn, currLine)) {
		if(currLine.substr(0, fileName.size())==fileName && currLine[fileName.size()]==' ') {
			found = true;
			currLine += " "+cdnAddr;
		}
		fileOut << currLine+"\n";
	}
	if(found==false)
		cout<<fileName+" is not found for add CDN operation"<<endl;
	fileIn.close();
	fileOut.close();
}

