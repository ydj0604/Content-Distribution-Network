#include <iostream>
#include "../Meta/MetaServer.h"
#include "../Shared.h"

using namespace std;

void testCalculateDistance() {
	//initialize
	MetaServer* meta = new MetaServer("metaFile", NULL);

	Address la_client(make_pair(34.05, -118.44), "0.0.0.0");
	Address sf(make_pair(37.77, -122.42), "1.1.1.1");
	Address seattle(make_pair(47.61, -122.33), "2.2.2.2");
	Address bahama(make_pair(25.03, -77.40), "3.3.3.3");
	Address northkorea(make_pair(40.34, 127.51), "4.4.4.4");
	Address austin_fss(make_pair(30.27, -97.74), "255.255.255.255");

	cout<<"la-sf "<<meta->calculateDistance(la_client, sf)<<endl;
	cout<<"la-seattle "<<meta->calculateDistance(la_client, seattle)<<endl;
	cout<<"la-bahama "<<meta->calculateDistance(la_client, bahama)<<endl;
	cout<<"la-northkorea "<<meta->calculateDistance(la_client, northkorea)<<endl;
	cout<<"la-austin "<<meta->calculateDistance(la_client, austin_fss)<<endl;
}

void testReadWrite() {
	MetaServer *metaServer = new MetaServer("metaFile", NULL);
	string temp;
	while(true) {
		cout<<"type insert/delete/update/addcdn: "<<endl;
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
			vector<Address> cdnList;
			while(true) {
				cin>>cdnAddr;
				if(cdnAddr=="" || cdnAddr==".")
					break;
				cdnList.push_back(MetaServer::parseAddress(cdnAddr));
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
			vector<Address> cdnList;
			while(true) {
				cin>>cdnAddr;
				if(cdnAddr=="" || cdnAddr==".")
					break;
				cdnList.push_back(MetaServer::parseAddress(cdnAddr));
			}
			cout<<endl;
			metaServer->updateMetaEntry(fileName, fileHash, cdnList);

		} else if(temp=="addcdn") {
			string fileName, cdnAddr;
			cout<<"file name: ";
			cin>>fileName;
			cout<<endl;
			cout<<"cdn address: ";
			cin>>cdnAddr;
			cout<<endl;
			metaServer->addCdnToMetaEntry(fileName, MetaServer::parseAddress(cdnAddr));

		} else if(temp=="Q") {
			break;
		} else {
			cout<<"wrong command"<<endl;
			continue;
		}
	}
}

void testProcess() {
	MetaServer *meta = new MetaServer("metaFile", NULL);

	Address la_client(make_pair(34.05, -118.44), "0.0.0.0");
	Address sf(make_pair(37.77, -122.42), "1.1.1.1");
	Address seattle(make_pair(47.61, -122.33), "2.2.2.2");
	Address bahama(make_pair(25.03, -77.40), "3.3.3.3");
	Address northkorea(make_pair(40.34, 127.51), "4.4.4.4");
	Address austin_fss(make_pair(30.27, -97.74), "255.255.255.255");

	meta->addCdnAddr(sf);
	meta->addCdnAddr(seattle);
	meta->addCdnAddr(bahama);
	meta->addCdnAddr(northkorea);
	meta->setFssAddr(austin_fss);

	Address closest = meta->getClosestCDN(meta->getCdnList(), la_client);
	cout<<"Closest to client: "<< closest.ipAddr << endl;

	if(meta->isCDN_closerThanFSS(northkorea, la_client))
		cout<<"nk is closer than austin from la"<<endl;

	vector<Address> acdn; acdn.push_back(sf);
	meta->addNewMetaEntry("a", "ahash", acdn);

	vector<Address> bcdn; bcdn.push_back(seattle);
	meta->addNewMetaEntry("b", "bhash", bcdn);

	vector<Address> ccdn; ccdn.push_back(bahama);
	meta->addNewMetaEntry("c", "chash", ccdn);

	vector<Address> dcdn; dcdn.push_back(northkorea);
	meta->addNewMetaEntry("d", "dhash", dcdn);

	vector<Address> ecdn;
	ecdn.push_back(sf);
	ecdn.push_back(seattle);
	ecdn.push_back(bahama);
	ecdn.push_back(northkorea);
	meta->addNewMetaEntry("e", "ehash", ecdn);

	vector<Address> fcdn; //empty
	meta->addNewMetaEntry("f", "fhash", fcdn);

	//client request
	vector< pair<string, string> > clientList;
	clientList.push_back(make_pair("a", "ahash1")); //file names are unique
	clientList.push_back(make_pair("b", "bhash2"));
	vector< pair<string, Address> > arequest = meta->processListFromOriginDownload(clientList, la_client);
	cout<<"Client needs to download: "<<endl;
	for(int i=0; i<arequest.size(); i++) {
		cout<<arequest[i].first<<" from "<<arequest[i].second.ipAddr<<endl;
	}


	cout<<"DONE"<<endl;
	delete meta;
}

int main() {
	testProcess();
}
