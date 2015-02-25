#ifndef ORIGIN_CLIENT_RECEIVER_H
#define ORIGIN_CLIENT_RECEIVER_H

#include "cpprest/http_listener.h"

class OriginServer;

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class OriginClientReceiver {
public:
    OriginClientReceiver(OriginServer* origin=NULL) { m_origin = origin; }
    OriginClientReceiver(utility::string_t url_explicit, utility::string_t url_sync);
    void setOrigin(OriginServer* origin) { m_origin = origin; }
    static void initialize(const string_t& address, OriginServer* origin);
    static void shutDown();
    static OriginClientReceiver* getInstance() { return m_instance; }
    pplx::task<void> open_explicit() { return m_listener_explicit.open(); }
    pplx::task<void> close_explicit() { return m_listener_explicit.close(); }
    pplx::task<void> open_sync() { return m_listener_sync.open(); }
    pplx::task<void> close_sync() { return m_listener_sync.close(); }
private:
    void handle_explicit(http_request message);
    void handle_sync(http_request message);
    OriginServer* m_origin;
    http_listener m_listener_explicit;
    http_listener m_listener_sync;
    static OriginClientReceiver* m_instance;
};


#endif
