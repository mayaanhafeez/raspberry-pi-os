#include"utils.h"
#include "mini_uart.h"
#include "printf.h"
#include "timer.h"
#include "entry.h"
#include "peripherals/irq.h"

const char *entry_error_messages[] = {
  "SYNC_INVALID_EL1t",
  "IQR_INVALID_EL1t",
  "IQR_INVALID_EL1t",
  "ERROR_INVALID_EL1t",

  "SYNC_INVALID_EL1h",
  "IQR_INVALID_EL1h",
  "IQR_INVALID_EL1h",
  "ERROR_INVALID_EL1h",

  "SYNC_INVALID_EL0_64",
  "IQR_INVALID_EL0_64",
  "IQR_INVALID_EL0_64",
  "ERROR_INVALID_EL0_64",

  "SYNC_INVALID_EL0_32",
  "IQR_INVALID_EL0_32",
  "IQR_INVALID_EL0_32",
  "ERROR_INVALID_EL0_32",
};


void enable_interrupt_controller() {
  put32(ENABLE_IRQS_1, 1 << 29);
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address) { 
  printf("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
}

void handle_irq(void) {
  unsigned int irq = get32(IRQ_PENDING_1);
  switch (irq){
    case (SYSTEM_TIMER_IRQ_1):
      handle_timer_irq();
      break;
    case (1<<29):
      uart_send(uart_recv()); 
      break;
    default:
      printf("Unknown pending irq: %x\r\n", irq);
  }
}

