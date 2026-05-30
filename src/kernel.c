#include "mini_uart.h" 
#include "utils.h"
#include "printf.h"
#include "timer.h"
#include "irq.h"

void kernel_main(void){
  uart_init();
  init_printf(0, putc);
  printf("irq vector init\n");
  irq_vector_init();
  printf("timer init\n");
  timer_init();
  enable_interrupt_controller();
  printf("enable_irq\n");
  enable_irq();
  int el= get_el();
  printf("Exception Level: %d \r\n",el);

  while (1){
    }
}
