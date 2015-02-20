#include "cpprest/http_listener.h"
#include "../Origin/OriginClientReceiver.h"

using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

int main() {
    utility::string_t port = U("34568");

    utility::string_t address = U("http://localhost:");
    address.append(port);

    OriginClientReceiver::intialize(address);

    std::cout << "Press ENTER to stop OriginClientReceiver." << std::endl;

    std::string line;
    std::getline(std::cin, line);

    OriginClientReceiver::shutDown();
    return 0;
}
