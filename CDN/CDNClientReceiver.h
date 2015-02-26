#ifndef CDN_CLIENT_RECEIVER_H
#define CDN_CLIENT_RECEIVER_H

#include "cpprest/http_listener.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class CDN_Node;

class CDNClientReceiver {
public:
	CDNClientReceiver(utility::string_t url);
	void setCDN(CDN_Node* cdn) { m_cdn = cdn; }
	static void initialize(const string_t& address, CDN_Node* cdn);
	static void shutDown();
	static CDNClientReceiver* getInstance() { return m_instance; }
    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }
private:
    void handle_get(http_request message);
    CDN_Node* m_cdn;
    http_listener m_listener;
    static CDNClientReceiver* m_instance;
};

#endif
