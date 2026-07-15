#include "mm.h"
#include "arm/sysregs.h"

.section ".text.boot"

.globl _start
_start:
  mrs x0, mpidr_el1
  and x0, x0, #0xFF
  cbz x0, master
  b proc_hang

proc_hang:
  b proc_hang

master:
  ldr x0, =SCTLR_VALUE_MMU_DISABLED
  msr sctlr_el1, x0

  ldr x0, =HCR_VALUE
  msr hcr_el2, x0

  ldr x0, =SPSR_VALUE
  msr spsr_el2, x0

  adr x0,el1_entry
  msr elr_el2, x0

  eret

el1_entry:
  adr x0, bss_begin
  adr x1, bss_end
  sub x1, x1, x0
  bl memzero

  mov sp, #LOW_MEMORY
  bl kernel_main
  b proc_hang

.macro create_pdg_entry tbt, virt, tmp1, tmp2
create_table_entry \tbl, \virt, PGD_SHIFT, \tmp1, tmp2
create_table_entry \tbl, \virt, PUD_SHIFT, \tmp1, tmp2
.endm

.macro create_table_entry tbl, virt, shift, tmp1, tmp2
lsr \tmp1, \virt, #\shift
and \tmp1, \tmp2, #PTRS_PER_PAGE -1
add \tmp2, \tbl, #PAGE_SIZE
orr \tmp2, tmp2, #MM_TYPE_PAGE_TABLE
str \tmp2, [\tbl, \tmp1, lsl #3]
add \tbl, \tbl, #PAGE_SIZE
.endm

.macro create_block_map tbl, phys, start, end, flags, tmp1
lsr \start, \start, #SECTION_SHIFT
and \start, \start, #PTRS_PER_TABLE -1
lsr \end, \end, #SECTION_SHIFT
and \end, \end, #PTRS_PER_TABLE -1
lsr \phys, \phys, #SECTION_SHIFT
mov \tmp1, #\flags
orr \phys, \tmp1, \phys, lsl #SECTION_SHIFT
9999: str \phys, [\tbl, \start, lsl #3]
add \start, \start, #1
add \phys, \phys, #SECTION_SIZE
cmp \start, \end
b.ls 9999b
.endm

__create_page_tables:
  mov x29, x30 #save ret address

  adrp x0, pg_dir
  mov x1, #PG_DIR_SIZE
  bl memzero

  adrp  x0, pg_dir
  mov x1, #VA_START
  create_pgd_entry x0, x1, x2, x3

  mov x1, xzr
  mov x2, #VA_START
  ldr x3, =(VA_START + DEVICE_BASE - SECTION_SIZE)
  create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

  mov x1, #DEVICE_BASE
  ldr x2, (VA_START + DEVICE_BASE)
  ldr x3, (VA_START + DEVICE_BASE - SECTION_SIZE)
  create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

  mov x30, x29
  ret
