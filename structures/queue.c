#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* free a queue *and* all stored values */
void queue_free (queue_t** q ) {
	if( q == NULL) return;

	/* remove all but last node */
	while( (*q)->next != (*q)->prev )
		free( queue_pull( q ) ); /* free returned value */

	free( queue_pull( q ) ); /* free final node and value */
	(*q) = NULL;
}

/* make an empty queue_t node */
queue_t* queue_make_node( void* v ) {
	queue_t* ret = malloc( sizeof(queue_t*) );
	assert(ret != NULL);
	
	ret->value = v;
	ret->next = ret;
	ret->prev = ret;

	return ret;
}

/* pull an item from the queue */
void* queue_pull( queue_t** q ) {
	if( q == NULL ) return NULL;

	void* ret = (*q)->value;
	
	queue_t* new_head = (*q)->next;
	queue_t* tail = (*q)->prev;

	tail->next = new_head;
	new_head->prev = tail;

	(*q)->value = NULL; /* replace with null to be 100% certain we dont free our return value */
	free( (*q) );

	(*q) = new_head; /* update the address of q */

	return ret;
}

/* add an item to a queue */
void queue_push( queue_t** q, void* v ) {
	if(q == NULL) { /* Pushing onto empty queue, make a new node */
		*q = queue_make_node( v );
		return;
	}

	queue_t* new = queue_make_node( v );
	queue_t* last_node = (*q)->prev;

	new->prev = last_node;
	last_node->next = new;
	(*q)->prev = new;
	new->next = (*q);
}