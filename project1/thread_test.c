#include <stdio.h>
#include <stdlib.h>
#include "threads.h"

/*
 * Project group
 * Prateek Shankar (ASU id: 1207643108)
 * Tejdeep Kautharam (ASU id:1207690064)
 */

//Global counter we increment and print in all user threads
int global = 0;

//f1,f2,f3 are infinte running functions that increments and prints global and local variables.
//these functions are treated as user level threads
void f1(){
	int local=0;
	printf("Starting f1\n");
	while(1){
		sleep(1);
		printf("In function f1, local=%d and global=%d\n",local++,global++);
		sleep(1);
		printf("Yielding f1\n");
		yield_t();
	}
}

void f2(){
	int local=0;
	printf("Starting f2\n");
	while(1){
		sleep(1);
		printf("In function f2, local=%d and global=%d\n",local++,global++);
		sleep(1);
		printf("Yielding f2\n");
		yield_t();
	}
}

void f3(){
	int local=0;
	printf("Starting f3\n");
	while(1){
		sleep(1);
		printf("In function f3, local=%d and global=%d\n",local++,global++);
		sleep(1);
		printf("Yielding f3\n");
		yield_t();
	}
}

//Main function where we start functions f1,f2 and f3.
int main(void) {
	start_thread(&f1,1);
	start_thread(&f2,2);
	start_thread(&f3,3);
	run();
	return 0;
}
