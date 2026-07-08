#include <fynaos/kernel.h>
#include <fynaos/kd.h>

#define IDT_COUNT 256

extern uintptr_t isr_vector_table[IDT_COUNT];

struct idt_entry idt[IDT_COUNT] = { 0 };

__noreturn void timer_interrupt(struct trap_frame *frame);

static void set_idt_entry(unsigned int index, uintptr_t routine, uint8_t attributes)
{
    struct idt_entry *entry = &idt[index];

    entry->attributes = attributes;
    entry->kernel_cs  = 0x08;
    entry->reserved   = 0;
    entry->isr_low    = (uint16_t)(routine & 0xFFFF);
    entry->isr_mid    = (uint16_t)((routine >> 16) & 0xFFFF);
    entry->isr_high   = (uint32_t)((routine >> 32) & 0xFFFFFFFF);
    entry->ist_offset = 0;
}

static void init_idt(void)
{
    struct idtr idtr;

    idtr.base  = (uintptr_t)idt;
    idtr.limit = (uint16_t)(sizeof(idt) - 1);

    asm volatile (
        "lidt %0"
        :: "m"(idtr)
    );
}

void init_interrupts(void)
{
    for (unsigned int i = 0; i < IDT_COUNT; i++)
    {
        set_idt_entry(
            i,
            isr_vector_table[i],
            IDT_ATTRIBUTE_PRESENT |
            IDT_ATTRIBUTE_PRIVILEGE_KERNELONLY |
            IDT_ATTRIBUTE_TYPE_INTERRUPT_GATE
            );
    }

    init_idt();
}

__noreturn void dispatch_interrupt(struct trap_frame *frame)
{
    switch (frame->vector)
    {
    case 3:
        exit_interrupt(frame);

    case 32:
        timer_interrupt(frame);

    default:
        kprintf("Unhandled interrupt occured. vector=%u errorCode=%u",
                frame->vector, frame->errcode);
        kernel_panic("Unhandled interrupt", PANIC_FLAG_SILENCE);
    }
}

__noreturn void timer_interrupt(struct trap_frame *frame)
{
    send_eoi(32);
    timer_tick++;
    sched_tick();
    exit_interrupt(frame);
}
