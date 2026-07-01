#include <fxos/types.h>
#include <fxos/rtl/string.h>
#include <fxos/rtl/printf.h>
#include <stdarg.h>

#define IS_DIGIT(C)             ((C) >= '0' && (C) <= '9')
#define IS_DIGIT_EXCEPT_ZERO(C) ((C) >  '0' && (C) <= '9')

typedef struct _FORMAT_INFO {
    UINT8  Flags;
    INT8   FormatType;
    UINT16 PaddingLength;
} FORMAT_INFO, *PFORMAT_INFO;

#define FORMAT_INFO_FLAG_ZEROPAD   0x01
#define FORMAT_INFO_FLAG_LEFTALIGN 0x04
#define FORMAT_INFO_FLAG_LONG      0x08
#define FORMAT_INFO_FLAG_LONGLONG  0x18

PCSTR
RtlpParseSingleFormat(
    PCSTR        Format,
    PFORMAT_INFO FormatInfo
    )
{
    RtlZeroMemory(FormatInfo, sizeof(*FormatInfo));

    Format++;

    while (*Format && !IS_DIGIT_EXCEPT_ZERO(*Format))
    {
        if (*Format == '-')
        {
            FormatInfo->Flags |= FORMAT_INFO_FLAG_LEFTALIGN;
        }
        else if (*Format == '0')
        {
            FormatInfo->Flags |= FORMAT_INFO_FLAG_ZEROPAD;
        }

        Format++;
    }

    while (*Format && IS_DIGIT(*Format))
    {
        FormatInfo->PaddingLength = (UINT16)(FormatInfo->PaddingLength * 10) + (UINT16)(*Format - '0');
        Format++;
    }

    if (*Format == 'l')
    {
        FormatInfo->Flags |= FORMAT_INFO_FLAG_LONG;
        Format++;
    }
    if (*Format == 'l')
    {
        FormatInfo->Flags |= FORMAT_INFO_FLAG_LONGLONG;
        Format++;
    }

    FormatInfo->FormatType = *Format;

    return Format + 1;
}

PSTR
RtlpPrintSingleFormat(
    PSTR         Buffer,
    PCSTR        Limit,
    PFORMAT_INFO Info,
    va_list      Arg
    )
{
    CHAR Temp[512];

    switch (Info->FormatType)
    {
    case 'd':
        if (Info->Flags & FORMAT_INFO_FLAG_LONGLONG)
        {
            RtlIntegerToString64(
                (INT64)va_arg(Arg, long long),
                Temp,
                'i'
                );
        }
        else if (Info->Flags & FORMAT_INFO_FLAG_LONG)
        {
            RtlIntegerToString64(
                (INT64)va_arg(Arg, long),
                Temp,
                'i'
                );
        }
        else
        {
            RtlIntegerToString64(
                (INT64)va_arg(Arg, int),
                Temp,
                'i'
                );
        }
        break;
    
    case 'u':
        if (Info->Flags & FORMAT_INFO_FLAG_LONGLONG)
        {
            RtlUnsignedIntegerToString64(
                (UINT64)va_arg(Arg, unsigned long long),
                Temp,
                'i'
                );
        }
        else if (Info->Flags & FORMAT_INFO_FLAG_LONG)
        {
            RtlUnsignedIntegerToString64(
                (UINT64)va_arg(Arg, unsigned long),
                Temp,
                'i'
                );
        }
        else
        {
            RtlUnsignedIntegerToString64(
                (INT64)va_arg(Arg, unsigned int),
                Temp,
                'i'
                );
        }
        break;

    case 'x':
        if (Info->Flags & FORMAT_INFO_FLAG_LONGLONG)
        {
            RtlUnsignedIntegerToString64(
                (UINT64)va_arg(Arg, unsigned long long),
                Temp,
                'x'
                );
        }
        else if (Info->Flags & FORMAT_INFO_FLAG_LONG)
        {
            RtlUnsignedIntegerToString64(
                (UINT64)va_arg(Arg, unsigned long),
                Temp,
                'x'
                );
        }
        else
        {
            RtlUnsignedIntegerToString64(
                (INT64)va_arg(Arg, unsigned int),
                Temp,
                'x'
                );
        }
        break;
    
    case 'X':
        if (Info->Flags & FORMAT_INFO_FLAG_LONGLONG)
        {
            RtlUnsignedIntegerToString64(
                (UINT64)va_arg(Arg, unsigned long long),
                Temp,
                'X'
                );
        }
        else if (Info->Flags & FORMAT_INFO_FLAG_LONG)
        {
            RtlUnsignedIntegerToString64(
                (UINT64)va_arg(Arg, unsigned long),
                Temp,
                'X'
                );
        }
        else
        {
            RtlUnsignedIntegerToString64(
                (INT64)va_arg(Arg, unsigned int),
                Temp,
                'X'
                );
        }
        break;

    case 's':
        RtlCopyString(Temp, va_arg(Arg, PCSTR));
        break;

    case 'c':
        Temp[0] = (CHAR)va_arg(Arg, INT);
        Temp[1] = '\0';
        break;

    default:
        Temp[0] = '%';
        Temp[1] = Info->FormatType;
        Temp[2] = '\0';
    }

    SIZE Length = RtlGetStringLength(Temp);
    SIZE Padding;
    
    if (Length > Info->PaddingLength)
    {
        Padding = 0;
    }
    else
    {
        Padding = Info->PaddingLength - Length;
    }

    if (Info->Flags & FORMAT_INFO_FLAG_LEFTALIGN)
    {
        PCSTR p = Temp;

        while (*p)
        {
            *Buffer++ = *p++;

            if (Buffer >= Limit)
            {
                return NULL;
            }
        }
        
        while (Padding--)
        {
            *Buffer++ = ' ';

            if (Buffer >= Limit)
            {
                return NULL;
            }
        }
    }
    else
    {
        PCSTR p = Temp;

        while (Padding--)
        {
            *Buffer++ = Info->Flags & FORMAT_INFO_FLAG_ZEROPAD ? '0' : ' ';

            if (Buffer >= Limit)
            {
                return NULL;
            }
        }

        while (*p)
        {
            *Buffer++ = *p++;

            if (Buffer >= Limit)
            {
                return NULL;
            }
        }
    }
    
    return Buffer;
}

PSTR
RtlVStringPrintf(
    PSTR    Buffer,
    SIZE    Length,
    PCSTR   Format,
    va_list Args
    )
{
    PCSTR Limit = Buffer + Length;

    while (Buffer < Limit && *Format)
    {
        if (*Format == '%')
        {
            FORMAT_INFO Info;

            Format = RtlpParseSingleFormat(
                Format,
                &Info
                );

            Buffer = RtlpPrintSingleFormat(
                Buffer,
                Limit,
                &Info,
                Args
                );

            if (!Buffer)
            {
                return NULL;
            }
        }
        else
        {
            *Buffer++ = *Format++;
        }
    }

    *Buffer = '\0';

    return Buffer;
}

PSTR
RtlStringPrintf(
    PSTR  Buffer,
    SIZE  Length,
    PCSTR Format,
    ...
    )
{
    va_list Args;
    va_start(Args, Format);

    PSTR s = RtlVStringPrintf(Buffer, Length, Format, Args);

    va_end(Args);
    return s;
}
