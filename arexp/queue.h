#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdlib.h>

/* structure for the elements */
typedef struct QueueElmt_ {
	void * data;
	struct QueueElmt_ * next;
} QueueElmt;

/* structure for the queue */
typedef struct Queue_ {
	int size;
	void (*destroy)(void * data);
	QueueElmt * head;
	QueueElmt * tail;
} Queue;

/* public interface */
void queue_init(Queue * queue, void (*destroy)(void * data));
/* 
returns: nothing

description: Initializes the queue specified by queue. Must be called before queue can be used. 
destroy provides a user defined way of freeing memory when queue_destroy is called. destroy can be NULL
if elements must not be deallocated after the queue is destroyed.

complexity: O(1) 
*/

void queue_destroy(Queue * queue);
/*
returns: nothing

description: Destroys queue by calling destroy provided in queue_init, if it's not NULL. 
No other operations are permitted after calling queue_destroy.

complexity: O(n)
*/

int queue_enq(Queue * queue, const void * data);
/*
returns: 0 on success, -1 on failure

description: Pushes an element on the queue.

complexity: O(1)
*/

int queue_deq(Queue * queue, void ** data);
/*
returns: 0 on success, -1 on failure

description: Dequeues an element from the queue. Upon return, data points to the data stored in the 
element that was popped. The caller manages memory.

complexity: O(1)
*/

#define queue_peek(queue) ((queue)->head == NULL ? NULL : (queue)->head->data)
/*
returns: pointer to the data at the front of the queue, or NULL if queue is empty.

description: Macro for peeking in the queue.

complexity: O(1)
*/

#endif