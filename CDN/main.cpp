#include "../CDN/CDN_Node.h"
#include "../CDN/CDNSender.h"
#include "../CDN/CDNReceiver.h"
#include "cpprest/http_client.h"
#include "cpprest/asyncrt_utils.h"
#include "cpprest/rawptrstream.h"
#include "cpprest/containerstream.h"
#include "cpprest/producerconsumerstream.h"
#include "cpprest/filestream.h"
#include <string>
#include <iostream>

using namespace http::client;
using namespace std;
using namespace web;
using namespace utility;
using namespace concurrency;

int main(int argc, char *argv[]) {
	string cdnIpAddr = "http://localhost:4000", metaIpAddr="http://localhost:5000", fssIpAddr="http://localhost:6000";
	if(argc > 3) {
		string temp(argv[1]);
		cdnIpAddr = temp;
		string temp1(argv[2]);
		metaIpAddr = temp1;
		string temp2(argv[3]);
		fssIpAddr = temp2;
	}

	CDN_Node* cdn = new CDN_Node(cdnIpAddr, metaIpAddr, fssIpAddr);
	cdn->startListening();
	std::cout << "Press ENTER to stop CDN." << std::endl;
	std::string line;
	std::getline(std::cin, line);
	cdn->endListening();
}
