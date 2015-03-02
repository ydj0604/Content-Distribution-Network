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

using namespace http::client;
using namespace std;
using namespace web;
using namespace utility;
using namespace concurrency;

void endToEndTest() {
	CDN_Node* cdn = new CDN_Node("http://localhost:5000", "http://localhost:5000");
	cdn->startListening();
	std::cout << "Press ENTER to stop CDN." << std::endl;
	std::string line;
	std::getline(std::cin, line);
	cdn->endListening();

}

int main() {
	endToEndTest();
}
