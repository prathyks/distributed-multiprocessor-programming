#include "q.h"

//runQ which queues active user threads
TCB_t* RunQ = NULL;
int STACK_SIZE = 8192;

void start_thread(void (*function)(int *), int *num)
{
	TCB_t* temp = NULL;
	if(RunQ == NULL){
		RunQ = InitQ(&RunQ);
	}
	//allocate a stack of a certain size
	void* stack = (void*) malloc(STACK_SIZE);
	//allocate a TCB
	temp = (TCB_t*) malloc(sizeof(TCB_t));
	//call init_TCB with appropriate arguments
	init_TCB(temp, (*function), stack, STACK_SIZE, num);
	//call addQ to add this TCB into the “RunQ” which is a global header pointer
	AddQ(&RunQ, temp);
	//print queue for our reference
	printf("runQ : ");
	printQueue(RunQ);
	//end pseudo code
}

void run()
{   // real code
	ucontext_t parent;     // get a place to store the main context, for faking
	getcontext(&parent);   // magic sauce
	swapcontext(&parent, &(RunQ->context));  // start the first thread
}

void yield_t() // similar to run
{
	TCB_t* cur=RunQ;

	//rotate the run Q;
	RotateQ(&RunQ);
//	printf("runQ : ");
//	printQueue(RunQ);

	//swap the context, from previous thread to the thread pointed to by runQ
	swapcontext(&(cur->context),&(RunQ->context));
}
