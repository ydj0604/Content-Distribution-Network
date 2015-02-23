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
	MetaCDNReceiver(utility::string_t update_url, utility::string_t delete_url, utility::string_t register_url);
	void setMeta(MetaServer* meta) { m_meta = meta; }
	static void initialize(const string_t& address, MetaServer* meta);
	static void shutDown();
	static MetaCDNReceiver* getInstance() { return m_instance; }
    pplx::task<void> updateOpen() { return m_update_listener.open(); }
    pplx::task<void> updateClose() { return m_update_listener.close(); }
    pplx::task<void> deleteOpen() { return m_delete_listener.open(); }
    pplx::task<void> deleteClose() { return m_delete_listener.close(); }
    pplx::task<void> registerOpen() { return m_register_listener.open(); }
    pplx::task<void> registerClose() { return m_register_listener.close(); }
private:
    void handle_update(http_request message);
    void handle_delete(http_request message);
    void handle_register(http_request message);
    MetaServer* m_meta;
    http_listener m_update_listener;
    http_listener m_delete_listener;
    http_listener m_register_listener;
    static MetaCDNReceiver* m_instance;
};

#endif
