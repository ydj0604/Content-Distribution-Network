#include <iostream>
#include "../Meta/MetaServer.h"
#include "../Origin/OriginServer.h"

using namespace std;

void testMetaForCommunicationWithCDN() {
	MetaServer *metaServer = new MetaServer("metaFile", NULL);
	string temp;
	while(true) {
		cout<<"type insert/delete/update/add: "<<endl;
		cin>>temp;
		if(temp=="insert") {
			string fileName, fileHash, cdnAddr;
			cout<<"file name: ";
			cin>>fileName;
			cout<<endl;
			cout<<"file hash: ";
			cin>>fileHash;
			cout<<endl;
			cout<<"CDN addresses: ";
			vector<string> cdnList;
			while(true) {
				cin>>cdnAddr;
				if(cdnAddr=="" || cdnAddr==".")
					break;
				cdnList.push_back(cdnAddr);
			}
			cout<<endl;
			metaServer->addNewMetaEntry(fileName, fileHash, cdnList);

		} else if(temp=="delete") {
			string fileName;
			cout<<"file name: ";
			cin>>fileName;
			cout<<endl;
			metaServer->deleteMetaEntry(fileName);

		} else if(temp=="update") {
			string fileName, fileHash, cdnAddr;
			cout<<"file name: ";
			cin>>fileName;
			cout<<endl;
			cout<<"file hash: ";
			cin>>fileHash;
			cout<<endl;
			cout<<"CDN addresses: ";
			vector<string> cdnList;
			while(true) {
				cin>>cdnAddr;
				if(cdnAddr=="" || cdnAddr==".")
					break;
				cdnList.push_back(cdnAddr);
			}
			cout<<endl;
			metaServer->updateMetaEntry(fileName, fileHash, cdnList);

		} else if(temp=="add") {
			string fileName, cdnAddr;
			cout<<"file name: ";
			cin>>fileName;
			cout<<endl;
			cout<<"cdn address: ";
			cin>>cdnAddr;
			cout<<endl;
			metaServer->addCdnToMetaEntry(fileName, cdnAddr);

		} else if(temp=="Q") {
			break;
		} else {
			cout<<"wrong command"<<endl;
			continue;
		}
	}
}

void testCalculateDistance() {
	//initialize
	OriginServer* origin = new OriginServer();
	MetaServer* meta = new MetaServer("metaFile", origin);
	origin->setMeta(meta);

	//add cdns and fss
	meta->addLatLngWithIpAddr("0.0.0.0", 34.05, -118.44); //client-la
	meta->addLatLngWithIpAddr("1.1.1.1", 37.77, -122.42); //cdn1-sf
	meta->addLatLngWithIpAddr("2.2.2.2", 47.61, -122.33); //cdn2-seattle
	meta->addLatLngWithIpAddr("3.3.3.3", 25.03, -77.40); //cdn3-bahama
	meta->addLatLngWithIpAddr("4.4.4.4", 40.34, 127.51); //cdn4-north korea
	meta->addLatLngWithIpAddr("255.255.255.255", 30.27, -97.74); //fss-austin

	cout<<meta->calculateDistance("0.0.0.0", "1.1.1.1")<<endl;
	cout<<meta->calculateDistance("0.0.0.0", "2.2.2.2")<<endl;
	cout<<meta->calculateDistance("0.0.0.0", "3.3.3.3")<<endl;
	cout<<meta->calculateDistance("0.0.0.0", "4.4.4.4")<<endl;
	cout<<meta->calculateDistance("0.0.0.0", "255.255.255.255")<<endl;

	delete origin;
	delete meta;
}

void testWholeFlow() {
	//initialize
	OriginServer* origin = new OriginServer();
	MetaServer* meta = new MetaServer("metaFile", origin);
	origin->setMeta(meta);
	meta->setFssIpAddr("255.255.255.255");

	//add cdns and fss
	meta->addLatLngWithIpAddr("0.0.0.0", 34.05, -118.44); //client-la
	meta->addLatLngWithIpAddr("1.1.1.1", 37.77, -122.42); //cdn1-sf
	meta->addLatLngWithIpAddr("2.2.2.2", 47.61, -122.33); //cdn2-seattle
	meta->addLatLngWithIpAddr("3.3.3.3", 25.03, -77.40); //cdn3-bahama
	meta->addLatLngWithIpAddr("4.4.4.4", 40.34, 127.51); //cdn4-north korea
	meta->addLatLngWithIpAddr("255.255.255.255", 30.27, -97.74); //fss-austin

	//add files
	string aname="a", ahash="ahash";
	vector<string> alist;
	alist.push_back("1.1.1.1");
	meta->addNewMetaEntry(aname, ahash, alist);

	string bname="b", bhash="bhash";
	vector<string> blist;
	blist.push_back("2.2.2.2");
	meta->addNewMetaEntry(bname, bhash, blist);

	string cname="c", chash="chash";
	vector<string> clist;
	clist.push_back("3.3.3.3");
	meta->addNewMetaEntry(cname, chash, clist);

	string dname="d", dhash="dhash";
	vector<string> dlist;
	dlist.push_back("4.4.4.4");
	meta->addNewMetaEntry(dname, dhash, dlist);

	string ename="e", ehash="ehash";
	vector<string> elist;
	elist.push_back("1.1.1.1");
	elist.push_back("2.2.2.2");
	elist.push_back("3.3.3.3");
	elist.push_back("4.4.4.4");
	meta->addNewMetaEntry(ename, ehash, elist);

	string fname="f", fhash="fhash";
	vector<string> flist;
	meta->addNewMetaEntry(fname, fhash, flist);

	//test closest CDN
	vector<string> cdnVec;
	cdnVec.push_back("1.1.1.1");
	cdnVec.push_back("2.2.2.2");
	cdnVec.push_back("3.3.3.3");
	cdnVec.push_back("4.4.4.4");
	cout<<"Closest: "<<meta->getClosestCDN(cdnVec, "0.0.0.0")<<endl;
	cout<<endl;

	//test closer than FSS
	for(int i=0; i<cdnVec.size(); i++) {
		if(meta->isCDN_closerThanFSS(cdnVec[i], "0.0.0.0"))
			cout<<cdnVec[i]<<" is closer than FSS"<<endl;
	}
	cout<<endl;

	//test cdns that contain a file
	vector<string> cdnsThatContainA = meta->getCdnsThatContainFile("a"),
				   cdnsThatContainB = meta->getCdnsThatContainFile("b"),
				   cdnsThatContainC = meta->getCdnsThatContainFile("c"),
				   cdnsThatContainD = meta->getCdnsThatContainFile("d"),
				   cdnsThatContainE = meta->getCdnsThatContainFile("e"),
				   cdnsThatContainF = meta->getCdnsThatContainFile("f");

	for(int i=0; i<cdnsThatContainA.size(); i++)
		cout<<cdnsThatContainA[i]<<" contains a"<<endl;
	for(int i=0; i<cdnsThatContainB.size(); i++)
		cout<<cdnsThatContainB[i]<<" contains b"<<endl;
	for(int i=0; i<cdnsThatContainC.size(); i++)
		cout<<cdnsThatContainC[i]<<" contains c"<<endl;
	for(int i=0; i<cdnsThatContainD.size(); i++)
		cout<<cdnsThatContainD[i]<<" contains d"<<endl;
	for(int i=0; i<cdnsThatContainE.size(); i++)
		cout<<cdnsThatContainE[i]<<" contains e"<<endl;
	for(int i=0; i<cdnsThatContainF.size(); i++)
		cout<<cdnsThatContainF[i]<<" contains f"<<endl;
	cout<<endl;

	//test download list
	vector< pair<string, string> > clientList;
	clientList.push_back(make_pair("a", "ahash"));
	clientList.push_back(make_pair("b", "bhash2"));
	clientList.push_back(make_pair("g", "ghash"));
	vector< pair<string, string> > listToDownload = meta->processListFromOriginDownload(clientList, "0.0.0.0");

	cout<<"client 1 needs to download... "<<endl;
	for(int i=0; i<listToDownload.size(); i++) {
		string fileName = listToDownload[i].first,
			   address = listToDownload[i].second;
		cout<<fileName<<" from "<<address<<endl;
	}
	cout<<endl;

	vector< pair<string, string> > clientList2;
	vector< pair<string, string> > listToDownload2 = meta->processListFromOriginDownload(clientList2, "0.0.0.0");

	cout<<"client 2 needs to download... "<<endl;
	for(int i=0; i<listToDownload2.size(); i++) {
		string fileName = listToDownload2[i].first,
			   address = listToDownload2[i].second;
		cout<<fileName<<" from "<<address<<endl;
	}
	cout<<endl;

}

int main() {
	testWholeFlow();
}
