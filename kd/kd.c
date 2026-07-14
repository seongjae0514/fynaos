#include <fynaos/kernel.h>
#include <fynaos/kd.h>

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
