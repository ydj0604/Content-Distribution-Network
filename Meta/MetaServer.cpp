#ifdef _WIN32
	#include <unordered_map>
	#include <unordered_set>
#else
	#include <tr1/unordered_map>
	#include <tr1/unordered_set>
#endif

#include "MetaServer.h"
#include "../Origin/OriginServer.h"
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;
using namespace std::tr1;

MetaServer::MetaServer(string file, OriginServer* origin) {
	m_file = file;
	m_origin = origin;
	// Initialize MetaServer receiver/sender objects
}

MetaServer::~MetaServer() {
		// destructor for MetaServer class object
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
	if(m_ipAddrToLatLngMap.count(ipAddr)==0)
		return make_pair(0.0, 0.0); //ipAddr doesn't exist in our map
	return m_ipAddrToLatLngMap[ipAddr];
}

double MetaServer::calculateDistance(string ipAddr1, string ipAddr2) { //in kilometers
	pair<double, double> loc1 = getLatLng(ipAddr1), loc2 = getLatLng(ipAddr2);
	double theta, dist;
	theta = loc1.second - loc2.second;
	dist = sin(deg2rad(loc1.first)) * sin(deg2rad(loc2.first)) + cos(deg2rad(loc1.first)) * cos(deg2rad(loc2.first)) * cos(deg2rad(theta));
	dist = acos(dist);
	dist = rad2deg(dist);
	dist = dist * 60 * 1.1515 * 1.609344;
	return dist;
}


/*-------------------------------*/
/* for communication with Origin */
/*-------------------------------*/

vector<string> MetaServer::getCdnsThatContainFile(string fileName) {
	//read from m_file and retrieves the list of CDN addresses
	vector<string> result;
	return result;
}

string MetaServer::getClosestCDN(const vector<string>& cdnAddrList, string clientIpAddr) { //also checks load inside
	if(cdnAddrList.size()==0)
		return "";

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

	if(minIdx==-1) //all the CDNs are heavily loaded
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

vector<pair<string, string>> MetaServer::processListFromOriginDownload(const vector<pair<string, string>>& listFromClientApp, string clientIpAddr) {
	vector<pair<string, string>> result;
	ifstream file;
	file.open(m_file.c_str());
	if(!file.is_open())
		return result; //file not open => return an empty vector

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
		if(isCDN_closerThanFSS(candidateCdnAddr, clientIpAddr)) { //cdn is closer than fss
			result.push_back(make_pair(fileName, candidateCdnAddr));
		} else {
			vector<string> allCDNs(m_setOfCdnIpAddr.begin(), m_setOfCdnIpAddr.end());
			string closestCDN = getClosestCDN(allCDNs, clientIpAddr);
			result.push_back(make_pair(fileName, closestCDN));
		}
	}

	return result;
}

vector<pair<string, string>> MetaServer::processListFromOriginUpload(const vector<pair<string, string>>& listFromClientApp, string clientIpAddr) {
	vector<pair<string, string>> result;
	ifstream file;
	file.open(m_file.c_str());
	if(!file.is_open())
		return result; //file not open => return an empty vector

	unordered_map<string, string> clientNameToHashMap;
	for(int i=0; i<listFromClientApp.size(); i++) {
		clientNameToHashMap[listFromClientApp[i].first] = listFromClientApp[i].second;
	}

	vector<string> allCDNs(m_setOfCdnIpAddr.begin(), m_setOfCdnIpAddr.end());
	string closestCdnAddr = getClosestCDN(allCDNs, clientIpAddr);

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

}

void MetaServer::addNewMetaEntry(string fileName, const string& fileHash, const vector<string>& CdnAddrList) {

}

void MetaServer::updateMetaEntryHash(string fileName, const string& fileHash) {

}

void MetaServer::addCdnToMetaEntry(string fileName, string CdnAddrList) {

}

