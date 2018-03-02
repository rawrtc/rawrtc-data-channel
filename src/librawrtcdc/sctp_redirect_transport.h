#pragma once

enum {
    RAWRTC_SCTP_REDIRECT_TRANSPORT_DEFAULT_PORT = 5000,
};

/*
 * Redirect transport.
 */
struct rawrtc_sctp_redirect_transport {
    enum rawrtc_sctp_redirect_transport_state state;
    struct rawrtc_dtls_transport* dtls_transport; // referenced
    uint16_t local_port;
    uint16_t remote_port;
    struct sa redirect_address;
    struct mbuf* buffer;
    int socket;
};
