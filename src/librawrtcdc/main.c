#ifdef HAVE_PTHREAD
#include <pthread.h> // pthread_*
#endif
#include <rawrtcdc.h>
#include "main.h"

#define DEBUG_MODULE "rawrtcdc-main"
//#define RAWRTC_DEBUG_MODULE_LEVEL 7 // Note: Uncomment this to debug this module only
#include "debug.h"

struct rawrtcdc_global rawrtcdc_global;

/*
 * Initialise rawrtc. Must be called before making a call to any other
 * function.
 *
 * Note: In case you override the default mutex used by re it's vital
 *       that you create a recursive mutex or you will get deadlocks!
 */
enum rawrtc_code rawrtcdc_init(
        bool const init_re
) {
    int err;
#ifdef HAVE_PTHREAD
    pthread_mutexattr_t mutex_attribute;
#endif

    // Initialise re (if requested)
    if (init_re) {
        if (libre_init()) {
            return RAWRTC_CODE_INITIALISE_FAIL;
        }
    }

#ifdef HAVE_PTHREAD
    // Initialise and set mutex attribute
    // Note: A recursive mutex is required as an upcall can trigger an upcall.
    err = pthread_mutexattr_init(&mutex_attribute);
    if (err) {
        DEBUG_WARNING("Failed to initialise mutex attribute, reason: %m\n", err);
        return rawrtc_error_to_code(err);
    }
    err = pthread_mutexattr_settype(&mutex_attribute, PTHREAD_MUTEX_RECURSIVE);
    if (err) {
        DEBUG_WARNING("Failed to set mutex attribute, reason: %m\n", err);
        return rawrtc_error_to_code(err);
    }

    // Initialise mutex
    err = pthread_mutex_init(&rawrtcdc_global.mutex, &mutex_attribute);
    if (err) {
        DEBUG_WARNING("Failed to initialise mutex, reason: %m\n", err);
        return rawrtc_error_to_code(err);
    }

    // Register mutex on re
    re_set_mutex(&rawrtcdc_global.mutex);
#endif

    // Set usrsctp initialised counter
    rawrtcdc_global.usrsctp_initialized = 0;

    // Done
    return RAWRTC_CODE_SUCCESS;
}

/*
 * Close rawrtc and free up all resources.
 *
 * Note: In case `close_re` is not set to `true`, you MUST close
 *       re yourselves.
 */
enum rawrtc_code rawrtcdc_close(
        bool const close_re
) {
    int err;

    // TODO: Close usrsctp if initialised

#ifdef HAVE_PTHREAD
    // Destroy mutex
    err = pthread_mutex_destroy(&rawrtcdc_global.mutex);
    if (err) {
        DEBUG_WARNING("Failed to destroy mutex, reason: %m\n", err);
    }
#endif

    // Close re (if requested)
    if (close_re) {
        libre_close();
    }

    // Done
    return RAWRTC_CODE_SUCCESS;
}
