/*
 * fynaos/kernel/timer.h
 * 
 * Programmable Interval Timer management code
 */

#include <fynaos/kernel.h>
#include <fynaos/cpu.h>

#define FREQ 1193181U

void init_pit(uint16_t freq)
{
    uint16_t divisor = (uint16_t)FREQ / freq;

    // Channel 0
    outb(0x43, 0x36);

    // Low byte
    outb(0x40, divisor & 0xFFU);

    // High byte
    outb(0x40, (divisor >> 8) & 0xFFU);
}
