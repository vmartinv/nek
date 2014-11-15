#ifndef NESTYPES_H
#define NESTYPES_H

#include <types.h>


// Integer types
typedef uint_least32_t u32;
typedef uint_least16_t u16;
typedef uint_least8_t   u8;
typedef  int_least8_t   s8;

// Bitfield utilities
template<unsigned bitno, unsigned nbits=1, typename T=u8>
struct RegBit{
    T data;
    enum { mask = (1u << nbits) - 1u };
    template<typename T2>
    RegBit& operator=(T2 val)
    {
        data = (data & ~(mask << bitno)) | ((nbits > 1 ? val & mask : !!val) << bitno);
        return *this;
    }
    operator unsigned() const { return (data >> bitno) & mask; }
    RegBit& operator++ ()     { return *this = *this + 1; }
    unsigned operator++ (int) { unsigned r = *this; ++*this; return r; }
};

#endif
