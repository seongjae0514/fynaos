#include <fynaos/kernel.h>
#include <fynaos/kd.h>
#include <fynaos/cpu.h>

__noreturn void kernel_panic(const char *msg, unsigned int flags)
{
    disable_interrupts();

    if (!(flags & PANIC_FLAG_SILENCE))
    {
        kprintf(
            "!!! KERNEL PANIC !!!\n"
            "\n"
            "%s\n"
            "\n"
            "system halted.\n",
            msg
            );
    }
    else
    {   
        kprintf("kernel panic: %s\n", msg);
    }

    halt_cpu_forever();
}
