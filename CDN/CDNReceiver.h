#ifndef CDN_RECEIVER_H
#define CDN_RECEIVER_H

#include "cpprest/http_listener.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class CDN_Node;

class CDNReceiver {
public:
	CDNReceiver(utility::string_t url);
	void setCDN(CDN_Node* cdn) { m_cdn = cdn; }
	static void initialize(const string_t& address, CDN_Node* cdn);
	static void shutDown();
	static CDNReceiver* getInstance() { return m_instance; }
    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }
private:
    void handle_delete(http_request message); //meta invalidates files
    void handle_get(http_request message); //client gets files from cdn
    void handle_put(http_request message); //client transfers files to cdn
    CDN_Node* m_cdn;
    http_listener m_listener;
    static CDNReceiver* m_instance;
};

#endif
