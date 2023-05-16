#include "hashmap.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* hash function */
int hashpjw( char* s ) {
	char *p;
	unsigned h = 0, g;
	for( p=s; *p != '\0'; ++p ) {
		h = (h<<4) + (*p);
		if( (g = h&0xF0000000) ) {
			h ^= g >> 24;
			h ^=g;
		}
	}

	return h % PRIME;
}

/* create new empty hashmap */
bucket_node_t** hashmap_new() {
	bucket_node_t** ret = malloc( PRIME * sizeof(bucket_node_t*) );
	assert(ret != NULL);
	memset(ret, 0, (sizeof(bucket_node_t*) * PRIME));
	return ret;	
}

/* free a hashmap */
void hashmap_free( bucket_node_t** h ) {
	int i;
	/* clear each bucket */
	for(i=0; i<PRIME; ++i) {
		bucket_node_t* temp = h[i];
		while( temp != NULL ) { /* free each element within the bucket */
			bucket_node_t* node = temp;
			temp = temp->next;

			//queue_free(&(node->args));
			free(node->name);
			free(node);
		}
	}

	free(h); /* free the array */
}

/* add a new element to the hashmap */
bucket_node_t* hashmap_add_node( bucket_node_t** h, char* name ) {
	/* allocate mem */
	bucket_node_t* new = malloc( sizeof(bucket_node_t) );
	assert(new != NULL);

	/* hash index */
	int index = hashpjw( name );

	/* assign values */
	new->name = strdup(name);
	new->next = h[index];

	/* update bucket */
	h[index] = new;

	return new;
}

/* get a bucket node at a given key, return NULL if not present */
bucket_node_t* hashmap_get_node( bucket_node_t** h, char* name ) {
	int index = hashpjw( name );
	bucket_node_t* check = h[index];
	while(check != NULL) {
		if( strcmp(check->name, name) == 0 )
			return check;

		check = check->next;
	}
	
	return NULL;
}

/* get the value given a name */
int hashmap_get_node_type( bucket_node_t** h, char* name ) {
	bucket_node_t* node = hashmap_get_node(h, name);
	return node != NULL ? node->type : -1;
}

/* remove an item from the hashmap */
bool hashmap_remove( bucket_node_t** h, char* name ) {
	int index = hashpjw( name );
	bucket_node_t* check = h[index];
	bucket_node_t* previous = NULL;
	/* check each element of the bucket */
	while (check != NULL) {
		if( strcmp(check->name, name) == 0 ) {
			bucket_node_t* temp = check->next;
			/* free node's memory */
			//queue_free(&(check->args));
			free(check->name);
			free(check);

			if(previous == NULL)
				h[index] = temp;
			else
				previous->next = temp;
			return true; /* successfully removed */
		}
		previous = check;
		check = check->next;
	}
	return false; /* could not find value */
}

void hashmap_print( bucket_node_t** h )
{
	int i;
	for(i=0; i<PRIME; ++i) {
		bucket_node_t* current = h[i];
		while(current != NULL) {
			fprintf(stderr, "\"%s\": \"%s\"\n", current->name, current->value);
			current = current->next;
		}
	}
}
