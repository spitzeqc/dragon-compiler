#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* create a new stack node with a given value */
stack_t* stack_make_node( void* v ) {
	stack_t* new = malloc( sizeof(stack_t) );
	assert(new != NULL);

	new->value = v;
}

/* push value onto stack */
void stack_push( stack_t** s, void* v ) {
	stack_t* new = stack_make_node( v );
	new->next = (*s);
	*s = new;
}

/* get a value from a stack */
void* stack_pop( stack_t** s ) {
	assert(s != NULL);
	
	void* ret = (*s)->value;
	stack_t* temp = *s;

	*s = (*s)->next;
	
	free(temp);

	return ret;
}

/* free entire stack *and* values */
void stack_free( stack_t** s ) {
	while (*s != NULL)
		free( stack_pop(s) );
}

/* see top item on a stack */
inline void* stack_top( stack_t* s ) {
	return s->value;
}