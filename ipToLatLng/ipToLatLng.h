#ifndef IP_TO_LATLNG
#define IP_TO_LATLNG

#include <string>
#include <stdlib.h>

using namespace std;

class ipToLatLng {
public:
  ipToLatLng();
  string exec( const char* cmd );	// c++ use of calling shell command in order to return my public ip address
  void ip_to_latlng( string ip_addr );
  string getipaddr() { return myipaddr; };
  double getlat() { return mylat; };
  double getlng() { return mylng; };

private:
  string myipaddr;
  double mylat, mylng;

};

#endif
