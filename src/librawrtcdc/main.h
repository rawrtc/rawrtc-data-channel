#pragma once
#include <rawrtcdc.h>

/*
 * Global rawrtcdc vars.
 */
struct rawrtcdc_global {
#ifdef HAVE_PTHREAD
    pthread_mutex_t mutex;
#endif
    uint_fast32_t usrsctp_initialized;
    struct tmr usrsctp_tick_timer;
    size_t usrsctp_chunk_size;
};
