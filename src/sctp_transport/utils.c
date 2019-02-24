#include <rawrtcdc/sctp_transport.h>

/*
 * Get the corresponding name for an SCTP transport state.
 */
char const* rawrtc_sctp_transport_state_to_name(enum rawrtc_sctp_transport_state const state) {
    switch (state) {
        case RAWRTC_SCTP_TRANSPORT_STATE_NEW:
            return "new";
        case RAWRTC_SCTP_TRANSPORT_STATE_CONNECTING:
            return "connecting";
        case RAWRTC_SCTP_TRANSPORT_STATE_CONNECTED:
            return "connected";
        case RAWRTC_SCTP_TRANSPORT_STATE_CLOSED:
            return "closed";
        default:
            return "???";
    }
}
