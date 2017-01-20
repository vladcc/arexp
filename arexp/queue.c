/* queue.c -- an implementation of a queue */

#include <stdlib.h> // for NULL
#include <string.h> // for memset()
#include <stdbool.h>
#include "queue.h"

void queue_init(Queue * queue, void (*destroy)(void * data))
{
	/* initialize the queue */
	queue->size = 0;
	queue->destroy = destroy;
	queue->head = NULL;
	queue->tail = NULL;
	
	return;
}

void queue_destroy(Queue * queue)
{
	/* remove each element */
	void * data;
	
	while (queue->size > 0)
	{
		if ( (queue_deq(queue, (void **)&data) == 0) && (queue->destroy != NULL) )
			queue->destroy(data);
	}
	// zero out memory of the structure
	memset(queue, 0, sizeof(*queue));
	
	return;
}

int queue_enq(Queue * queue, const void * data)
{
	/* enqueue element */
	QueueElmt * new_element;
	
	// allocate storage
	if ( (new_element = (QueueElmt *)malloc(sizeof(*new_element))) == NULL )
		return -1;
	
	new_element->data = (void *)data;
	new_element->next = NULL;
	
	// insert the first element
	if (0 == queue->size)
	{
		queue->head = new_element;
		queue->tail = new_element;	
	}
	else
	{
		// insert at the back of the queue
		queue->tail->next = new_element;
		queue->tail = new_element;
	}
	
	// adjust the size
	queue->size++;
	
	return 0;
}

int queue_deq(Queue * queue, void ** data)
{
	/* dequeue element */
	QueueElmt * old_element;
	
	// don't remove from an empty queue
	if (0 == queue->size)
		return -1;

	if (queue->head == queue->tail)
		queue->tail = NULL;
	
	// remove the head
	*data = queue->head->data;
	old_element = queue->head;
	queue->head = queue->head->next;
	
	// free element memory
	free(old_element);
	
	// adjust size
	queue->size--;
	
	return 0;
}