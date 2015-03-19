#include "fss.h"
#include <string>
#include "cpprest/http_listener.h"
#include "cpprest/http_client.h"
#include "../ipToLatLng/ipToLatLng.h"
#include <iostream>
#include <stdlib.h> // for system calls

using namespace std;
using namespace web;
using namespace utility;
using namespace http;
using namespace json;
using namespace web::http::experimental::listener;
using namespace web::http::client;

FSS::FSS(string metaIpAddrPort, string fssIpAddrPort) {
  // get the ip_address of the client and lat/lng
  // Get client ip instance
  fss_addr = fssIpAddrPort;
  fss_ipport =fssIpAddrPort;

  //ip_instance = new ipToLatLng();
  //fss_ipport = ip_instance->getipaddr();

  /*
  // use GET http request to retrieve client's latitude/longitude
  ip_instance->IPJsonToLatLng( fss_ipport );
  fss_lat = ip_instance->getlat();
  fss_lng = ip_instance->getlng();
  */

  //hard code fss location for testing purposes
  fss_lat = 34.05;
  fss_lng = -118.44;

  cout<<"FSS IP ADDR: " + fss_ipport<<endl;
  cout<<"FSS LAT: " + to_string(fss_lat)<<endl;
  cout<<"FSS LNG: " + to_string(fss_lng)<<endl;

  // Start communication for FSS
  uri_builder getUri("http://"+fss_addr);
  getUri.append_path("get");
  get_listener = http_listener(getUri.to_uri().to_string());
  get_listener.support(methods::GET, std::bind(&FSS::handle_get, this, std::placeholders::_1));

  uri_builder postUri("http://"+fss_addr);
  postUri.append_path("post");
  post_listener = http_listener(postUri.to_uri().to_string());
  post_listener.support(methods::POST, std::bind(&FSS::handle_post, this, std::placeholders::_1));

  m_metaIpAddrPort = metaIpAddrPort;
  register_with_meta(); //send register msg to meta


}

FSS::~FSS() {
  get_listener.close();
  post_listener.close();
}

void FSS::listen() {
  cout << "[ FSS ] Listening at address " << fss_addr << endl;
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

void FSS::register_with_meta() {
	http_client meta_client = http_client("http://"+m_metaIpAddrPort);

	json::value jsonObj = json::value::object();
	jsonObj[U("Type")] = json::value::number(1);
	jsonObj[U("IP")] = json::value::string(U(fss_ipport));
	jsonObj[U("Lat")] = json::value::number(fss_lat);
	jsonObj[U("Lng")] = json::value::number(fss_lng);

	http_response resp = meta_client.request(methods::POST, U("/meta/register/"), jsonObj).get();
	if (resp.status_code() != status_codes::OK) {
		cout<<"FSS::register_with_meta - registration failed"<<endl;
	}
}

void FSS::handle_get(http_request message) {
  /*
   * Invoked when cdn would like to recieve a file
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
  /*
   * Invoked when cdn would like to place a file
   * /post/path/to/file.txt
   *
   * Places post content body in file of that path
   *
   * Responds with
   *  200 OK, always
   */
  string fileName = message.relative_uri().to_string();
  string filePath = FSS_DIR + fileName;
  string contents = message.extract_string().get();

  // Diagnostics
  cout << endl << "---------------"<< endl;
  cout << "POST " << filePath << endl;
  cout << message.to_string() << endl;
  //cout << contents << endl;

  // Create paths as needed
  // As usual, ignore injection and other security concerns, because school project
  string cmd = "mkdir -p $(dirname " + filePath + ")";
  int ret = system(cmd.c_str());
  if (ret != 0)
    cout << "Error creating directories for path " << filePath << endl;

  // Write to file
  ofstream f;
  f.open(filePath);
  f << contents;
  f.close();

  message.reply(status_codes::OK, "");
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
