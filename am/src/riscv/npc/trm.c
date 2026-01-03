#include <am.h>
#include <klib-macros.h>

extern char _heap_start;
int main(const char *args);

#define UART_BASE ((volatile unsigned char *)0x10000000ul)

extern char _pmem_start;
#define PMEM_SIZE (128 * 1024 * 1024)
#define PMEM_END  ((uintptr_t)&_pmem_start + PMEM_SIZE)

Area heap = RANGE(&_heap_start, PMEM_END);
static const char mainargs[MAINARGS_MAX_LEN] = TOSTRING(MAINARGS_PLACEHOLDER); // defined in CFLAGS

#define BUSY (0)
void putch(char ch) {
  while ((UART_BASE[5] & 0x20) == 0); // wait until TX is empty
  UART_BASE[0] = ch;
}
void halt(int code) {
  asm volatile("mv a0, %0; ebreak" : :"r"(code));
  while (1);
}

static void uart_init() {
  unsigned char lcr = UART_BASE[3];
  UART_BASE[3] = lcr | 0x80;
  UART_BASE[1] = 0;
  UART_BASE[0] = 13;
  UART_BASE[3] = lcr;
}

void _trm_init() {
  uart_init();
  int ret = main(mainargs);
  halt(ret);
}
