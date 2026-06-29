#pragma once
// LVGL memory allocator redirected to PSRAM on boards that have it.
// Falls back to DRAM if PSRAM alloc fails (fragmentation / OOM safety net).
// On targets without PSRAM (native_test) this is a transparent passthrough.
#ifdef BOARD_HAS_PSRAM
#  include <esp_heap_caps.h>
static inline void* lv_psram_malloc(size_t size) {
    void* p = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!p) p = malloc(size);
    return p;
}
static inline void* lv_psram_realloc(void* ptr, size_t size) {
    void* p = heap_caps_realloc(ptr, size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!p && size > 0) p = realloc(ptr, size);
    return p;
}
#else
#  include <stdlib.h>
static inline void* lv_psram_malloc(size_t size)             { return malloc(size); }
static inline void* lv_psram_realloc(void* ptr, size_t size) { return realloc(ptr, size); }
#endif
