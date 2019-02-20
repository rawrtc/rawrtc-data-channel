#pragma once
#include <re.h>

struct rawrtc_sctp_capabilities {
    uint64_t max_message_size;
};

int rawrtc_sctp_capabilities_debug(
    struct re_printf* const pf,
    struct rawrtc_sctp_capabilities const* const capabilities
);
