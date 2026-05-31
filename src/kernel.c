#include "irq.h"
#include "mini_uart.h"
#include "printf.h"
#include "timer.h"
#include "utils.h"
#include "sched.h"
#include "fork.h"

void process(char* array) {
  while (1){
    for(int i=0;i<5;i++){
      uart_send(array[i]);
      delay(100000);
    }
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
  int el = get_el();
  printf("Exception Level: %d \r\n", el);
  printf("current addr = %x, init priority = %d\r\n", current, current->priority);
  int res = copy_process((unsigned long)&process, (unsigned long)"12345");
  if (res !=0){
    printf("error while starting process 1");
    return;
  }
  res = copy_process((unsigned long)&process, (unsigned long)"abcde");
  if (res !=0){
    printf("error while starting process 2");
    return;
  }

  while (1) {
    schedule();
  }
}
