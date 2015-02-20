#include <unordered_map>
#include <unordered_set>

#include "MetaServer.h"
#include "MetaCDNReceiver.h"
#include "MetaCDNSender.h"
#include "../Origin/OriginServer.h"
#include <string>
#include <utility>
#include <vector>
#include <math.h>
#include <sys/stat.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ios>

using namespace std;

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
}

MetaServer::~MetaServer() {
	//clean up
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

void MetaServer::setFssAddr(Address fss) {
	m_FssAddr = fss;
}
void MetaServer::addCdnAddr(Address cdn) {
	m_cdnAddrList.push_back(cdn);
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

double MetaServer::calculateDistance(Address addr1, Address addr2) { //in miles
	double theta, dist;
	pair<double, double> loc1 =  addr1.latLng;
	pair<double, double> loc2 = addr2.latLng;
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

Address MetaServer::parseAddress(const string& line) { // "23,45,1.1.1.1" -> { latLng: pair<23, 45>, ipAddr:"1.1.1.1" }
	string latStr = "", lngStr = "", ipAddrStr = "";
	double lat, lng;
	int idx = 0;

	while(idx<line.size() && line[idx]!=',') {
		latStr += line[idx];
		idx++;
	}
	lat = strtod(latStr.c_str(), NULL);

	idx++;
	while(idx<line.size() && line[idx]!=',') {
		lngStr += line[idx];
		idx++;
	}
	lng = strtod(lngStr.c_str(), NULL);

	idx++;
	while(idx<line.size()) {
		ipAddrStr += line[idx];
		idx++;
	}

	Address result(make_pair(lat, lng), ipAddrStr);
	return result;
}

void parseLine(const string& line, string& fileName, string& fileHash, vector<Address>& cdnList) {
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
			else {
				Address addr = MetaServer::parseAddress(currWord);
				cdnList.push_back(addr);
			}
			wordCount++;
			currWord = "";
		}
	}
	if(wordCount==1) //when the cdnList is empty
		fileHash = currWord;
	else if(wordCount>1) {
		Address addr = MetaServer::parseAddress(currWord);
		cdnList.push_back(addr);
	}
}

vector<Address> MetaServer::getCdnsThatContainFile(string fileName) {
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
			vector<Address> cdnsThatContainFile;
			parseLine(currLine, fileName, fileHash, cdnsThatContainFile);
			return cdnsThatContainFile;
		}
	}
	vector<Address> emptyVec;
	return emptyVec;
}

bool MetaServer::CDN_load_OK(Address cdnAddr) {
	return true; //always true for now
}

Address MetaServer::getClosestCDN(const vector<Address>& cdnAddrList, Address clientAddr) { //also checks load inside
	double minDist = -1;
	int minIdx = -1;

	for(int i=0; i<cdnAddrList.size(); i++) {
		if(!CDN_load_OK(cdnAddrList[i]))
			continue;
		double currDist = calculateDistance(cdnAddrList[i], clientAddr);
		if(minDist < 0 || currDist<minDist) {
			minDist = currDist;
			minIdx = i;
		}
	}

	if(minIdx==-1) { //all the CDNs are heavily loaded or cdn list is empty
		Address errResult(make_pair(0.0, 0.0), "");
		return errResult;
	}

	return cdnAddrList[minIdx];
}


bool MetaServer::isCDN_closerThanFSS(Address cdnAddr, Address clientAddr) {
	double distToCDN = calculateDistance(cdnAddr, clientAddr);
	double distToFSS = calculateDistance(m_FssAddr, clientAddr);
	return distToCDN <= distToFSS;
}

vector< pair<string, Address> > MetaServer::processListFromOriginDownload(const vector< pair<string, string> >& listFromClientApp, Address clientAddr) {
	vector< pair<string, Address> > result;
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
		vector<Address> cdnsThatContainFile;
		parseLine(currLine, fileName, fileHash, cdnsThatContainFile);
		if(clientNameToHashMap.count(fileName)>0 && clientNameToHashMap[fileName]==fileHash) //no need to download since name and hash match
			continue;
		Address candidateCdnAddr = getClosestCDN(cdnsThatContainFile, clientAddr);
		if(!(candidateCdnAddr.latLng.first==0.0 && candidateCdnAddr.latLng.second==0.0) && isCDN_closerThanFSS(candidateCdnAddr, clientAddr)) { //cdn is closer than fss
			result.push_back(make_pair(fileName, candidateCdnAddr));
		} else {
			Address closestCDN = getClosestCDN(m_cdnAddrList, clientAddr);
			result.push_back(make_pair(fileName, closestCDN));
		}
	}

	return result;
}

vector< pair<string, Address> > MetaServer::processListFromOriginUpload(const vector< pair<string, string> >& listFromClientApp, Address clientAddr) {
	vector< pair<string, Address> > result;
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

	Address closestCdnAddr = getClosestCDN(m_cdnAddrList, clientAddr);

	//add the files that exist in both FSS and client local and need to be updated
	string currLine;
	while(getline(file, currLine)) {
		string fileName="", fileHash="";
		vector<Address> cdnsThatContainFile;
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

string constructLine(string fileName, const string& fileHash, const vector<Address>& CdnAddrList) {
	string result = "";
	result += fileName + " " + fileHash;
	for(int i=0; i<CdnAddrList.size(); i++)
		result += " " + to_string(CdnAddrList[i].latLng.first) + "," + to_string(CdnAddrList[i].latLng.second) + "," + CdnAddrList[i].ipAddr;
	result += "\n";
	return result;
}

void MetaServer::addNewMetaEntry(string fileName, const string& fileHash, const vector<Address>& CdnAddrList) {
	string currFileName = m_file+"_v" + char('0'+m_version);
	ofstream file(currFileName.c_str(), ios_base::app | ios_base::out);
	if(!file.is_open()) {
		cout<<"MetaServer::addNewMetaEntry - file not opened"<<endl;
		exit(0);
	}
	string lineToAdd = constructLine(fileName, fileHash, CdnAddrList);
	file << lineToAdd;
	file.close();
}

void MetaServer::updateMetaEntry(string fileName, const string& fileHash, const vector<Address>& CdnAddrList) {
	deleteMetaEntry(fileName);
	addNewMetaEntry(fileName, fileHash, CdnAddrList);
}

void MetaServer::addCdnToMetaEntry(string fileName, Address cdnAddr) {
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
			currLine += " " + to_string(cdnAddr.latLng.first) + "," + to_string(cdnAddr.latLng.second) + "," + cdnAddr.ipAddr;
		}
		fileOut << currLine+"\n";
	}
	if(found==false)
		cout<<fileName+" is not found for add CDN operation"<<endl;
	fileIn.close();
	fileOut.close();
}

