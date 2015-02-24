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
  uri_builder getUri(FSS_ADDR);
  getUri.append_path("get");
  get_listener = http_listener(getUri.to_uri().to_string());
  get_listener.support(methods::GET, std::bind(&FSS::handle_get, this, std::placeholders::_1));


  uri_builder postUri(FSS_ADDR);
  postUri.append_path("post");
  post_listener = http_listener(postUri.to_uri().to_string());
  post_listener.support(methods::POST, std::bind(&FSS::handle_post, this, std::placeholders::_1));
}

FSS::~FSS() {
  get_listener.close();
  post_listener.close();
}

void FSS::listen() {
  cout << "[ FSS ] Listening at address " << FSS_ADDR << endl;
  try {
    get_listener
      .open()
      .wait();
    post_listener
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
   *
   * Responds with either
   *  200 OK, and file contents
   *  404 Not found, if requested file does not exist
   */
  string fileName = message.relative_uri().to_string();
  string filePath = FSS_DIR + fileName;

  bool fileExists = has_file(filePath);

  // Diagnostics
  cout << endl << "---------------"<< endl;
  cout << filePath << endl;
  cout << message.to_string() << endl;
  cout << "File exists: " << fileExists << endl;

  if (fileExists)
    message.reply(status_codes::OK, get_file_contents(filePath));
  else
    message.reply(status_codes::NotFound, "");
}

void FSS::handle_post(http_request message) {
  cout << "post recieved " << message.relative_uri().to_string() << endl;
}

bool FSS::has_file(string filePath) {
  ifstream file(filePath.c_str());
  if (!file)
    return false;
  else
    return true;
}

string FSS::get_file_contents(string filePath) {
  // Return contents of file
  // Assumes check for file existing already handled
  ifstream f(filePath);
  stringstream buf;
  buf << f.rdbuf();

  return buf.str();
}
