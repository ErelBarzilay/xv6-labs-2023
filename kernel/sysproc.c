#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#define PTE_A (1L<<6)
uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}



uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 va;
  int pages;
  uint64 abits;
  unsigned int mask = 0;
  struct proc *p = myproc();

  argaddr(0, &va);
  argint(1, &pages);
  argaddr(2, &abits);

  /* page number upper limit */
  if (pages > 32 || pages < 0){
    return -1;
  }

  for (int i = 0; i < pages; i++) {
    pte_t* pte_p = walk(p->pagetable, va + i * PGSIZE, 0);
    if (*pte_p == 0) {
      return -1;
    }
    if((*pte_p & PTE_V) == 0) {
      return -1;
    }
    if ((PTE_FLAGS(*pte_p) & PTE_A) != 0) { //if valud
      *pte_p =*pte_p & ~PTE_A; //turn off the access bit to assume this 
      mask =mask | 1 << i; //put in the mask now the value 1 in the appropriate location
    }
  }
  if (copyout(p->pagetable, abits, (char *)&mask, sizeof(mask)) < 0) { //if fails to copy mask to abits
    return -1;
  }
  
  return 0;
}