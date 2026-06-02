#include "mm.h"
#include "sched.h"
#include "entry.h"
#include "printf.h"
int copy_process(unsigned long fn, unsigned long arg) {
  preempt_disable();
  struct task_struct *p=(struct task_struct *)get_free_page();
  struct task_struct *t = task_list;
  while (t->next) { 
    t=t->next;
  }
  t->next=p;
  p->next=0;
  nr_tasks++;

  if (!p)
    return 1;
  p->priority = current->priority;
  p->state = TASK_RUNNING;
  p->counter=p->priority;
  p->preempt_count=1;

  p->cpu_context.x19 = fn;
  p->cpu_context.x20 = arg;
  p->cpu_context.pc= (unsigned long)ret_from_fork;
  p->cpu_context.sp=(unsigned long)p + THREAD_SIZE;
  preempt_enable();
  return 0;
}
