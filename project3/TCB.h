#include <ucontext.h>
#include <string.h> /* memset */

typedef struct TCB_t{
	struct TCB_t *next;
	ucontext_t context;
	//int thread_num; //giving a number to thread just for our reference, to be used in printQueue
	struct TCB_t *previous;
} TCB_t;

void init_TCB (TCB_t *tcb, void *function, void *stackP, int stack_size)
{
    memset(tcb, '\0', sizeof(TCB_t));       // wash, rinse
    //tcb->thread_num=num;              //initialize given number to queue
    getcontext(&tcb->context);              // have to get parent context, else snow forms on hell
    tcb->context.uc_stack.ss_sp = stackP;
    tcb->context.uc_stack.ss_size = (size_t) stack_size;
    makecontext(&tcb->context, function, 1);// context is now cooked
}
