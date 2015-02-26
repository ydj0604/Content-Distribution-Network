#include "cpprest/http_client.h"
#include <iostream>
#include <string>
#include "ipToLatLng.h"

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace json;

int main() {
    ipToLatLng* ip_instance = new ipToLatLng();

    cout << ip_instance->getipaddr() << endl;

    // using function IPJsonToLatLng(), obtain latitude, longitude from GET method
    ip_instance->IPJsonToLatLng( ip_instance->getipaddr() );

    cout << "Obtained latitude is: " << ip_instance->getlat() << endl;
    cout << "Obtained longitude is: " << ip_instance->getlng() << endl;

    return 0;
}
