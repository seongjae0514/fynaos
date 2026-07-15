/*
 * fynaos/kernel/wait.c 
 * Implementation for `struct waitable_header`
 */

#include <fynaos/cpu.h>
#include <fynaos/kd.h>
#include <fynaos/kernel.h>

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

void wait_for_waitable_header(struct waitable_header *header)
{
    unsigned long flags = save_and_disable_interrupts();

    current_task->state = TASK_PENDING;
    remove_task_from_list(&ready_tasks, current_task);
    insert_task_to_list(&pending_tasks, current_task);
    
    current_task->next_waiting = header->waiting;
    header->waiting = current_task;

    schedule();

    restore_interrupts(flags);
}

void wake_waiting_tasks(struct waitable_header *header)
{
    struct task *cur = header->waiting;
    
    while (cur)
    {
        ready_task(cur);
        cur = cur->next_waiting;
    }
}
