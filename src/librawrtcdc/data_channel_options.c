#include <rawrtcdc.h>
#include "data_channel_options.h"

/*
 * Default data channel options.
 */
struct rawrtc_data_channel_options rawrtc_default_data_channel_options = {
    .deliver_partially = false
};

/*
 * Create data channel options.
 *
 * `*optionsp` must be unreferenced.
 *
 * If `deliver_partially` is set to `true`, you will receive partial
 * messages. If set to `false`, messages will be reassembled before
 * delivery. If enabled, message chunks will be delivered until the
 * message is complete. Other messages' chunks WILL NOT be interleaved
 * on the same channel.
 */
enum rawrtc_code rawrtc_data_channel_options_create(
        struct rawrtc_data_channel_options** const optionsp, // de-referenced
        bool const deliver_partially
) {
    struct rawrtc_data_channel_options* options;

    // Check arguments
    if (!optionsp) {
        return RAWRTC_CODE_INVALID_ARGUMENT;
    }

    // Allocate
    options = mem_zalloc(sizeof(*options), NULL);
    if (!options) {
        return RAWRTC_CODE_NO_MEMORY;
    }

    // Set fields
    options->deliver_partially = deliver_partially;

    // Set pointer & done
    *optionsp = options;
    return RAWRTC_CODE_SUCCESS;
}
