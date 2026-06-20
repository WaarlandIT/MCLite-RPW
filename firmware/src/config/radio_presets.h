#pragma once

#include "ConfigManager.h"
#include <math.h>

// Region radio presets — mirror the config tool's PRESETS table
// (tools/config-tool: eu_narrow / us_ca). On-device the Radio screen offers
// these as a picker instead of raw freq/SF/BW/CR fields, so users can't make a
// regulatory/compat mistake. Applying a preset is a reboot-on-leave change.

namespace mclite {

struct RadioPreset {
    const char* key;     // stable id (matches config tool)
    const char* label;   // human label
    float       freq;    // MHz
    uint8_t     sf;
    float       bw;      // kHz
    uint8_t     cr;
    int8_t      tx;      // dBm
};

// Keep in sync with tools/config-tool PRESETS.
static constexpr RadioPreset RADIO_PRESETS[] = {
    { "eu_narrow", "EU/UK/CH", 869.618f, 8, 62.5f, 8, 22 },
    { "us_ca",     "US/Canada", 910.525f, 7, 62.5f, 5, 22 },
};
static constexpr size_t RADIO_PRESET_COUNT = sizeof(RADIO_PRESETS) / sizeof(RADIO_PRESETS[0]);

// Apply a preset's frequency/SF/BW/CR/TX onto a RadioConfig (scope/path-hash
// untouched). Returns false if the index is out of range.
inline bool applyRadioPreset(RadioConfig& r, size_t idx) {
    if (idx >= RADIO_PRESET_COUNT) return false;
    const RadioPreset& p = RADIO_PRESETS[idx];
    r.frequency       = p.freq;
    r.spreadingFactor = p.sf;
    r.bandwidth       = p.bw;
    r.codingRate      = p.cr;
    r.txPower         = p.tx;
    return true;
}

// Index of the preset matching the current radio params, or -1 for "Custom".
// TX power is intentionally excluded — it's separately adjustable on-device, so
// a user-lowered TX still reads as its region preset.
inline int matchRadioPreset(const RadioConfig& r) {
    for (size_t i = 0; i < RADIO_PRESET_COUNT; i++) {
        const RadioPreset& p = RADIO_PRESETS[i];
        if (fabsf(r.frequency - p.freq) < 0.001f &&
            r.spreadingFactor == p.sf &&
            fabsf(r.bandwidth - p.bw) < 0.01f &&
            r.codingRate == p.cr) {
            return (int)i;
        }
    }
    return -1;
}

}  // namespace mclite
