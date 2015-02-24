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
  get_listener = http_listener(FSS_ADDR);

  get_listener.support(methods::GET, std::bind(&FSS::handle_get, this, std::placeholders::_1));
}

FSS::~FSS() {
  get_listener.close();
}

void FSS::listen() {
  cout << "[ FSS ] Listening at address " << FSS_ADDR << endl;
  try {
    get_listener
      .open()
      .wait();

      while (1);
  } catch (exception const &e) {
    cout << e.what() << endl;
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

  message.reply(status_codes::OK, "test");
}
