#include "entry.h"
#include "mm.h"
#include "printf.h"
#include "sched.h"
#include "utils.h"
#include "fork.h"

int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long stack) {
  preempt_disable();
  struct task_struct *p;
  p = (struct task_struct *) get_free_page();
  if (!p) {return -1;};

  struct pt_regs *childregs = task_pt_regs(p);
  memzero((unsigned long)childregs, sizeof(struct pt_regs));
  memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));

  if (clone_flags & PF_KTHREAD) {
    p->cpu_context.x19 = fn;
    p->cpu_context.x20 = arg;
  } else {
    struct pt_regs * cur_regs = task_pt_regs(current);
    *childregs = *cur_regs;
    childregs->regs[0] = 0;
    childregs->sp = stack + PAGE_SIZE;
    p->stack = stack;
  }

  p->flags = clone_flags;
  p->priority = current->priority;
  p->state = TASK_RUNNING;
  p->counter = current->priority;
  p->preempt_count = 1;

  p->cpu_context.pc = (unsigned long)ret_from_fork;
  p->cpu_context.sp = (unsigned long)childregs;
  int pid = nr_tasks++;
  task[pid] = p;
  preempt_enable();
  return pid;
}

int move_to_user_mode(unsigned long start, unsigned long size, unsigned long pc) {
  struct pt_regs *regs = task_pt_regs(current);
  regs->pstate = PSR_MODE_EL0t;
  regs->pc = pc;
  regs->sp = 2*PAGE_SIZE;
  unsigned long code_page = allocate_user_page(current, 0);
  if (code_page==0) {return -1;}
  memcpy(code_page, start, size);
  set_pgd(current->mm.pgd);
  return 0;
}

struct pt_regs * task_pt_regs(struct task_struct *tsk){
  unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
  return (struct pt_regs *)p;
}
