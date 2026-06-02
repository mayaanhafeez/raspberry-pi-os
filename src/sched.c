#include "sched.h"
#include "irq.h"
#include "printf.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
struct task_struct * task_list = &(init_task);
int nr_tasks=1;

void preempt_disable(void){
  current->preempt_count++;
}

void preempt_enable(void){
  current->preempt_count--;
}

void _schedule(void){
  preempt_disable();
  struct task_struct *next_task = task_list;
  int c;
  struct task_struct * p;
  while (1) {
    c=-1;
    p= task_list;
    while (p) {
      if (p->state == TASK_RUNNING && p->counter >c) {
        c=p->counter;
        next_task=p;
      }
      p=p->next;
    }
    if (c) {
      break;
    }
    p=task_list;
    while (p) {
      p->counter = (p->counter >>1 ) + p->priority;
      p=p->next;
    }
  }
  switch_to(next_task);
  preempt_enable();
}

void schedule(void){
  current->counter = 0;
  _schedule();
}

void switch_to(struct task_struct * next){
  if (current==next)
    return;
  struct task_struct * prev = current;
  current = next;
  cpu_switch_to(prev,next);
}

void schedule_tail(void){
  preempt_enable();
}

void timer_tick(){
  --current->counter;
  if (current->counter>0 || current -> preempt_count >0) {
    return;
  }
  current->counter =0;
  enable_irq();
  _schedule();
  disable_irq();
}
