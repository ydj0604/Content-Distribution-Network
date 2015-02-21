#include "cpprest/http_listener.h"
#include "../Origin/OriginClientReceiver.h"
#include "../Origin/OriginServer.h"
#include "../Meta/MetaServer.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

void simpleTest() {
    utility::string_t port = U("34568");
    utility::string_t address = U("http://localhost:");
    address.append(port);

    OriginClientReceiver::initialize(address, NULL);

    std::cout << "Press ENTER to stop OriginClientReceiver." << std::endl;

    std::string line;
    std::getline(std::cin, line);

    OriginClientReceiver::shutDown();
}

void endToEndTest() {
	OriginServer* origin = new OriginServer();
	MetaServer* meta = new MetaServer("TestFile", origin);
	origin->setMeta(meta);

	//populate meta server
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

	//start origin server
	origin->startListening();
    std::cout << "Press ENTER to stop OriginClientReceiver." << std::endl;
    std::string line;
    std::getline(std::cin, line);
    origin->endListening();
}

int main() {
	endToEndTest();
}
