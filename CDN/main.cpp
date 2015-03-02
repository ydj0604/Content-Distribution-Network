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
	string metaIpAddr="http://localhost:5000", fssIpAddr="http://localhost:5000";
	if(argc > 2) {
		string temp(argv[1]);
		metaIpAddr = temp;
		string temp2(argv[2]);
		fssIpAddr = temp2;
		cout<<"Meta IP Addr: " + metaIpAddr<<endl;
		cout<<"FSS IP Addr: " + fssIpAddr<<endl;
	}

	CDN_Node* cdn = new CDN_Node(metaIpAddr, fssIpAddr);
	cdn->startListening();
	std::cout << "Press ENTER to stop CDN." << std::endl;
	std::string line;
	std::getline(std::cin, line);
	cdn->endListening();
}
