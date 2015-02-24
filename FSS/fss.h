#ifndef CDN_FSS_H
#define CDN_FSS_H

#include "cpprest/http_listener.h"

using namespace web;
using namespace utility;
using namespace http;
using namespace web::http::experimental::listener;

class FSS {
public:
  FSS();
  ~FSS();
  void listen();
private:
  void handle_get(http_request message);
  void handle_put(http_request message);
  http_listener get_listener;
  http_listener put_listener;
};
#endif
