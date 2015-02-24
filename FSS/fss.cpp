#include "fss.h"
#include "cpprest/http_listener.h"
#include <iostream>

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace json;
using namespace web::http::experimental::listener;

FSS::FSS() {
  string addr = "http://localhost:5000/";
  get_listener = http_listener(addr);
  get_listener.support(methods::GET, std::bind(&FSS::handle_get, this, std::placeholders::_1));
  cout << "listening at address " << addr << endl;
  get_listener.open().wait();
}

FSS::~FSS() {
  get_listener.close();
}

void FSS::listen() {
  while (1) {
    // listen for http
    // either
    // save file
    // or respond with file contents
  }
}

void FSS::handle_get(http_request message) {
  /*
   * Envoked when cdn would like to recieve a file
   */
  string fileName = message.relative_uri().to_string();

  // Diagnostics
  cout << message.to_string() << endl;
  cout << fileName << endl;
}
