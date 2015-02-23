#include "../CDN/CDNMetaReceiver.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

void simpleTest() {
	utility::string_t port = U("5000");
	utility::string_t address = U("http://localhost:");
	address.append(port);
	CDNMetaReceiver::initialize(address, NULL);
	std::cout << "Press ENTER to stop OriginClientReceiver." << std::endl;
	std::string line;
	std::getline(std::cin, line);
	CDNMetaReceiver::shutDown();
}

int main() {
	simpleTest();
}
