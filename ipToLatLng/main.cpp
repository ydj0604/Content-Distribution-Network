#include <iostream>
#include "ipToLatLng.h"
//#include "cpprest/http_listener.h"
#include <string>

using namespace std;

int main() {
    ipToLatLng* ip_instance = new ipToLatLng();

    cout << ip_instance->getipaddr() << endl;

    return 0;
}
