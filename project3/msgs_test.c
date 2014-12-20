#include<stdio.h>
#include<stdlib.h>
#include"msgs.h"
#define NUM_SERVERS 3
#define NUM_CLIENTS 15


int server_ports[NUM_SERVERS];
int server_count=0;

msg_t get_random_message(int port_num){
	msg_t temp;
	int i;
	for(i=0; i < SIZE_MSG;i++){
		temp.data[i] = rand();
	}
	temp.replyport = port_num;
	return temp;
}

int get_random_server(){
	int temp = rand() % NUM_SERVERS;
	return server_ports[temp];
}

void server(){
	int server_port = create_port();
	server_ports[server_count++] = server_port;
	printf("Server created at port %d\n",server_port);
	while(1){
		msg_t *msg = malloc(sizeof(msg_t));
		receive(server_port,msg);
		int client = msg->replyport;

		printf("Server at port %d received request from client at port %d\n",server_port,client);
		msg->replyport = server_port;
		//process the message
		//We don't process anything here.

		send(client,*msg);
		free(msg);
		printf("Server at port %d sending reply to client at port %d\n",server_port, client);
		sleep(1);
	}
}

void client(){
	int port_num = create_port();
	printf("Client created at port %d\n",port_num);
	while(1){
		msg_t msg = get_random_message(port_num);
		int server_port = get_random_server();

		printf("Client at port %d is sending request to server at port %d\n",port_num,server_port);
		send(server_port, msg);
		msg_t *ret_msg = malloc(sizeof(msg_t));
		receive(port_num,ret_msg);
		int serv = ret_msg->replyport;
		free(ret_msg);

		printf("Client at port %d received reply from server at port %d\n",port_num,serv);
		sleep(1);
	}
}

int main(){
	int i;
	for(i=0; i < NUM_SERVERS; i++){
		start_thread(&server);
	}
	for(i=0; i < NUM_CLIENTS; i++){
		start_thread(&client);
	}
	run();
	return 0;
}
