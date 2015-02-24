#include <iostream>
#include "ipToLatLng.h"
//#include "cpprest/http_listener.h"
#include <stdlib.h>
#include <string>

using namespace std;

// constructor to get my public ip address from bash cmdline
ipToLatLng::ipToLatLng() {
    myipaddr = exec( "dig +short myip.opendns.com @resolver1.opendns.com" );
}

string ipToLatLng::exec( const char* cmd ) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;

}

// function to convert ip address to latitude and longitude
void ip_to_latlng( string ip_addr ) {
    // use myipaddr obtained above



    return;
}
