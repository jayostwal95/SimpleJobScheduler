/* Implementation of a simple circular queue using a static array */
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"


/* create the queue data structure and initialize it */
queue *queue_init(int n) {
	queue *q = (queue *)malloc(sizeof(queue));
	q->size = n;
	q->buffer = (qs *)malloc(sizeof(qs)*n);
	q->start = 0;
	q->start = 0;
	q->end = 0;
	q->count = 0;
	return q;
}

/* insert an item into the queue, update the pointers and count, and
   return the no. of items in the queue (-1 if queue is null or full) */
int queue_insert(queue *q, int id, char *command) {
	//if ((q == NULL) || (q->id < 0))
	int end_t = q->end % q->size;
	q->buffer[q->end % q->size].id = id;
	//q->command = command;
	q->buffer[q->end % q->size].cmd = command;
	q->buffer[q->end % q->size].status= "Waiting";
	q->end = (q->end + 1) % q->size;
	q->count++;
	return end_t;
}

void queue_update(queue *q, int end_t){
	q->buffer[end_t].status="Running";
	//printf("status = %s\n", q->buffer[q->end % q->size].status);
}

/* delete an item from the queue, update the pointers and count, and
   return the item deleted (-1 if queue is null or empty) */

int queue_delete(queue *q) {
	if ((q == NULL) || (q->count == 0))
	   return -1;

	//int x = q->buffer[q->start];
	q->start = (q->start + 1) % q->size;
	q->count--;
}

/* display the contents of the queue data structure */
void queue_display(queue *q) {
	int i;
	if (q != NULL && q->buffer->id >= 0) {
		for (i = 0; i < q->count; i++)
			printf("%d\t%s\t%s\n",
				q->buffer[(q->start + i) % q->size].id, q->buffer[(q->start + i) % q->size].cmd, q->buffer[(q->start + i) % q->size].status);
	} else
		printf("queue empty, nothing to display\n");
}

/* delete the queue data structure */
/*
void queue_destroy(queue *q) {
	free(q->buffer);
	free(q);
}
*/
