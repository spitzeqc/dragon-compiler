#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue_s {
	void* value;
	struct queue_s* next;
	struct queue_s* prev;
} queue_t;

void queue_free( queue_t** q );

void queue_push( queue_t** q, void* v );
void* queue_pull( queue_t** q );

#endif
