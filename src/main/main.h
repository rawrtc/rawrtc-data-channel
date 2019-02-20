#pragma once
#include <rawrtcdc/main.h>

#include <re.h>

extern struct rawrtcdc_global rawrtcdc_global;

/*
 * Global RAWRTCDC settings.
 */
struct rawrtcdc_global {
    rawrtcdc_timer_handler timer_handler;
    uint_fast32_t usrsctp_initialized;
    size_t usrsctp_chunk_size;
};
