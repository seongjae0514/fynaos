#include <fxos/types.h>
#include <fxos/hal/port.h>
#include <fxos/hal/serial.h>

#define COM1_DATA             0x3F8
#define COM1_INTERRUPT_ENABLE 0x3F9
#define COM1_FIFO_CONTROL     0x3FA
#define COM1_LINE_CONTROL     0x3FB
#define COM1_MODEM_CONTROL    0x3FC
#define COM1_LINE_STATUS      0x3FD
#define COM1_MODEM_STATUS     0x3FE

KRESULT
HalInitializeSerial(
    VOID
    )
{
    //
    // Initialize serial port
    // 115200, 8N1, FIFO Enabled
    //

    HalWritePort8(COM1_INTERRUPT_ENABLE, 0);
    HalWritePort8(COM1_LINE_CONTROL,     0x80);
    HalWritePort8(COM1_DATA,             0x01);
    HalWritePort8(COM1_INTERRUPT_ENABLE, 0x00);
    HalWritePort8(COM1_LINE_CONTROL,     0x03);
    HalWritePort8(COM1_FIFO_CONTROL,     0xC7);
    HalWritePort8(COM1_MODEM_CONTROL,    0x0B);

    return KRESULT_SUCCESS;
}

KRESULT
HalPutCharacterToSerial(
    CHAR Character
    )
{
    while ((HalReadPort8(COM1_LINE_STATUS) & 0x20) == 0);
    HalWritePort8(COM1_DATA, (UINT8)Character);

    return KRESULT_SUCCESS;
}
