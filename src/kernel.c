#include "mini_uart.h" 
#include "utils.h"
#include "printf.h"

void kernel_main(void){
  uart_init();
  init_printf(0, putc);
  int el= get_el();
  printf("Exception Level: %d \r\n",el);

  while (1){
      uart_send(uart_recv());
    }
}
