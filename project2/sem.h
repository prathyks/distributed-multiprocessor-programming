#include"threads.h"

typedef struct Semaphore_t{
	struct TCB_t *semQ;
	int counter;
} Semaphore_t;

Semaphore_t* CreateSem(int InputValue){
	Semaphore_t* cur_sem = (Semaphore_t*) malloc(sizeof(Semaphore_t));
	cur_sem->counter = InputValue;
	cur_sem->semQ = NULL;
	return cur_sem;
}

void P(Semaphore_t* sem){
	TCB_t* temp;
	sem->counter--;
	if(sem->counter < 0){
		temp = DelQ(&RunQ);
		AddQ(&(sem->semQ), temp);
		swapcontext(&(temp->context),&(RunQ->context));
	}
}

void V(Semaphore_t* sem){
	TCB_t* temp;
	sem->counter++;
	if(sem->counter <= 0){
		temp = DelQ(&(sem->semQ));
		AddQ(&RunQ,temp);
	}
	yield_t();
}
