#ifndef CDN_FSS_H
#define CDN_FSS_H

#include "cpprest/http_listener.h"
#include "../ipToLatLng/ipToLatLng.h"

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;

// #define FSS_ADDR "http://localhost:5000/"
#define FSS_DIR "./FSS_Storage"

class FSS {
public:
  FSS(string metaIpAddrPort="localhost:4000", string fssIpAddrPort="localhost:5000");
  ~FSS();
  void listen();
private:
  // Request function
  void register_with_meta();
  void handle_get(http_request message);
  void handle_post(http_request message);
  http_listener get_listener;
  http_listener post_listener;

  // Other utility
  bool has_file(string filePath);
  string get_file_contents(string filePath);
  string fss_ipport;
  string fss_addr;
  double fss_lat, fss_lng;
  string m_metaIpAddrPort;
  ipToLatLng* ip_instance;
};
#endif
