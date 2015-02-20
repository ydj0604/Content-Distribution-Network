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
    OriginClientReceiver(utility::string_t url);
    void setOrigin(OriginServer* origin) { m_origin = origin; }
    static void initialize(const string_t& address);
    static void shutDown();
    static OriginClientReceiver* getInstance() { return m_instance; }

    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }

private:

    void handle_get(http_request message);
    void handle_put(http_request message);
    void handle_post(http_request message);
    void handle_delete(http_request message);

    OriginServer* m_origin;
    http_listener m_listener;
    static OriginClientReceiver* m_instance;
};


#endif
