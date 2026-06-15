#pragma once

// Minimal 24-bit BMP encoding helpers for the screenshot feature.
// Pure (no Arduino / no allocation) so they're unit-testable on the host.
//
// The LVGL snapshot buffer is byte-swapped RGB565 (LV_COLOR_DEPTH 16,
// LV_COLOR_16_SWAP 1); rgb565SwappedToBgr() unswaps and expands to 8-bit BGR,
// which is the byte order a BMP scanline wants.

#include <stdint.h>
#include <stddef.h>

namespace mclite {

// BMP header is always 14 (file) + 40 (info) = 54 bytes for a 24-bit BI_RGB image.
static constexpr size_t BMP_HEADER_SIZE = 54;

// Padded scanline length: each row is rounded up to a 4-byte boundary.
inline int bmpRowStride24(int w) { return ((w * 3 + 3) / 4) * 4; }

namespace detail {
inline void put16(uint8_t* p, uint16_t v) { p[0] = v & 0xFF; p[1] = (v >> 8) & 0xFF; }
inline void put32(uint8_t* p, uint32_t v) {
    p[0] = v & 0xFF; p[1] = (v >> 8) & 0xFF; p[2] = (v >> 16) & 0xFF; p[3] = (v >> 24) & 0xFF;
}
}  // namespace detail

// Fill a 54-byte 24-bit BMP header for a w×h image. biHeight is negative so the
// pixel rows are stored top-to-bottom (matches the snapshot buffer order).
inline void bmpWriteHeader24(uint8_t out[BMP_HEADER_SIZE], int w, int h) {
    const uint32_t rowStride = (uint32_t)bmpRowStride24(w);
    const uint32_t imageSize = rowStride * (uint32_t)h;
    const uint32_t fileSize  = (uint32_t)BMP_HEADER_SIZE + imageSize;

    for (size_t i = 0; i < BMP_HEADER_SIZE; i++) out[i] = 0;

    // BITMAPFILEHEADER (14 bytes)
    out[0] = 'B'; out[1] = 'M';
    detail::put32(out + 2, fileSize);          // bfSize
    // bytes 6-9 reserved = 0
    detail::put32(out + 10, BMP_HEADER_SIZE);  // bfOffBits

    // BITMAPINFOHEADER (40 bytes)
    detail::put32(out + 14, 40);               // biSize
    detail::put32(out + 18, (uint32_t)w);      // biWidth
    detail::put32(out + 22, (uint32_t)(-h));   // biHeight (negative → top-down)
    detail::put16(out + 26, 1);                // biPlanes
    detail::put16(out + 28, 24);               // biBitCount
    detail::put32(out + 30, 0);                // biCompression = BI_RGB
    detail::put32(out + 34, imageSize);        // biSizeImage
    detail::put32(out + 38, 2835);             // biXPelsPerMeter (~72 DPI)
    detail::put32(out + 42, 2835);             // biYPelsPerMeter
    // biClrUsed (46) + biClrImportant (50) = 0
}

// Convert one byte-swapped RGB565 pixel (as stored by LVGL with LV_COLOR_16_SWAP)
// into 8-bit B,G,R (BMP scanline order).
inline void rgb565SwappedToBgr(uint16_t swapped, uint8_t& b, uint8_t& g, uint8_t& r) {
    uint16_t px = (uint16_t)((swapped >> 8) | (swapped << 8));  // unswap → native RGB565
    uint8_t r5 = (px >> 11) & 0x1F;
    uint8_t g6 = (px >> 5)  & 0x3F;
    uint8_t b5 =  px        & 0x1F;
    r = (uint8_t)((r5 * 255 + 15) / 31);
    g = (uint8_t)((g6 * 255 + 31) / 63);
    b = (uint8_t)((b5 * 255 + 15) / 31);
}

}  // namespace mclite
