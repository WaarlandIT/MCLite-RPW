#include <unity.h>
#include "util/bmp.h"

using namespace mclite;

static uint32_t le32(const uint8_t* p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static uint16_t le16(const uint8_t* p) { return (uint16_t)(p[0] | (p[1] << 8)); }

void test_bmp_row_stride() {
    TEST_ASSERT_EQUAL_INT(960, bmpRowStride24(320));   // 320*3 = 960 (already /4)
    TEST_ASSERT_EQUAL_INT(1232, bmpRowStride24(410));  // 1230 → pad to 1232
    TEST_ASSERT_EQUAL_INT(4, bmpRowStride24(1));        // 3 → pad to 4
    TEST_ASSERT_EQUAL_INT(12, bmpRowStride24(3));       // 9 → pad to 12
}

void test_bmp_header_fields() {
    uint8_t h[BMP_HEADER_SIZE];
    bmpWriteHeader24(h, 320, 240);

    TEST_ASSERT_EQUAL_UINT8('B', h[0]);
    TEST_ASSERT_EQUAL_UINT8('M', h[1]);
    TEST_ASSERT_EQUAL_UINT32(54u, le32(h + 10));            // pixel data offset
    TEST_ASSERT_EQUAL_UINT32(40u, le32(h + 14));            // biSize
    TEST_ASSERT_EQUAL_UINT32(320u, le32(h + 18));           // biWidth
    TEST_ASSERT_EQUAL_INT32(-240, (int32_t)le32(h + 22));   // biHeight negative = top-down
    TEST_ASSERT_EQUAL_UINT16(1, le16(h + 26));              // biPlanes
    TEST_ASSERT_EQUAL_UINT16(24, le16(h + 28));             // biBitCount
    TEST_ASSERT_EQUAL_UINT32(0u, le32(h + 30));             // biCompression = BI_RGB
    // file size = 54 + rowStride(960) * 240
    TEST_ASSERT_EQUAL_UINT32(54u + 960u * 240u, le32(h + 2));
    TEST_ASSERT_EQUAL_UINT32(960u * 240u, le32(h + 34));    // biSizeImage
}

void test_bmp_header_odd_width_padding() {
    uint8_t h[BMP_HEADER_SIZE];
    bmpWriteHeader24(h, 3, 2);  // rowStride 12, image 24, file 78
    TEST_ASSERT_EQUAL_UINT32(54u + 12u * 2u, le32(h + 2));
    TEST_ASSERT_EQUAL_UINT32(12u * 2u, le32(h + 34));
}

void test_pixel_white_black() {
    uint8_t b, g, r;
    rgb565SwappedToBgr(0xFFFF, b, g, r);
    TEST_ASSERT_EQUAL_UINT8(255, r); TEST_ASSERT_EQUAL_UINT8(255, g); TEST_ASSERT_EQUAL_UINT8(255, b);
    rgb565SwappedToBgr(0x0000, b, g, r);
    TEST_ASSERT_EQUAL_UINT8(0, r); TEST_ASSERT_EQUAL_UINT8(0, g); TEST_ASSERT_EQUAL_UINT8(0, b);
}

void test_pixel_primaries_byteswapped() {
    uint8_t b, g, r;
    // Inputs are byte-SWAPPED RGB565 (as LVGL stores with LV_COLOR_16_SWAP=1).
    rgb565SwappedToBgr(0x00F8, b, g, r);  // native 0xF800 = pure red
    TEST_ASSERT_EQUAL_UINT8(255, r); TEST_ASSERT_EQUAL_UINT8(0, g); TEST_ASSERT_EQUAL_UINT8(0, b);
    rgb565SwappedToBgr(0xE007, b, g, r);  // native 0x07E0 = pure green
    TEST_ASSERT_EQUAL_UINT8(0, r); TEST_ASSERT_EQUAL_UINT8(255, g); TEST_ASSERT_EQUAL_UINT8(0, b);
    rgb565SwappedToBgr(0x1F00, b, g, r);  // native 0x001F = pure blue
    TEST_ASSERT_EQUAL_UINT8(0, r); TEST_ASSERT_EQUAL_UINT8(0, g); TEST_ASSERT_EQUAL_UINT8(255, b);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_bmp_row_stride);
    RUN_TEST(test_bmp_header_fields);
    RUN_TEST(test_bmp_header_odd_width_padding);
    RUN_TEST(test_pixel_white_black);
    RUN_TEST(test_pixel_primaries_byteswapped);
    return UNITY_END();
}
