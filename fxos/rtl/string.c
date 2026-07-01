#include <fxos/rtl/string.h>

static
const
CHAR
RtlpHexadecimalString[][16] = {
    { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' },
    { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' }
};

PSTR
RtlpUnsignedIntegerToReversedString64(
    UINT64 Value,
    PSTR   Buffer,
    CHAR   Radix
    );

VOID
RtlCopyMemory(
    PSTR  Destination,
    PCSTR Source,
    SIZE  Length
    )
{
    while (Length--)
    {
        *Destination++ = *Source++;
    }
}

VOID
RtlMoveMemory(
    PSTR  Destination,
    PCSTR Source,
    SIZE  Length
    )
{
    if (Destination > Source)
    {
        while (Length)
        {
            Destination[Length - 1] = Source[Length - 1];
            Length--;
        }
    }
    else
    {
        RtlCopyMemory(Destination, Source, Length);
    }
}

VOID
RtlSetMemory(
    PSTR Destination,
    CHAR Value,
    SIZE Length
    )
{
    while (Length--)
    {
        *Destination++ = Value;
    }
}

SIZE
RtlGetStringLength(
    PCSTR String
    )
{
    PCSTR Ptr = String;
    while (*Ptr) Ptr++;
    return (SIZE)(Ptr - String);
}

VOID
RtlCopyString(
    PSTR  Destination,
    PCSTR Source
    )
{
    while (*Source)
    {   
        *Destination++ = *Source++;
    }
    *Destination = '\0';
}

PSTR
RtlpUnsignedIntegerToReversedString64(
    UINT64 Value,
    PSTR   Buffer,
    CHAR   Radix
    )
{
    if (Value == 0)
    {
        Buffer[0] = '0';
        return Buffer + 1;
    }

    UINT    RadixDivisor = 10;
    BOOLEAN UpperHex     = FALSE;
    PSTR    Tail         = Buffer;

    switch (Radix)
    {
    case 'b':
        RadixDivisor = 2;
        break;
    
    case 'i':
        RadixDivisor = 10;
        break;
    
    case 'x':
        RadixDivisor = 16;
        UpperHex     = FALSE;
        break;

    case 'X':
        RadixDivisor = 16;
        UpperHex     = TRUE;
        break;

    default:
        return NULL;
    }

    while (Value != 0)
    {
        *Tail = RtlpHexadecimalString[(INT)UpperHex][Value % RadixDivisor];
        Value /= RadixDivisor;
        Tail++;
    }

    return Tail;
}


KRESULT
RtlUnsignedIntegerToString64(
    UINT64 Value,
    PSTR   Buffer,
    CHAR   Radix
    )
{
    PSTR Head = Buffer;
    PSTR Tail = RtlpUnsignedIntegerToReversedString64(
        Value,
        Buffer,
        Radix
        );

    if (!Tail)
    {
        return KRESULT_INVALID_PARAMETER;
    }

    *Tail-- = '\0';

    while (Tail > Head)
    {
        CHAR Temp = *Tail;
        *Tail = *Head;
        *Head = Temp;
        Tail--;
        Head++;
    }

    return KRESULT_SUCCESS;
}

KRESULT
RtlIntegerToString64(
    INT64 Value,
    PSTR  Buffer,
    CHAR  Radix
    )
{
    UINT64  UnsignedValue;
    BOOLEAN Negative;

    if (Value < 0)
    {
        UnsignedValue = (UINT64)(~(UINT64)Value + 1);
        Negative      = TRUE;
    }
    else
    {
        UnsignedValue = (UINT64)Value;
        Negative      = FALSE;
    }

    PSTR Head = Buffer;
    PSTR Tail = RtlpUnsignedIntegerToReversedString64(
        UnsignedValue,
        Buffer,
        Radix
        );

    if (!Tail)
    {
        return KRESULT_INVALID_PARAMETER;
    }

    if (Negative)
    {
        *Tail       = '-';
        *(Tail + 1) = '\0';
    }
    else
    {
        *Tail-- = '\0';
    }

    while (Tail > Head)
    {
        CHAR Temp = *Tail;
        *Tail = *Head;
        *Head = Temp;
        Tail--;
        Head++;
    }

    return KRESULT_SUCCESS;
}
