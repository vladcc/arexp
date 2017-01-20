/* stack.c -- an implementation of a stack */

#include <stdlib.h> // for NULL
#include <string.h> // for memset()
#include <stdbool.h>
#include "stack.h"

void stack_init(Stack * stack, void (*destroy)(void * data))
{
	/* initialize the stack */
	stack->size = 0;
	stack->destroy = destroy;
	stack->head = NULL;
	
	return;
}

void stack_destroy(Stack * stack)
{
	/* remove each element */
	void * data;
	
	while (stack->size > 0)
	{
		if ( (stack_pop(stack, (void **)&data) == 0) && (stack->destroy != NULL) )
			stack->destroy(data);
	}
	// zero out memory of the structure
	memset(stack, 0, sizeof(*stack));
	
	return;
}

int stack_push(Stack * stack, const void * data)
{
	/* insert as next element */
	StackElmt * new_element;
	
	// allocate storage
	if ( (new_element = (StackElmt *)malloc(sizeof(*new_element))) == NULL )
		return -1;
	
	new_element->data = (void *)data;
	
	// insert at the head		
	new_element->next = stack->head;
	stack->head = new_element;
	
	// update size
	stack->size++;
	
	return 0;
}

int stack_pop(Stack * stack, void ** data)
{
	/* remove next element */
	StackElmt * old_element;
	
	// don't pop from an empty stack
	if (0 == stack->size)
		return -1;
	
	// remove the head
	*data = stack->head->data;
	old_element = stack->head;
	stack->head = stack->head->next;

	// free element memory
	free(old_element);
	
	// adjust size
	stack->size--;
	
	return 0;
}