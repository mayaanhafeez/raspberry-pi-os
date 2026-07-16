#include <stddef.h>
#include <stdint.h>
#include "printf.h"
#include "utils.h"
#include "timer.h"
#include "irq.h"
#include "fork.h"
#include "sched.h"
#include "mini_uart.h"
#include "sys.h"
#include "user.h"


void user_process1(char* array) {
  char buf[2] = {0};
  while (1) {
    for (int i=0; i<5; i++) {
      buf[0] = array [i];
      call_sys_write(buf);
      delay(10000);
    }
  }
}

void user_process() {
  char buf[30] = {0};
  tfp_sprintf(buf, "User process started\n\r");
  call_sys_write(buf);
  unsigned long stack = call_sys_malloc();
  if (stack < 0) {
    printf("Error while allocating stack for process 1\n\r");
    return;
  }

  int err = call_sys_clone((unsigned long)&user_process1, (unsigned long)"12345", stack);
  if (err < 0) {
    printf("Error while cloning process 1\n\r");
    return;
  }

  stack= call_sys_malloc();
  if (stack <0) {
    printf("Error while allocating stack for process 1\n\r");
    return;
  }

  err = call_sys_clone((unsigned long)&user_process1, (unsigned long)"abcde", stack);
  if (err < 0) {
    printf("Error while cloning process 1\n\r");
    return;
  }
  call_sys_exit();
}

void kernel_process(){
  printf("Kernel proess started. EL %d\r\n",get_el());
  unsigned long begin = (unsigned long)&user_begin;
  unsigned long end = (unsigned long)&user_end;
  unsigned long process = (unsigned long)&user_process;
  int err = move_to_user_mode(begin, end-begin, process-begin);
  if (err<0){
    printf("Error while moving to user mode\n\r");
  }
}

void kernel_main(void) {
  current->priority=1;
  uart_init();
  init_printf(0, putc);
  irq_vector_init();
  timer_init();
  enable_interrupt_controller();
  enable_irq();
  int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0, 0);
  if (res < 0){
    printf("error while starting process");
    return;
  }

  while (1) {
    schedule();
  }
}
