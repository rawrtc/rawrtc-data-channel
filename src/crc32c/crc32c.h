#pragma once
#include <re.h>

uint32_t rawrtc_crc32c(
    uint32_t crc,
    void const* buffer,
    size_t length
);

void rawrtc_crc32c_init(void);
