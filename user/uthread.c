#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"

// TODO: Implement cooperative user-level threads.


void thread_stub(void);


void swtch(struct context **old, struct context *new);


#define MAX_THREADS 8
#define STACK_SIZE 4096


thread_t threads[MAX_THREADS];
int current_thread = 0;

//-------------------
static int find_free_thread() {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (threads[i].state == FREE)
            return i;
    }
    return -1;
}

//--------------




void thread_init(void){


 for(int i = 0; i < MAX_THREADS; i++){
        threads[i].tid = i;
        threads[i].state = FREE;
        threads[i].stack = 0;
        threads[i].context = 0;
    }

    threads[0].state = RUNNING;
    current_thread = 0;
}



//----------------------
tid_t thread_create(void (*fn)(void*), void *arg) {
    int tid = find_free_thread();
    if (tid < 0) return -1;

    thread_t *t = &threads[tid];
    t->stack = malloc(STACK_SIZE);
    if (!t->stack) return -1;

    
    char *sp = t->stack + STACK_SIZE;

    sp -= sizeof(struct context);
    t->context = (struct context*)sp;

  
    t->context->eip = (uint)thread_stub; 
    t->context->esi = (uint)fn;          
    t->context->edi = (uint)arg;         
    t->context->ebx = 0;
    t->context->ebp = 0;

    t->state = RUNNABLE;
    return tid;
}
//---------------






void thread_yield(void) {
    int old = current_thread;
    int next = (old + 1) % MAX_THREADS;

    
    for(int i = 0; i < MAX_THREADS; i++) {
        if(threads[next].state == RUNNABLE) {
            current_thread = next;
            threads[next].state = RUNNING;

            if(threads[old].state == RUNNING) {
                threads[old].state = RUNNABLE;
            }

            swtch(&threads[old].context, threads[next].context);
            return;
        }
        next = (next + 1) % MAX_THREADS;
    }
   
}




int thread_join(tid_t tid) {
    if(tid < 0 || tid >= MAX_THREADS || threads[tid].state == FREE)
        return -1;

    
    while(threads[tid].state != ZOMBIE) {
        thread_yield();
    }

    if(threads[tid].stack) {
        free(threads[tid].stack);
        threads[tid].stack = 0;
    }
    threads[tid].state = FREE;
    return 0;
}





void thread_stub(void) {
    void (*fn)(void*);
    void *arg;

    asm volatile("movl %%esi, %0" : "=r"(fn));
    asm volatile("movl %%edi, %0" : "=r"(arg));

    fn(arg);
    threads[current_thread].state = ZOMBIE;

   
    while(1) {
        thread_yield();
    }
}



asm(
".globl swtch\n"
"swtch:\n"
"  movl 4(%esp), %eax\n"
"  movl 8(%esp), %edx\n"
"  pushl %ebp\n"
"  pushl %ebx\n"
"  pushl %esi\n"
"  pushl %edi\n"
"  movl %esp, (%eax)\n"
"  movl %edx, %esp\n"
"  popl %edi\n"
"  popl %esi\n"
"  popl %ebx\n"
"  popl %ebp\n"
"  ret\n"
);
