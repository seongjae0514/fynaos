#include <fynaos/kernel.h>
#include <fynaos/kd.h>

#define CALL_STACK_SIZE 62

const char *call_stack[CALL_STACK_SIZE] = { NULL };
int         top                         = -1;
boolean_t   call_stack_overflowed       = FALSE;

void kd_assert(boolean_t cond, const char *exp,
               const char *msg,
               const char *file, int line, const char *function)
{
    if (cond) return;

    kprintf(
        "!!! DEBUG ASSERTION FAILED !!!\n"
        "\n"
        "%s\n"
        "\n"
        "expression: %s\n"
        "file:       %s\n"
        "line:       %d\n"
        "function:   %s\n"
        "\n",
         msg, exp, file, line, function
        );

    kprintf("Calling stack:\n");
    kd_print_callstack();
    kprintf("\n");

    kernel_panic("kd: debug assertion failed", PANIC_FLAG_SILENCE);
}

void kd_enterproc(const char *fn)
{
    DPRINT("Entering procedure: %s\n", fn);

    if (top < CALL_STACK_SIZE)
    {
        top++;
        call_stack[top] = fn;
    }
    else
    {
        call_stack_overflowed = TRUE;
    }
}

void kd_leaveproc(void)
{
    DPRINT("Leaving procedure: %s\n", call_stack[top]);

    if (top < 0)
    {
        kernel_panic("kd: call stack underflow", 0);
    }

    top--;
}

void kd_print_callstack(void)
{
    kprintf("\nwarning: callstack have been overflowed.\n\n");

    for (int i = 0; i <= top; i++)
    {
        kprintf(
            "%d: %s\n",
            i,
            call_stack[i]
            );
    }
}
