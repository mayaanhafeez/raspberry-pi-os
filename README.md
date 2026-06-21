# raspberry-pi-os

A bare-metal operating system kernel for the Raspberry Pi 3 (AArch64),
written from scratch in C and ARMv8 assembly. This follows the excellent
[s-matyukevich/raspberry-pi-os](https://github.com/s-matyukevich/raspberry-pi-os)
lesson series, building the kernel up one subsystem at a time.

No libc, no bootloader beyond the Pi's own firmware — just a `kernel8.img`
that the GPU loads and jumps into.

## Progress

Lessons 1–5 are complete; lesson 6 (virtual memory) is not yet started.

- [x] **1 — Kernel boot:** boot code, BSS setup, mini UART driver, `printf`
- [x] **2 — Processor initialization:** drop from EL3/EL2 to EL1, exception levels
- [x] **3 — Interrupt handling:** exception vector table, IRQ routing, system timer
- [x] **4 — Process scheduler:** `copy_process`, round-robin `schedule()`, context switch
- [x] **5 — User processes & system calls:** `move_to_user_mode`, EL0 user mode,
  syscalls (`write`, `malloc`, `clone`, `exit`), `fork`-style process creation
- [ ] **6 — Virtual memory management** *(next)*

The current `kernel_main` boots a kernel process that switches to user mode
and spawns two user processes via the `clone` syscall; each prints a repeating
string through `sys_write`, demonstrating preemptive scheduling between
user-mode tasks.

## Requirements

- An AArch64 bare-metal toolchain: `aarch64-elf-gcc`, `aarch64-elf-ld`,
  `aarch64-elf-objcopy` (the Makefile uses the `aarch64-elf` prefix).
  - On macOS: `brew install aarch64-elf-gcc aarch64-elf-binutils`
- [QEMU](https://www.qemu.org/) with `qemu-system-aarch64` for emulation.
  - On macOS: `brew install qemu`

## Build

```bash
make            # produces kernel8.img
make clean      # remove build artifacts
```

## Run (QEMU)

```bash
./run.sh
```

This rebuilds and launches the kernel under QEMU's `raspi3b` machine, with the
mini UART wired to stdio (`-serial null -serial stdio`) and no display. Output
appears directly in your terminal; press `Ctrl-A X` to quit QEMU.

## Run on real hardware

Copy `kernel8.img` and `src/config.txt` to the root of a FAT32 SD card
alongside the Raspberry Pi firmware (`bootcode.bin`, `start.elf`, etc.) and
boot a Raspberry Pi 3. `config.txt` sets `kernel_old=1` and
`disable_commandline_tags=1` so the firmware loads the kernel at `0x80000`
without prepending ATAGs. A USB-to-TTL serial cable on the mini UART pins
shows the output.

## Layout

```
src/
├── boot.S          entry point: park secondary cores, zero BSS, call kernel_main
├── entry.S         exception vector table + register save/restore
├── irq.{c,S}       IRQ setup and the C-level interrupt handler
├── timer.c         system timer driver (scheduler tick)
├── mini_uart.c     mini UART driver (serial I/O)
├── uart.c          PL011 UART driver
├── printf.c        tinyprintf implementation (no libc)
├── mm.{c,S}        physical page allocator + memory routines
├── sched.{c,S}     scheduler, context switch
├── fork.c          process creation (copy_process, move_to_user_mode)
├── sys.{c,S}       system call table and trampolines
├── utils.S         small register/MMIO helpers
├── kernel.c        kernel_main + demo kernel/user processes
├── linker.ld       memory layout (kernel loaded at 0x80000)
└── config.txt      Raspberry Pi firmware config
include/            headers, incl. peripherals/ for BCM2837 register maps
Makefile            build rules
run.sh              build + launch under QEMU
```

## Credits

Built by following Sergey Matyukevich's
[Raspberry Pi OS](https://github.com/s-matyukevich/raspberry-pi-os) tutorial.
