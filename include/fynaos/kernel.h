#ifndef _FYNAOS_KERNEL_H
#define _FYNAOS_KERNEL_H

#include <fynaos/types.h>

/*
 * Definitions
 */

struct trap_frame
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;

    uint64_t vector;
    uint64_t errcode;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

struct idt_entry
{
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t  ist_offset;
    uint8_t  attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed));

struct idtr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

#define IDT_ATTRIBUTE_PRESENT                0x80

#define IDT_ATTRIBUTE_PRIVILEGE_KERNELONLY   0x00
#define IDT_ATTRIBUTE_PRIVILEGE_USERCALLABLE 0x60

#define IDT_ATTRIBUTE_TYPE_INTERRUPT_GATE    0x0E
#define IDT_ATTRIBUTE_TYPE_TRAP_GATE         0x0F
#define IDT_ATTRIBUTE_TYPE_TASK_GATE         0x05

#define PANIC_FLAG_SILENCE 1

struct context
{
    uint64_t rsp;
    uint64_t rbx;
    uint64_t rbp;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
};

struct mm;

struct task
{
    unsigned int    id;
    unsigned int    state;
    unsigned int    flags;
    int             exit_code;
    uintptr_t       sched_counter;

    struct context  context;
    struct mm      *mm;
    void           *kernel_stack;
    void           *user_stack;

    struct task    *next;
};

#define TASK_TERMINATED 1
#define TASK_STOPPED    0
#define TASK_PENDING    3
#define TASK_READY      4
#define TASK_RUNNING    5

#define TASK_KERNEL     0x1
#define TASK_USER       0x2
#define TASK_CRITICAL   0x4

/*
 * Functions
 */

void init_serial(void);
void write_serial(char ch);
size_t kprintf(const char *fmt, ...);
void init_interrupts(void);
__noreturn void kernel_panic(const char *msg, unsigned int flags);
__noreturn void exit_interrupt(struct trap_frame *frame);
struct task *switch_to(struct task *task);
__noreturn void exit_task(int code);
boolean_t ready_task(struct task *task);
struct task *create_kernel_task(void ( *fn)(void) __noreturn);
void schedule(void);
void reset_task_counters(void);
void delete_task(struct task *task);
boolean_t init_sched(void);
void ticking();

#endif
