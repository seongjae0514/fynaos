/*
 * fynaos/kernel/sched.c
 *
 * FYNAOS Kernel Scheduler
 */

#include <fynaos/kernel.h>
#include <fynaos/mm.h>
#include <fynaos/kd.h>
#include <fynaos/string.h>
#include <fynaos/cpu.h>

#define SCHED_COUNTER_BASE 1000000
#define DEFAULT_STACK_SIZE 4096

/*
 * Interrupt support is not available in this version.
 */

#define enable_interrupts()
#define disable_interrupts()

struct task *current_task;
struct task *ready_tasks;
struct task *stopped_tasks;
struct task *pending_tasks;

extern struct context *switch_context(struct context *prev, struct context *next);

/*
 * Switches current task to <task>.
 * Returns task that switched current context to this context.
 * 
 * Before this function, the interrupts are must be disabled.
 */
struct task *switch_to(struct task *task)
{
    INTERRUPT_DISABLED_ASSERT("Interrupts are must be disabled");
    ASSERT(current_task != NULL, "current_task is NULL");
    if (!current_task) kernel_panic("current_task is NULL.", 0);

    if (task == current_task)
    {
        return task;
    }

    struct task    *prev = current_task;
    struct context *last_ctx;

    current_task->state = TASK_READY;
    current_task = task;

    last_ctx = switch_context(&prev->context, &task->context);

    current_task = prev;
    current_task->state = TASK_RUNNING;
    return _container_of(last_ctx, struct task, context);
}

__noreturn void exit_task(int code)
{
    disable_interrupts();

    /* Check if current task is critical task */

    if (current_task->flags & TASK_CRITICAL)
    {
        kernel_panic("Critical process tried to exit", 0);
    }

    /* Mark terminated */

    current_task->state = TASK_TERMINATED;
    current_task->exit_code = code;

    /* Remove current task from list */

    struct task *cur  = ready_tasks;
    struct task *prev = NULL;

    while (cur)
    {
        if (cur == current_task) break;
        prev = cur;
        cur = cur->next;
    }

    if (prev) prev->next  = cur->next;
    else      ready_tasks = cur->next;

    /* Reschedule */

    schedule();
    __builtin_unreachable();
}

boolean_t ready_task(struct task *task)
{
    /* Remove the task from list */

    struct task **list;

    switch (task->state)
    {
    case TASK_STOPPED:
        list = &stopped_tasks;
        break;

    case TASK_PENDING:
        list = &pending_tasks;
        break;
    
    default:
        return FALSE;

    }

    unsigned long flags = save_and_disable_interrupts();
    struct task *cur  = *list;
    struct task *prev = NULL;

    while (cur)
    {
        if (cur == task) break;

        prev = cur;
        cur  = cur->next;
    }

    if (prev)  prev->next = cur->next;
    else      *list       = cur->next;

    /* Insert task into the ready list */

    task->next = ready_tasks;
    ready_tasks = task;

    /* Set task state */

    task->state = TASK_READY;

    restore_interrupts(flags);
    return TRUE;
}

/*
 * This function initializes current task
 * in its context.
 */
static void init_task(void)
{
    DPRINT("A task birthed!\n");
    enable_interrupts();
}

struct task *create_kernel_task(void (*fn)(void) __noreturn)
{
    struct task *task;
    phys_addr_t  stack_page;

    if (!(task = kmalloc(sizeof(*task))))
    {
        return NULL;
    }

    if ((stack_page = alloc_page()) == INVALID_PHYSICAL_ADDRESS)
    {
        kfree(task);
        return NULL;
    }

    memset(task, 0, sizeof(*task));

    /* Initialize task states */

    task->flags         = TASK_KERNEL;
    task->state         = TASK_STOPPED;
    task->sched_counter = SCHED_COUNTER_BASE;
    task->kernel_stack  = phys_to_virt(stack_page);
    task->user_stack    = NULL;
    task->mm            = NULL;

    /* Initialize task context */

    struct context *ctx = &task->context;

    ctx->rsp = (uintptr_t)task->kernel_stack + DEFAULT_STACK_SIZE - 16;
    *(uint64_t*)ctx->rsp       = (uint64_t)init_task;
    *((uint64_t*)ctx->rsp + 1) = (uint64_t)fn;

    /* Insert task into stopped tasks list */

    unsigned long flags = save_and_disable_interrupts();
    task->next    = stopped_tasks;
    stopped_tasks = task;
    restore_interrupts(flags);

    return task;
}

static void __reset_task_counters(void)
{
    struct task *cur = ready_tasks;

    while (cur)
    {
        cur->sched_counter = SCHED_COUNTER_BASE;
        cur = cur->next;
    }
}

void reset_task_counters(void)
{
    unsigned long flags = save_and_disable_interrupts();
    __reset_task_counters();
    restore_interrupts(flags);
}

void schedule(void)
{
    unsigned long  flags   = save_and_disable_interrupts();
retry:
    struct task   *cur     = ready_tasks;
    struct task   *next    = NULL;
    unsigned long  counter = 0;

    if (!ready_tasks)
    {
        kernel_panic("Idle task is missing.", 0);
    }

    while (cur)
    {
        if (cur->sched_counter > counter)
        {
            counter = cur->sched_counter;
            next = cur;
        }
        cur = cur->next;
    }

    if (counter == 0)
    {
        __reset_task_counters();
        goto retry;
    }

    if (next != current_task)
    {
        switch_to(next);
    }

    restore_interrupts(flags);
}

void delete_task(struct task *task)
{
    /* Not implemented */
    (void)task;
    return;
}

extern uint8_t kernel_stack;

boolean_t init_sched(void)
{
    /* Create idle task */

    struct task *idle = kmalloc(sizeof(*idle));

    if (!idle)
    {
        return FALSE;
    }

    memset(idle, 0, sizeof(*idle));

    idle->flags         = TASK_KERNEL | TASK_CRITICAL;
    idle->kernel_stack  = &kernel_stack;
    idle->sched_counter = SCHED_COUNTER_BASE;
    idle->state         = TASK_RUNNING;
    
    ready_tasks = idle;

    current_task = idle;

    return TRUE;
}

/*
 * This function is for test.
 * May be deleted, or may survibe with timer interrupt.
 */
void ticking()
{
    current_task->sched_counter--;
}
