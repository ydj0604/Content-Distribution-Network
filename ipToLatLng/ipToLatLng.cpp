#include <iostream>
#include "ipToLatLng.h"
#include "cpprest/http_client.h"
#include "cpprest/uri_builder.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace json;

// constructor to get my public ip address from bash cmdline
ipToLatLng::ipToLatLng() {
    myipaddr = exec( "dig +short myip.opendns.com @resolver1.opendns.com" );
    fixString(myipaddr);
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

void ipToLatLng::fixString( string& str ) {
    string ret_str = "";
    int i = 0;
    while (str[i] != '\n') {
	ret_str += str[i];
	i++;
    }

    str = ret_str;
    return;
}

// function to convert ip address to latitude and longitude
void ipToLatLng::IPJsonToLatLng( string ip_addr ) {

    try {
        // use ip_addr to create URI to which GET method will be sent
        uri_builder ip_url(U("http://www.geoplugin.net/json.gp"));
        ip_url.set_query(U("ip=" + ip_addr));

        client::http_client client(ip_url.to_uri()); // create client instance

        ucout << string_t(U("created client instance and now sending GET request...")) << endl;

        // request the URI using GET method and return json value object
        pplx::task<http_response> task = client.request( methods::GET );
        http_response response = task.get();

	if (response.status_code() == status_codes::OK) {

	    cout << "status code OK" << endl;

	    string_t bodyText = response.extract_string().get();
	    
	    // parsing body of the response into json object
	    stringstream_t ssBody;
	    ssBody << bodyText;
	    json::value json = json::value::parse(ssBody);

	    if (json.is_null()) {
		fprintf(stderr, "JSON object is empty!");
	        return;	// json object is empty
            }

	    ucout << U("Parsing my latitude...") << endl;

	    string str_lat = json.at(U("geoplugin_latitude")).as_string();
	    mylat = stod(str_lat);
	    
	    ucout << U("Parsing my longitude...") << endl;

	    string str_lng = json.at(U("geoplugin_longitude")).as_string();
	    mylng = stod(str_lng);

	    ucout << U("Parsing completed!") << endl;
	}

    } catch ( json::json_exception &e ) {
	fprintf(stderr, "JSON object error: %s\n", e.what());
	return;
    }

    return;
}
