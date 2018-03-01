#include <rawrtcdc.h>
#include "data_transport.h"

#define DEBUG_MODULE "data-transport"
//#define RAWRTC_DEBUG_MODULE_LEVEL 7 // Note: Uncomment this to debug this module only
#include <rawrtcc/internal/debug.h>

/*
 * Destructor for an existing data transport.
 */
static void rawrtc_data_transport_destroy(
        void* arg
) {
    struct rawrtc_data_transport* const transport = arg;

    // Un-reference
    mem_deref(transport->transport);
}

/*
 * Create a data transport instance.
 */
enum rawrtc_code rawrtc_data_transport_create(
        struct rawrtc_data_transport** const transportp, // de-referenced
        enum rawrtc_data_transport_type const type,
        void* const internal_transport, // referenced
        rawrtc_data_transport_channel_create_handler* const channel_create_handler,
        rawrtc_data_transport_channel_close_handler* const channel_close_handler,
        rawrtc_data_transport_channel_send_handler* const channel_send_handler
) {
    struct rawrtc_data_transport* transport;

    // Check arguments
    if (!transportp || !internal_transport || !channel_create_handler) {
        return RAWRTC_CODE_INVALID_ARGUMENT;
    }

    // Allocate
    transport = mem_zalloc(sizeof(*transport), rawrtc_data_transport_destroy);
    if (!transport) {
        return RAWRTC_CODE_NO_MEMORY;
    }

    // Set fields
    transport->type = type;
    transport->transport = mem_ref(internal_transport);
    transport->channel_create = channel_create_handler;
    transport->channel_close = channel_close_handler;
    transport->channel_send = channel_send_handler;

    // Set pointer & done
    DEBUG_PRINTF("Created data transport of type %s\n", rawrtc_data_transport_type_to_str(type));
    *transportp = transport;
    return RAWRTC_CODE_SUCCESS;
}

/*
 * Get the data transport's type and underlying transport reference.
 * `*internal_transportp` must be unreferenced.
 */
enum rawrtc_code rawrtc_data_transport_get_transport(
        enum rawrtc_data_transport_type* const typep, // de-referenced
        void** const internal_transportp, // de-referenced
        struct rawrtc_data_transport* const transport
) {
    // Check arguments
    if (!typep || !transport) {
        return RAWRTC_CODE_INVALID_ARGUMENT;
    }

    // Set type & transport
    *typep = transport->type;
    *internal_transportp = mem_ref(transport->transport);

    // Done
    return RAWRTC_CODE_SUCCESS;
}

static enum rawrtc_data_transport_type const map_enum_data_transport_type[] = {
    RAWRTC_DATA_TRANSPORT_TYPE_SCTP,
};

static char const * const map_str_data_transport_type[] = {
    "SCTP",
};

static size_t const map_data_transport_type_length = ARRAY_SIZE(map_enum_data_transport_type);

/*
 * Translate a data transport type to str.
 */
char const * rawrtc_data_transport_type_to_str(
        enum rawrtc_data_transport_type const type
) {
    size_t i;

    for (i = 0; i < map_data_transport_type_length; ++i) {
        if (map_enum_data_transport_type[i] == type) {
            return map_str_data_transport_type[i];
        }
    }

    return "???";
}
