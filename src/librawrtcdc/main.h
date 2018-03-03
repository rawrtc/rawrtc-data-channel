#pragma once
#ifdef HAVE_PTHREAD
#include <pthread.h> // pthread_*
#endif
#include <rawrtcdc.h>

extern struct rawrtcdc_global rawrtcdc_global;

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
