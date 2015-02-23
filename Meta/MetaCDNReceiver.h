#ifndef META_CDN_RECEIVER_H
#define META_CDN_RECEIVER_H

#include "cpprest/http_listener.h"

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class MetaServer;

class MetaCDNReceiver {
public:
	MetaCDNReceiver(utility::string_t url, int type);
	void setMeta(MetaServer* meta) { m_meta = meta; }
	static void initialize(const string_t& address, MetaServer* meta);
	static void shutDown();
	static MetaCDNReceiver* getUpdateInstance() { return m_updateInstance; }
	static MetaCDNReceiver* getDeleteInstance() { return m_deleteInstance; }
	static MetaCDNReceiver* getRegisterInstance() { return m_registerInstance; }
    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }
private:
    void handle_post(http_request message);
    void handle_delete(http_request message);
    void handle_register(http_request message);
    MetaServer* m_meta;
    http_listener m_listener;
    static MetaCDNReceiver* m_updateInstance;
    static MetaCDNReceiver* m_deleteInstance;
    static MetaCDNReceiver* m_registerInstance;
};

#endif
