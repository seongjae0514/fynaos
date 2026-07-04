#include <fynaos/kernel.h>

#define IDT_COUNT 256

extern uintptr_t isr_vector_table[IDT_COUNT];

struct idt_entry idt[IDT_COUNT] = { 0 };

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
    init_idt();

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
}

__noreturn void dispatch_interrupt(struct trap_frame *frame)
{
    (void)frame;

    kprintf("int: %d\n", frame->vector);

    if (frame->vector == 3)
    {
        exit_interrupt(frame);
    }
    else if (frame->vector == 14)
    {
        void *p;

        asm volatile (
            "mov %%cr2, %0"
            :"=r"(p)
        );

        kprintf(
            "!!! PAGE FAULT !!!\n"
            "address=%016p\n"
            "errorCode=%u\n\n",
            p, frame->errcode
            );

        kernel_panic("interrupt not handled\n", PANIC_FLAG_SILENCE);
    }

    kernel_panic("interrupt: not implemented", 0);
}
