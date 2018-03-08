#pragma once
#include <rawrtcdc.h>

/*
 * Compute a CRC-32C. If the crc32 instruction is available, use the hardware
 * version. Otherwise, use the software version.
 */
uint32_t rawrtc_crc32c(
    uint32_t crc,
    void const* buffer,
    size_t length
);

/*
 * Initialise CRC-32C.
 */
void rawrtc_crc32c_init();
