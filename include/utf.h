#ifndef UTF_H
#define UTF_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
typedef uint16_t char16_t;
#endif

typedef char utf8_t;
typedef char16_t utf16_t;
typedef char latin1_t;

#ifdef __cplusplus
}
#endif

#endif // UTF_H
