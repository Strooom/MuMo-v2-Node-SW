// #############################################################################
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#if !defined strlcpy || !defined strlcat
#include <stdint.h>        // required for uint8_t and similar type definitions
#include <cstring>         // required for memcpy()
#endif

#ifndef strlcpy
// on some platforms, cstring does not contain strlcpy, so in this case we add it here directly
size_t
strlcpy(char* dst, const char* src, size_t maxlen) {
    const size_t srclen = strnlen(src, maxlen);
    if (srclen + 1 < maxlen) {
        (void)memcpy(dst, src, srclen + 1);
    } else if (maxlen != 0) {
        (void)memcpy(dst, src, maxlen - 1);
        dst[maxlen - 1] = '\0';
    }
    return srclen;
}
#endif

#ifndef strlcat
// on some platforms, cstring does not contain strlcpy, so in this case we add it here directly
size_t
strlcat(char* dst, const char* src, size_t maxlen) {
    const size_t srclen = strnlen(src, maxlen);
    const size_t dstlen = strnlen(dst, maxlen);
    if (dstlen == maxlen) return maxlen + srclen;
    if (srclen < maxlen - dstlen) {
        (void)memcpy(dst + dstlen, src, srclen + 1);
    } else {
        (void)memcpy(dst + dstlen, src, maxlen - 1);
        dst[dstlen + maxlen - 1] = '\0';
    }
    return dstlen + srclen;
}
#endif