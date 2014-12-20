#include "TCB.h"

//If head is a existing Q, free head and start a new Q
TCB_t* InitQ(TCB_t **head){
	free(*head);
	TCB_t *newHead = NULL;
	return newHead;
}

//PrintQueue just for our reference.
void printQueue(TCB_t *head){
	TCB_t *temp;
	temp=head;
	if(head == NULL){
		printf("Queue empty.\n");
		return;
	}
	printf("%d",head->thread_num);
	while(temp->next != head){
		temp = temp->next;
		printf(" -> %d", temp->thread_num);
	}
	printf("\n");
}

//Add new item to the queue at the end of list.
void AddQ(TCB_t **head, TCB_t* newItem){
	TCB_t* tempHead = (*head);
	//if queue is empty, new item is the only item.
	if(tempHead == NULL){
		newItem->next = newItem;
		newItem->previous = newItem;
		(*head) = newItem;
		return;
	}
	tempHead->previous->next = newItem;
	newItem->previous = tempHead->previous;
	newItem->next = tempHead;
	tempHead->previous = newItem;
}

//Delete the head of queue and head will now point to next item on the queue.
TCB_t* DelQ(TCB_t **head){
	TCB_t *temp;
	temp=(*head);
	TCB_t *newItem;
	if(temp == NULL){
		printf("Queue Empty. Cannot delete.\n");
		return NULL;
	}
	//When only one item is remaining in the queue and we delete it and make head as NULL
	//copy the last item to new location and send the newItem.
	if(temp->next == temp){
		newItem = (TCB_t*) malloc(sizeof(TCB_t));
		memcpy(newItem, temp, sizeof(TCB_t));
		free(temp);
		(*head)=NULL;
		return newItem;
	}
	temp->previous->next = temp->next;
	temp->next->previous = temp->previous;
	*head = temp->next;
	return temp;
}

//Rotates the queue. ie, Moves the head to next position
void RotateQ(TCB_t **head){
	*head = (*head)->next;
}


