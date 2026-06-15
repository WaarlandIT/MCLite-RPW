#include "Screenshot.h"
#include "UIManager.h"
#include "../hal/Display.h"
#include "../storage/SDCard.h"
#include "../config/ConfigManager.h"
#include "../util/TimeHelper.h"
#include "../util/bmp.h"
#include "../util/log.h"
#include "../i18n/I18n.h"
#include <lvgl.h>
#include <SD.h>
#include <Arduino.h>
#include <time.h>
#include <string.h>

namespace mclite {

static void toastFail() { UIManager::instance().showToast(t("screenshot_failed")); }

bool Screenshot::capture() {
    const auto& cfg = ConfigManager::instance().config();
    if (!cfg.debug.screenshots) return false;            // gated (caller also checks)

    auto& sd = SDCard::instance();
    if (!sd.isMounted()) { toastFail(); return false; }

    lv_obj_t* scr = lv_scr_act();
    if (!scr) { toastFail(); return false; }

    // Render the active screen into a PSRAM buffer (RGB565, byte-swapped).
    uint32_t sz = lv_snapshot_buf_size_needed(scr, LV_IMG_CF_TRUE_COLOR);
    if (sz == 0) { toastFail(); return false; }
    void* buf = ps_malloc(sz);
    if (!buf) buf = malloc(sz);                           // PSRAM unavailable → try DRAM
    if (!buf) { LOGLN("[Screenshot] alloc failed"); toastFail(); return false; }

    lv_img_dsc_t dsc;
    memset(&dsc, 0, sizeof(dsc));
    if (lv_snapshot_take_to_buf(scr, LV_IMG_CF_TRUE_COLOR, &dsc, buf, sz) != LV_RES_OK) {
        LOGLN("[Screenshot] snapshot failed");
        free(buf); toastFail(); return false;
    }

    int w = dsc.header.w, h = dsc.header.h;
    if (w <= 0 || h <= 0) { w = Display::width(); h = Display::height(); }

    sd.mkdir("/screenshots");

    // Filename: dated when the clock is synced, else a millis-based fallback.
    char path[56];
    uint32_t epoch = TimeHelper::instance().bestEpoch();
    if (TimeHelper::instance().isSynced() && epoch > 1700000000) {
        time_t tval = (time_t)epoch;
        struct tm tmv;
        localtime_r(&tval, &tmv);
        snprintf(path, sizeof(path), "/screenshots/%04d-%02d-%02d_%02d%02d%02d.bmp",
                 tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
                 tmv.tm_hour, tmv.tm_min, tmv.tm_sec);
    } else {
        snprintf(path, sizeof(path), "/screenshots/shot_%lu.bmp", (unsigned long)millis());
    }

    File f = SD.open(path, FILE_WRITE);
    if (!f) { LOGLN("[Screenshot] SD open failed"); free(buf); toastFail(); return false; }

    uint8_t header[BMP_HEADER_SIZE];
    bmpWriteHeader24(header, w, h);
    bool ok = (f.write(header, BMP_HEADER_SIZE) == BMP_HEADER_SIZE);

    int stride = bmpRowStride24(w);
    uint8_t* row = (uint8_t*)malloc(stride);
    if (!row) { f.close(); free(buf); toastFail(); return false; }
    memset(row, 0, stride);                               // pad bytes stay zero across rows

    const lv_color_t* px = (const lv_color_t*)buf;
    for (int y = 0; y < h && ok; y++) {
        const lv_color_t* srcRow = px + (size_t)y * w;
        int o = 0;
        for (int x = 0; x < w; x++) {
            uint8_t b, g, r;
            rgb565SwappedToBgr(srcRow[x].full, b, g, r);
            row[o++] = b; row[o++] = g; row[o++] = r;     // BMP scanline = BGR
        }
        if (f.write(row, stride) != (size_t)stride) ok = false;
    }

    free(row);
    f.close();
    free(buf);

    if (ok) {
        LOGF("[Screenshot] saved %s (%dx%d)\n", path, w, h);
        UIManager::instance().showToast(t("screenshot_saved"));
    } else {
        toastFail();
    }
    return ok;
}

}  // namespace mclite
