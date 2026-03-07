#ifndef OPEN_EARABLE_U64_TO_STR_H
#define OPEN_EARABLE_U64_TO_STR_H

#include <stddef.h>
#include <stdint.h>

// Convert uint64_t to decimal string without using %llu (avoids nRF52/ARM toolchain issues).
// buf must have at least 21 bytes for full uint64 range.
inline void u64_to_str(uint64_t val, char* buf, size_t buf_size) {
    if (buf_size < 2) {
        if (buf_size > 0) buf[0] = '\0';
        return;
    }
    if (val == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    char tmp[22];
    int i = 0;
    while (val > 0 && i < 21) {
        tmp[i++] = (char)('0' + (val % 10));
        val /= 10;
    }
    int j = 0;
    while (i > 0 && j < (int)buf_size - 1) {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';
}

#endif // OPEN_EARABLE_U64_TO_STR_H
