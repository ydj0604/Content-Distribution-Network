#ifndef CDN_META_RECEIVER_H
#define CDN_META_RECEIVER_H

#include "cpprest/http_listener.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class CDN_Node;

class CDNMetaReceiver {
public:
	CDNMetaReceiver(utility::string_t url);
	void setCDN(CDN_Node* cdn) { m_cdn = cdn; }
	static void initialize(const string_t& address, CDN_Node* meta);
	static void shutDown();
	static CDNMetaReceiver* getInstance() { return m_instance; }
    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }
private:
    void handle_delete(http_request message);
    CDN_Node* m_cdn;
    http_listener m_listener;
    static CDNMetaReceiver* m_instance;
};

#endif
