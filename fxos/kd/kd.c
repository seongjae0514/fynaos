#include <fxos/types.h>
#include <fxos/hal/serial.h>
#include <fxos/rtl/printf.h>
#include <fxos/ke/panic.h>

#include <stdarg.h>

static
VOID
KdpPrintSerialString(
    PCSTR String
    )
{
    while (*String)
    {
        HalPutCharacterToSerial(*String++);
    }
}

KRESULT
KdPrintSerial(
    PCSTR Format,
    ...
    )
{
    va_list Args; 
    CHAR    Buffer[512];
    PSTR    Result;

    va_start(Args, Format);

    Result = RtlVStringPrintf(
        Buffer,
        sizeof(Buffer),
        Format,
        Args
        );

    va_end(Args);

    if (!Result)
    {
        KdpPrintSerialString("[...]");
        KdpPrintSerialString(Format);
        return KRESULT_OUT_OF_BOUND;
    }

    KdpPrintSerialString(Buffer);

    return KRESULT_SUCCESS;
}

VOID
KdAssert(
    BOOLEAN Condition,
    PCSTR   Expression,
    PCSTR   Message,
    PCSTR   File,
    UINT    Line,
    PCSTR   Function
    )
{
    if (Condition) return;

    KdPrintSerial(
        "!!! DEBUG ASSERTION FAILED !!!\n"
        "\n"
        "%s\n"
        "\n"
        "Expresstion: %s\n"
        "File:        %s\n"
        "Line:        %d\n"
        "Function:    %s\n"
        "\n",
        Message,
        Expression,
        File,
        Line,
        Function
        );

    KeKernelPanic("Assertion failed");
}
