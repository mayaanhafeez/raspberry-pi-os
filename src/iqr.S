.globl irq_vector_init
irq_vector_init:
  adr x0, vectors
  msr vbal_el1, x0
  ret

.globl enable_irq
enable_irq:
  msr daiclr, #2
  ret

