#ifndef _FYNAOS_TYPES_H
#define _FYNAOS_TYPES_H

/*
 * fixed-sized types
 */

typedef signed   char      int8_t;
typedef signed   short     int16_t;
typedef signed   int       int32_t;
typedef signed   long long int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

/*
 * pointer-sized
 */

typedef int64_t   intptr_t;
typedef uint64_t  uintptr_t;
typedef int64_t   ssize_t;
typedef uint64_t  size_t;
typedef uintptr_t phys_addr_t;
typedef uintptr_t virt_addr_t;
typedef uintptr_t page_index_t;

/*
 * null pointer
 */

#ifndef NULL
# ifdef __cplusplus
#  define NULL nullptr
# else
#  define NULL ((void*)0)
# endif
#endif

/*
 * boolean
 */

typedef char boolean_t;

#define TRUE  1
#define FALSE 0

/*
 * macros
 */

#define _counof(arr) \
        ((size_t)(sizeof(arr) / sizeof((arr)[0])))

#define _align_up(addr, align) \
        ((((uintptr_t)(addr)) + ((uintptr_t)(align) - 1)) & ~((uintptr_t)(align) - 1))

#define _align_down(addr, align) \
        (((uintptr_t)(addr)) & ~((uintptr_t)(align) - 1))

#define _offsetof __builtin_offsetof

#define _container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - _offsetof(type, member)))

#define INVALID_PHYSICAL_ADDRESS (phys_addr_t)-1
#define INVALID_VIRTUAL_ADDRESS  (phys_addr_t)-1

/*
 * kernel result
 */

typedef uint32_t kresult_t;
#define _ksuccess(result) !((result) & 0x80000000)
#define _kfailed(result)  !_ksuccess(result)

/*
 * attributes
 */

#define __noreturn    __attribute__((noreturn))
#define __forceinline __attribute__((always_inline))
#define __fallthrough __attribute__((__fallthrough__))

#endif /* _FYNAOS_TYPES_H */
