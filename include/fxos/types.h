#ifndef FXOS_TYPES_H
#define FXOS_TYPES_H

//
// C types
//

typedef unsigned char      UCHAR,     *PUCHAR;
typedef unsigned short     USHORT,    *PUSHORT;
typedef unsigned int       UINT,      *PUINT;
typedef unsigned long long ULONGLONG, *PULONGLONG;
typedef char               CHAR,      *PCHAR;
typedef short              SHORT,     *PSHORT;
typedef int                INT,       *PINT;
typedef long long          LONGLONG,  *PLONGLONG;

//
// Fixed-sized types
//

typedef UCHAR     UINT8,  *PUINT8;
typedef USHORT    UINT16, *PUINT16;
typedef UINT      UINT32, *PUINT32;
typedef ULONGLONG UINT64, *PUINT64;
typedef CHAR      INT8,   *PINT8;
typedef SHORT     INT16,  *PINT16;
typedef INT       INT32,  *PINT32;
typedef LONGLONG  INT64,  *PINT64;

//
// Size types
//

typedef INT64  SSIZE, *PSSIZE;
typedef UINT64 SIZE,  *PSIZE;

//
// Pointer-sized types
//

typedef UINT64 UINTPTR, *PUINTPTR;
typedef INT64  INTPTR,  *PINTPTR;

//
// String types
//

typedef char       *PSTR;
typedef const char *PCSTR;

//
// Boolean
//

typedef UCHAR BOOLEAN, *PBOOLEAN;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//
// Void
//

typedef void        VOID;
typedef void       *PVOID;
typedef const void *PCVOID;

//
// NULL
//

#ifndef NULL
#define NULL ((void*)0)
#endif

//
// Definitions
//

#define ALIGN_UP(VALUE, ALIGN)   (((UINTPTR)(VALUE) + ((UINTPTR)(ALIGN) - 1)) & ~((UINTPTR)(ALIGN) - 1ULL))
#define ALIGN_DOWN(VALUE, ALIGN) ((UINTPTR)(VALUE) & ~((UINTPTR)(ALIGN) - 1ULL))

#define COUNTOF(ARRAY)           (sizeof(ARRAY) / sizeof((ARRAY)[0]))

#define KB(BYTE)                 ((SIZE)(BYTE) * 1024ULL)
#define MB(BYTE)                 (KB(BYTE) * 1024ULL)
#define GB(BYTE)                 (MB(BYTE) * 1024ULL)

#define OFFSETOF                 __builtin_offsetof

#define BITS_TO_BYTES(B)         (((B) + 7) >> 3)

//
// KResult
//

typedef UINT32 KRESULT;

#define KSUCCESS(RESULT) (!(RESULT & 0x80000000))
#define KERROR(RESULT)   (!(KSUCCESS(RESULT)))

#define KRESULT_SUCCESS           0x00000000
#define KRESULT_TRUE              0x00000001
#define KRESULT_FALSE             0x00000000

#define KRESULT_UNSUCCESSFUL      0x80000000
#define KRESULT_INVALID_PARAMETER 0x80000001
#define KRESULT_MEMORY_NOT_ENOUGH 0x80000002
#define KRESULT_OUT_OF_BOUND      0x80000003

//
// Attributes
//

#define NORETURN                  __attribute__((noreturn))
#define MAYBE_UNUSED              __attribute__((unused))
#define UNREFERENCED_PARAMETER(P) ((VOID)P)

#endif
