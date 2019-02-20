#pragma once

/**
 * Data transport type.
 */
enum rawrtc_data_transport_type {
    RAWRTC_DATA_TRANSPORT_TYPE_SCTP
};

/**
 * Generic data transport.
 */
struct rawrtc_data_transport;

/**
 * Translate a data transport type to str.
 */
char const * rawrtc_data_transport_type_to_str(
    enum rawrtc_data_transport_type const type
);