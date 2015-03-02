#ifndef IP_TO_LATLNG
#define IP_TO_LATLNG

#include "cpprest/http_client.h"
#include <string>
#include <stdlib.h>

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace json;

class ipToLatLng {
public:
    ipToLatLng();
    string exec( const char* cmd );	// c++ use of calling shell command in order to return my public ip address
    void fixString( string& str );
    void IPJsonToLatLng( string ip_addr );
    string getipaddr() { return myipaddr; };
    double getlat() { return mylat; };
    double getlng() { return mylng; };

private:
    string myipaddr;
    double mylat, mylng;

};

#endif
