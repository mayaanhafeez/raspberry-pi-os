#include "mini_uart.h" 
#include "utils.h"
#include "printf.h"
#include "timer.h"
#include "irq.h"

void kernel_main(void){
  uart_init();
  init_printf(0, putc);
  irq_vector_init();
  timer_init();
  enable_interrupt_controller();
  enable_irq();
  int el= get_el();
  printf("Exception Level: %d \r\n",el);

  while (1){
      uart_send(uart_recv());
    }
}
