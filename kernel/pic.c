/*
 * fynaos/kernel/pic.c
 *
 * Programmable Interrupt Controller (legacy) management code
 */

#include <fynaos/kernel.h>
#include <fynaos/cpu.h>

#define PIC_MASTER_CMD 0x20
#define PIC_SLAVE_CMD  0xA0

#define PIC_MASTER_DAT 0x21
#define PIC_SLAVE_DAT  0xA1

void init_pic(void)
{
    /*
     * <ICW1>
     * Tell PIC, "Initialization starts. The next data is ICW2."
     */

    outb(PIC_MASTER_CMD, 0x11);
    outb(PIC_SLAVE_CMD,  0x11);

    /*
     * <ICW2>
     * Tell PIC interrupt vector number
     */

    outb(PIC_MASTER_DAT, 0x20);
    outb(PIC_SLAVE_DAT,  0x28);
    
    /*
     * <ICW3>
     * Tell PIC the connection information
     */
    
    outb(PIC_MASTER_DAT, 0x04);
    outb(PIC_SLAVE_DAT,  0x02);
    
    /*
     * <ICW4>
     * Tell PIC the operation mode
     */

    outb(PIC_MASTER_DAT, 0x01);
    outb(PIC_SLAVE_DAT,  0x01);

    /*
     * Mask all IRQs
     */

    outb(PIC_MASTER_DAT, 0xFF);
    outb(PIC_SLAVE_DAT,  0xFF);
}

void mask_pic_irq(uint8_t irq, boolean_t allow)
{
    uint16_t port;
    uint8_t  value;

    if (irq < 8)
    {
        port = PIC_MASTER_DAT;
    }
    else
    {
        port = PIC_SLAVE_DAT;
        irq -= 8;
    }

    value = inb(port);
    if (allow) value &= (uint8_t)(~(1U << irq));
    else       value |= (uint8_t)(1U << irq);
    outb(port, value);
}

void send_eoi(uint8_t irq)
{
    if (irq >= 8) outb(0xA0,0x20);
    outb(PIC_MASTER_CMD, 0x20);
}
