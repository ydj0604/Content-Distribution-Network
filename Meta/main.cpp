#include "cpprest/http_listener.h"
#include "../Origin/OriginClientReceiver.h"
#include "../Origin/OriginServer.h"
#include "MetaServer.h"
#include "MetaCDNReceiver.h"
#include "../Shared.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

/*
void endToEndTest() {
	//servers
	Address la_client(make_pair(34.05, -118.44), "0.0.0.0");
	Address sf(make_pair(37.77, -122.42), "1.1.1.1");
	Address seattle(make_pair(47.61, -122.33), "2.2.2.2");
	Address bahama(make_pair(25.03, -77.40), "3.3.3.3");
	Address northkorea(make_pair(40.34, 127.51), "4.4.4.4");
	Address austin_fss(make_pair(30.27, -97.74), "255.255.255.255");

	//init
	OriginServer* origin = new OriginServer();
	MetaServer* meta = new MetaServer("TestFile", origin);
	origin->setMeta(meta);
	meta->setFssAddr(austin_fss);

	//start servers
	origin->startListening();
	meta->startListening();
    std::cout << "Press ENTER to stop Origin and Meta." << std::endl;
    std::string line;
    std::getline(std::cin, line);
    origin->endListening();
    meta->endListening();
}
*/

int main(int argc, char *argv[]) {
	string metaIpAddrPort = "localhost:4000", originIpAddrPort = "localhost:3000";
	if(argc > 2) {
		string temp(argv[1]);
		metaIpAddrPort = temp;
		string temp1(argv[2]);
		originIpAddrPort = temp1;
	}
	OriginServer* origin = new OriginServer(originIpAddrPort);
	MetaServer* meta = new MetaServer(metaIpAddrPort, "metaFile", origin);
	origin->setMeta(meta);
	origin->startListening();
	meta->startListening();
    std::cout << "Press ENTER to stop Origin and Meta." << std::endl;
    std::string line;
    std::getline(std::cin, line);
    origin->endListening();
    meta->endListening();
}
