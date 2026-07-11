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

#define SCHED_COUNTER_BASE 10
#define DEFAULT_STACK_SIZE 4096

struct task *current_task;
struct task *ready_tasks;
struct task *stopped_tasks;
struct task *pending_tasks;
struct task *sleeping_tasks;
struct task *idle_task;

extern struct context *switch_context(struct context *prev, struct context *next);

static void remove_task_from_list(struct task **list, struct task *task)
{
    struct task *cur  = *list;
    struct task *prev = NULL;

    while (cur)
    {
        if (cur == task) break;
        prev = cur;
        cur  = cur->next;
    }

    ASSERT(cur != NULL, "Cannot find the entry in the list.");

    if (prev) prev->next = cur->next;
    else      *list      = cur->next;

    task->next = NULL;
}

static void insert_task_to_list(struct task **list, struct task *task)
{
    task->next = *list;
    *list = task;
}

/*
 * Switches current task to <task>.
 * Returns task that switched to us.
 * 
 * Before this function, the interrupts are must be disabled.
 */
struct task *switch_to(struct task *next)
{
    INTERRUPT_DISABLED_ASSERT("Interrupts are must be disabled");
    ASSERT(current_task != NULL, "current_task is NULL");
    if (!current_task) kernel_panic("current_task is NULL.", 0);

    struct task    *cur      = current_task;
    struct context *last_ctx = NULL;

    if (cur == next) return next;

    if (cur->state == TASK_RUNNING) cur->state = TASK_READY;
    next->state = TASK_RUNNING;
    swap_rsp0((uint64_t)next->kernel_stack + DEFAULT_STACK_SIZE);

    current_task = next;

    swap_mm(current_task->mm);

    last_ctx = switch_context(&cur->context, &next->context);

    /*
     * So, you are back!
     *
     * The task that switched to us has already updated the scheduler
     * state, so simply return the previous task.
     */

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

    /* Remove current task from ready list */

    remove_task_from_list(&ready_tasks, current_task);

    /* Wake tasks wating for me up */

    struct task *cur = current_task->waiting_for_me;
    
    while (cur)
    {
        struct task *next = cur->next_waiting;
        ready_task(cur);
        cur = next;
    }

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
    
    remove_task_from_list(list, task);
    insert_task_to_list(&ready_tasks, task);

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

struct task *create_kernel_task(void (*fn)(void) __noreturn, uintptr_t priority)
{
    struct task *task;
    phys_addr_t  stack_page;

    if (!(task = kmalloc(sizeof(*task))))
    {
        return NULL;
    }

    if ((stack_page = alloc_frames(0)) == INVALID_PHYSICAL_ADDRESS)
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
    task->priority      = priority;

    /* Initialize task context */

    struct context *ctx = &task->context;

    ctx->rsp = (uintptr_t)task->kernel_stack + DEFAULT_STACK_SIZE - 16;
    *(uint64_t*)ctx->rsp       = (uint64_t)init_task;
    *((uint64_t*)ctx->rsp + 1) = (uint64_t)fn;

    /* Insert task into stopped tasks list */

    unsigned long flags = save_and_disable_interrupts();
    insert_task_to_list(&stopped_tasks, task);
    restore_interrupts(flags);

    return task;
}

static void __reset_task_counters(void)
{
    struct task *cur = ready_tasks;

    while (cur)
    {
        cur->sched_counter = cur->priority + SCHED_COUNTER_BASE;
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
        switch_to(idle_task);
        restore_interrupts(flags);
        return;
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

    idle->priority      = 0;
    idle->flags         = TASK_KERNEL | TASK_CRITICAL;
    idle->kernel_stack  = &kernel_stack;
    idle->sched_counter = SCHED_COUNTER_BASE;
    idle->state         = TASK_RUNNING;
    
    idle_task = idle;

    current_task = idle;

    return TRUE;
}

void sched_tick(void)
{
    /* Wake sleeping tasks */

    struct task *prv = NULL, *cur = sleeping_tasks, *next;

    while (cur)
    {
        next = cur->next;
        if (cur->wakeup_tick <= timer_tick)
        {
            if (!prv) sleeping_tasks = cur->next;
            else      prv->next = cur->next;

            cur->state = TASK_READY;

            cur->next   = ready_tasks;
            ready_tasks = cur;
        }
        else
        {
            prv = cur;
        }
        cur = next;
    }

    /* Scheduling */

    if (current_task->sched_counter > 0) current_task->sched_counter--;
    if (current_task->sched_counter == 0) schedule();
}

void sleep_task(unsigned long ms)
{
    unsigned long flags = save_and_disable_interrupts();

    current_task->wakeup_tick = timer_tick + (ms + 9) / 10;
    current_task->state = TASK_SLEEPING;

    remove_task_from_list(&ready_tasks, current_task);
    insert_task_to_list(&sleeping_tasks, current_task);

    schedule();

    restore_interrupts(flags);
}

int wait_for_task(struct task *task)
{
    unsigned long flags = save_and_disable_interrupts();

    current_task->next_waiting = task->waiting_for_me;
    task->waiting_for_me = current_task;

    remove_task_from_list(&ready_tasks, current_task);

    current_task->state = TASK_PENDING;
    insert_task_to_list(&pending_tasks, current_task);

    schedule();

    restore_interrupts(flags);
    return task->exit_code;
}
