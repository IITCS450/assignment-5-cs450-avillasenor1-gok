#pragma once

typedef int tid_t;
void thread_init(void);
tid_t thread_create(void (*fn)(void*), void *arg);
void thread_yield(void);
int thread_join(tid_t tid);



//--------------------------


#define MAX_THREADS 8
#define STACK_SIZE 4096

// thread states
#define FREE 0
#define RUNNABLE 1
#define RUNNING 2
#define ZOMBIE 3

struct context {
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

typedef struct thread {
  tid_t tid;
  int state;
  char *stack;
  struct context *context;
} thread_t;
