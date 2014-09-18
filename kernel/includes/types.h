#ifndef TYPES_H
#define TYPES_H

#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>
#include <nesos.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// These tell gcc how to optimise branches since it's stupid
#define likely(x)    __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

#define __used	__attribute__((__used__))

#define ENDIAN_DWORD_SWAP(x) ((x >> 24) & 0xFF) | ((x << 8) & 0xFF0000) | ((x >> 8) & 0xFF00) | ((x << 24) & 0xFF000000)
#define ENDIAN_WORD_SWAP(x) ((x & 0xFF) << 0x08) | ((x & 0xFF00) >> 0x08)

#endif
