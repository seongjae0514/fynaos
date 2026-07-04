#include <fynaos/types.h>
#include <fynaos/cpu.h>

#define COM1_DATA             (uint16_t)0x3F8
#define COM1_INTERRUPT_ENABLE (uint16_t)0x3F9
#define COM1_FIFO_CONTROL     (uint16_t)0x3FA
#define COM1_LINE_CONTROL     (uint16_t)0x3FB
#define COM1_MODEM_CONTROL    (uint16_t)0x3FC
#define COM1_LINE_STATUS      (uint16_t)0x3FD
#define COM1_MODEM_STATUS     (uint16_t)0x3FE

void init_serial(void)
{
    outb(COM1_INTERRUPT_ENABLE, (uint8_t)0);
    outb(COM1_LINE_CONTROL,     (uint8_t)0x80);
    outb(COM1_DATA,             (uint8_t)0x01);
    outb(COM1_INTERRUPT_ENABLE, (uint8_t)0x00);
    outb(COM1_LINE_CONTROL,     (uint8_t)0x03);
    outb(COM1_FIFO_CONTROL,     (uint8_t)0xC7);
    outb(COM1_MODEM_CONTROL,    (uint8_t)0x0B);
}

void write_serial(char ch)
{
    while ((inb(COM1_LINE_STATUS) & (uint8_t)0x20) == 0);
    outb(COM1_DATA, (uint8_t)ch);
}
