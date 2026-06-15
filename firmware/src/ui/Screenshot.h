#pragma once

namespace mclite {

// Capture the active screen to /screenshots/*.bmp on the SD card (24-bit BMP).
// Gated by config `debug.screenshots` (default off). Shows a toast on success or
// failure. Synchronous (renders + writes in the caller's context) — fine for a
// debug capture. Misses overlays on lv_layer_top (toasts/PIN/SOS) by design.
class Screenshot {
public:
    static bool capture();
};

}  // namespace mclite
