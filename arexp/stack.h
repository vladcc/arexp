#ifndef STACK_H_
#define STACK_H_

#include <stdlib.h>

/* structure for the elements */
typedef struct StackElmt_ {
	void * data;
	struct StackElmt_ * next;
} StackElmt;

/* structure for the stack */
typedef struct Stack_ {
	int size;
	void (*destroy)(void * data);
	StackElmt * head;
} Stack;

/* public interface */
void stack_init(Stack * stack, void (*destroy)(void * data));
/* 
returns: nothing

description: Initializes the stack specified by stack. Must be called before stack can be used. 
destroy provides a user defined way of freeing memory when stack_destroy is called. destroy can be NULL
if elements must not be deallocated after the stack is destroyed.

complexity: O(1) 
*/

void stack_destroy(Stack * stack);
/*
returns: nothing

description: Destroys stack by calling destroy provided in stack_init, if it's not NULL. 
No other operations are permitted after calling stack_destroy.

complexity: O(n)
*/

int stack_push(Stack * stack, const void * data);
/*
returns: 0 on success, -1 on failure

description: Pushes an element on the stack.

complexity: O(1)
*/

int stack_pop(Stack * stack, void ** data);
/*
returns: 0 on success, -1 on failure

description: Pops an element from the stack. Upon return, data points to the data stored in the 
element that was popped. The caller manages memory.

complexity: O(1)
*/

#define stack_peek(stack) ((stack)->head == NULL ? NULL : (stack)->head->data)
/*
returns: pointer to the data on top of the stack, or NULL if stack is empty.

description: Macro for peeking in the stack.

complexity: O(1)
*/

#endif