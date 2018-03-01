#pragma once
#include <rawrtcdc.h>

extern struct rawrtc_data_channel_options rawrtc_default_data_channel_options;

struct rawrtc_data_channel_options {
    bool deliver_partially;
};
