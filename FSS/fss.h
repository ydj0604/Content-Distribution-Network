#ifndef CDN_FSS_H
#define CDN_FSS_H

#include "cpprest/http_listener.h"

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;

#define FSS_ADDR "http://localhost:5000/"
#define FSS_DIR "../../testdir"

class FSS {
public:
  FSS();
  ~FSS();
  void listen();
private:
  // Request function
  void handle_get(http_request message);
  void handle_put(http_request message);
  http_listener get_listener;
  http_listener put_listener;

  // Other utility
  bool has_file(string filePath);
  string get_file_contents(string filePath);
};
#endif
