#ifndef STACK_H
#define STACK_H

typedef struct stack_s {
	void* value;
	struct stack_s *next;
} stack_t;

stack_t* stack_make( void* v );
void stack_free( stack_t** s );

void stack_push( stack_t** s, void* v );
void* stack_pop( stack_t** s );
void* stack_top( stack_t* s );

#endif
