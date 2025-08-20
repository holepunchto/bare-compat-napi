#ifndef UTF_H
#define UTF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifndef __cplusplus
typedef uint16_t char16_t;
#endif

typedef unsigned char utf8_t;
typedef char16_t utf16_t;
typedef unsigned char latin1_t;

#ifdef __cplusplus
}
#endif

#endif // UTF_H
