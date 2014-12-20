#include<stdio.h>
#include<stdlib.h>
#include"msgs.h"
#define NUM_SERVERS 1 // Number of servers to start
#define NUM_CLIENTS 3 // Number of clients to start
#define NUM_PRECODED_STRINGS 5
#define SERVER_BUFFER_SIZE 10


/*
 * Project group
 * Prateek Shankar (ASU id: 1207643108)
 * Tejdeep Kautharam (ASU id:1207690064)
*/

//list of server ports started
int server_port;
char *server_table[SERVER_BUFFER_SIZE];
int server_in=0,server_out=0;
Semaphore_t *server_mutex;
Semaphore_t *client_mutex;
Semaphore_t *server_empty;
char *pre_coded_strings[NUM_PRECODED_STRINGS];

int OPCODE_ADD = 0;
int OPCODE_DELETE = 1;
int OPCODE_MODIFY = 2;
int OPCODE_PRINT = 3;

int RESULT_OK = 1;

void printInstructions(){
	int temp;
	char *instr = "--------------------------------------------------------------------------------------\n"
			"Program runs 1 server and 3 clients\n"
			"1. Server maintain table of 10 strings\n"
			"2. Server provides following service:\n"
			"\ta. adding string to server table at any available empty slot. If buffer is full, prints the message accordingly\n"
			"\tb. deleting string from server table at given index.If the given index is empty, prints the message accordingly\n"
			"\tc. modifying string from server table at given index.Replaces old string with given string.If the given index is empty, prints the message accordingly\n"
			"\td. returns whole string table data to client that requested it.\n"
			"3. Client 1 and 2 requests adding, deleting and modifying strings from string table\n"
			"4. Client 3 requests string table data and prints it.\n"
			"--------------------------------------------------------------------------------------\n";
	printf("%s\n",instr);
	printf("Press ENTER to continue.\n");
	getchar();
}

char* get_random_string(int port_num){
	int rand_i = rand() % NUM_PRECODED_STRINGS;
	return pre_coded_strings[rand_i];
}

void init_pre_coded_strings(){
	pre_coded_strings[0] = "blahblahblahblah";
	pre_coded_strings[1] = "testtesttest";
	pre_coded_strings[2] = "Bye";
	pre_coded_strings[3] = "Hello";
	pre_coded_strings[4] = "Hi There";
}

void add_string_to_table(char* msg){
	int i=0;
	char* str = malloc(strlen(msg)+1);
	for(i=0;i<=strlen(msg)+1;i++){
		str[i]=msg[i];
	}
	P(server_mutex);
	for(i=0; i< SERVER_BUFFER_SIZE; i++){
		if(server_table[i] == 0){
			printf("SERVER: ADDING %s at pos: %d\n",str,i);
			server_table[i] = str;
			break;
		}
	}
	if(i == (SERVER_BUFFER_SIZE)){
		printf("SERVER: CANNOT ADD.SERVER IS FULL\n");
		free(str);
	}
	V(server_mutex);
}

void modify_string_in_table(char*msg, int ind){
	int i=0;
	char* str = malloc(strlen(msg)+1);
	for(i=0;i<=strlen(msg)+1;i++){
		str[i]=msg[i];
	}
	P(server_mutex);
	if(server_table[ind] == 0){
		printf("SERVER: CANNOT MODIFY.pos %d IS EMPTY\n",ind);
		free(str);
	}else{
		printf("SERVER: MODIFYING STRING at pos:%d, OLD STR: %s, NEW STR:%s\n",ind,server_table[ind],str);
		free(server_table[ind]);
		//printf("SERVER: DELETED at pos:%d\n",ind);
		server_table[ind] = str;
	}
	V(server_mutex);
}

void delete_string_to_table(msg_t *msg){
	int ind = msg->data[1];
	P(server_mutex);
	if(server_table[ind] == '\0'){
		printf("SERVER: CANNOT DELETE.No string at pos:%d\n",ind);
	}else{
		printf("SERVER: DELETING %s at pos:%d\n",server_table[ind],ind);
		free(server_table[ind]);
		//printf("SERVER: DELETED at pos:%d\n",ind);
		server_table[ind] = 0;
	}
	V(server_mutex);
}

void init_table(){
	int i;
	for(i=0; i< SERVER_BUFFER_SIZE; i++){
		server_table[i] = 0;
	}
}

void print_table(msg_t *msg){
	int i,j;
	int reply_port = msg->replyport;
	int count=0;
	int send_count=0;
	char send_buffer[1000];
	msg_t *temp;
	printf("SERVER: PRINT INFO SEND: \n");
	P(server_mutex);
	send_buffer[0]=0;
	for(i=0; i<SERVER_BUFFER_SIZE;i++){
		char *str;
		str = server_table[i];
		if(str == 0){
			send_buffer[count++]='\n';
			continue;
		}
		while(*str != '\0'){
			send_buffer[count++] = *str;
			str++;
		}
		send_buffer[count++]='\n';
	}
	send_buffer[count] = '\0';
	V(server_mutex);

	printf("\n");
	temp = (msg_t*) malloc(sizeof(msg_t));
	count=0;
	for(i=0; i<strlen(send_buffer); i++){
		temp->data[count] = (int)send_buffer[i];
		if(count == SIZE_MSG-1){
			send_count++;
			send(reply_port,temp);
			free(temp);
			temp = (msg_t*) malloc(sizeof(msg_t));
			temp->replyport=server_port;
			count=0;
		}else
			count++;
	}
	temp->data[count]='\0';
	send_count++;
	send(reply_port,temp);
	//printf("SENT_COUNT:%d",send_count);
	free(temp);
}

//Server process
void server(){
	int i,count,len;
	char add_buff[3][100];
	int add_count[3];
	char mod_buff[3][100];
	int mod_count[3];
	int mod_ind[3];
	msg_t *ret_msg;
	server_mutex = CreateSem(1);
	init_table();
	server_port = create_port();
	printf("Server created at port %d\n",server_port);
	while(1){
		msg_t *ret_msg = malloc(sizeof(msg_t));
		receive(server_port,ret_msg);
		int client = ret_msg->replyport;
		int opcode = ret_msg->data[0];
		switch(opcode){
		case 0://When action is ADD
			for(i=1;i<SIZE_MSG;i++){
				if(ret_msg->data[i] == '\0'){
					add_buff[client][add_count[client]]='\0';
					add_string_to_table(add_buff[client]);
					add_count[client]=0;
					break;
				}else{
					add_buff[client][add_count[client]++] = ret_msg->data[i];
				}
			}
			free(ret_msg);
			break;
		case 1://When action is DELETE
			delete_string_to_table(ret_msg);
			free(ret_msg);
			break;
		case 2://When action is MODIFY
			mod_ind[client]=ret_msg->data[1];
			for(i=2;i<SIZE_MSG;i++){
				if(ret_msg->data[i] == '\0'){
					mod_buff[client][mod_count[client]]='\0';
					modify_string_in_table(mod_buff[client],mod_ind[client]);
					mod_count[client]=0;
					break;
				}else{
					mod_buff[client][mod_count[client]++] = ret_msg->data[i];
				}
			}
			free(ret_msg);
			break;
		case 3://When action is PRINT
			print_table(ret_msg);
			free(ret_msg);
			break;
		}
		sleep(1);
	}
}

int get_random_opcode(){
	return rand()%3;
}


void client_add_modify(){
	int i,count,ind;
	int port_num = create_port();
	printf("Client created at port %d\n",port_num);
	while(1){
		char *msg_str;
		msg_str = get_random_string(port_num);
		int opcode = get_random_opcode();
		//int opcode = 0;
		msg_t *msg = (msg_t*) malloc(sizeof(msg_t));
		msg->replyport=port_num;
		//package_msg(msg_str,opcode,msg,port_num);
		switch(opcode){
		case 0://ADD
			msg->replyport=port_num;
			msg->data[0]=opcode;
			count=1;
			printf("CLIENT%d: ADD string: %s\n",port_num,msg_str);
			for(i=0; i<strlen(msg_str); i++){
				msg->data[count] = (int)msg_str[i];
				if(count == SIZE_MSG-1){
					send(server_port,msg);
					free(msg);
					msg = (msg_t*) malloc(sizeof(msg_t));
					msg->replyport=port_num;
					msg->data[0]=opcode;
					count=1;
				}else
					count++;
			}
			msg->data[count]='\0';
			send(server_port,msg);
			free(msg);
			break;
		case 1://DELETE
			msg->replyport=port_num;
			msg->data[0]=opcode;
			ind = rand() % SERVER_BUFFER_SIZE;
			msg->data[1]=ind;
			send(server_port,msg);
			printf("CLIENT%d: DELETE pos %d\n",port_num,ind);
			free(msg);
			break;
		case 2://MODIFY
			ind = rand() % SERVER_BUFFER_SIZE;
			msg->replyport=port_num;
			msg->data[0]=opcode;
			msg->data[1]=ind;
			count=2;
			printf("CLIENT%d: MODIFY string: %s,pos:%d\n",port_num,msg_str,ind);
			for(i=0; i<strlen(msg_str); i++){
				msg->data[count] = (int)msg_str[i];
				if(count == SIZE_MSG-1){
					send(server_port,msg);
					free(msg);
					msg = (msg_t*) malloc(sizeof(msg_t));
					msg->replyport=port_num;
					msg->data[0]=opcode;
					msg->data[1]=ind;
					count=2;
				}else
					count++;
			}
			msg->data[count]='\0';
			send(server_port,msg);
			free(msg);
			break;
		}
		sleep(1);
	}
}

void client_print(){
	int port_num = create_port();
	char* msg_str = NULL;
	//char* client_table[SERVER_BUFFER_SIZE];
	char client_buffer[1000];
	int i,j,count;
	printf("Client created at port %d\n",port_num);
	while(1){
		msg_t *msgreq = malloc(sizeof(msg_t));
		msgreq->replyport=port_num;
		//package_msg(msg_str,OPCODE_PRINT,msg,port_num);
		msgreq->data[0]=OPCODE_PRINT;
		printf("CLIENT%d: PRINT opcode: %d\n",port_num,msgreq->data[0]);
		send(server_port, msgreq);
		free(msgreq);

		msg_t *ret_msg = malloc(sizeof(msg_t));
		receive(port_num,ret_msg);
		client_buffer[0]=0;
		count=0;
		int flag=0;
		while(1){
			for(i=0;i<SIZE_MSG;i++){
				if(ret_msg->data[i] == '\0'){
					client_buffer[count++]='\0';
					flag=1;
					break;
				}else{
					client_buffer[count++]=(char)ret_msg->data[i];
				}
			}
			if(flag==1){
				break;
			}
			else{
				free(ret_msg);
				ret_msg = (msg_t*) malloc(sizeof(msg_t));
				receive(port_num,ret_msg);
			}
		}
		free(ret_msg);
		printf("--------------------------------------------\n");
		printf("CLIENT%d: PRINTING stringtable:\n",port_num);
		count=0;
		printf("%d: ",count);
		for(i=0; i<strlen(client_buffer); i++){
			if(client_buffer[i] == '\n' && client_buffer[i+1] != '\0'){
				printf("\n");
				printf("%d: ",++count);
			}else if(client_buffer[i+1] == '\0'){
				continue;
			}
			else{
				printf("%c",client_buffer[i]);
			}
		}
		printf("\n--------------------------------------------\n");
		sleep(1);
	}
}


int main(){
	int i;
	printInstructions();
	init_pre_coded_strings();
	//We first start server threads and then start the client threads
	start_thread(&server);
	start_thread(&client_add_modify);
	start_thread(&client_add_modify);
	start_thread(&client_print);

	run();
	return 0;
}