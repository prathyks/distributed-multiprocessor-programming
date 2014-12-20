#include"sem.h"
#define SIZE_MSG 10
#define SIZE_PORT_BUFFER 10
#define NUM_PORTS 100

typedef struct msg_t{
	//We assume first integer in array is always port number of sender
	int replyport;
	int data[SIZE_MSG+1];
} msg_t;

typedef struct port_t{
	msg_t msg_buffer[SIZE_PORT_BUFFER];
	int in;
	int out;
	Semaphore_t* empty;
	Semaphore_t* full;
	Semaphore_t* mutex;
} port_t;

port_t set[NUM_PORTS];
int static set_counter=0;

int create_port(){
	int temp = set_counter;
	set[temp].empty = CreateSem(0);
	set[temp].full = CreateSem(SIZE_PORT_BUFFER);
	set[temp].mutex = CreateSem(1);
	set[temp].in=0;
	set[temp].out=0;
	set_counter++;
	return temp;
}

void send(int dest, msg_t message){
	port_t* dest_port = &(set[dest]);
	P(dest_port->full);
	P(dest_port->mutex);
	dest_port->msg_buffer[dest_port->in] = message;
	dest_port->in = (dest_port->in+1) % SIZE_PORT_BUFFER;
	V(dest_port->mutex);
	V(dest_port->empty);
}

msg_t* receive(int src, msg_t* result){
	port_t* src_port = &(set[src]);
	P(src_port->empty);
	P(src_port->mutex);
	memcpy(result,&(src_port->msg_buffer[src_port->out]),sizeof(msg_t));
	src_port->out = (src_port->out + 1) % SIZE_PORT_BUFFER;
	V(src_port->mutex);
	V(src_port->full);
	return result;
}
