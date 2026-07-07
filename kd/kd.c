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

    kernel_panic("kd: debug assertion failed", PANIC_FLAG_SILENCE);
}
