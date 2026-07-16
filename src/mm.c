#include "mm.h"
#include "mmu.h"
#include "sched.h"

static unsigned short mem_map [PAGING_PAGES] = {0,};

unsigned long allocate_user_page(struct task_struct *task, unsigned long va){
  unsigned long page = get_free_page();
  if (page ==0) {return 0;}
  map_page(task, va, page);
  return page+ VA_START;
}

unsigned long get_free_page(){
  for (int i=0;i<PAGING_PAGES; i++){
      if (mem_map[i]==0){
          mem_map[i]=1;
          return LOW_MEMORY + i*PAGE_SIZE;
      }
  }
  return 0;
}

void *memset(void *s, int c, unsigned long n) {
    unsigned char *p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

void free_page(unsigned long p){
  mem_map[(p - LOW_MEMORY) / PAGE_SIZE] =0;
}
