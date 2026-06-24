// Stub mbedtls base64 for native tests — a real (small) implementation so the
// hex/base64 key-decode paths (e.g. ConfigManager::findContactIndexByKey) are
// genuinely exercised, not faked. Mirrors the mbedtls API signatures.
#pragma once
#include <cstdint>
#include <cstddef>

inline int mbedtls_base64_decode(unsigned char* dst, size_t dlen, size_t* olen,
                                 const unsigned char* src, size_t slen) {
    static const auto val = [](unsigned char c) -> int {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '+') return 62;
        if (c == '/') return 63;
        return -1;
    };
    size_t out = 0;
    int acc = 0, bits = 0;
    for (size_t i = 0; i < slen; i++) {
        unsigned char c = src[i];
        if (c == '=' || c == '\r' || c == '\n' || c == ' ') continue;  // padding / whitespace
        int v = val(c);
        if (v < 0) return -1;  // invalid char
        acc = (acc << 6) | v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            if (out >= dlen) return -1;  // MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL
            dst[out++] = (unsigned char)((acc >> bits) & 0xFF);
        }
    }
    if (olen) *olen = out;
    return 0;
}

inline int mbedtls_base64_encode(unsigned char* dst, size_t dlen, size_t* olen,
                                 const unsigned char* src, size_t slen) {
    static const char* B = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t out = 0;
    for (size_t i = 0; i < slen; i += 3) {
        uint32_t n = src[i] << 16;
        if (i + 1 < slen) n |= src[i + 1] << 8;
        if (i + 2 < slen) n |= src[i + 2];
        char c0 = B[(n >> 18) & 63], c1 = B[(n >> 12) & 63];
        char c2 = (i + 1 < slen) ? B[(n >> 6) & 63] : '=';
        char c3 = (i + 2 < slen) ? B[n & 63] : '=';
        if (out + 4 > dlen) return -1;
        dst[out++] = c0; dst[out++] = c1; dst[out++] = c2; dst[out++] = c3;
    }
    if (out < dlen) dst[out] = '\0';
    if (olen) *olen = out;
    return 0;
}
