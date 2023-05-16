#include "list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

list_t *list_new( char* v )
{
	list_t* ret = malloc( sizeof(list_t) );
	assert( ret != NULL );
	
	// make copy of string before assigning
	ret->name = strdup(v);	
	ret->next = NULL;

	return ret;
}

void list_free( list_t* l )
{
	list_t* next;
	while(l != NULL) {
		next = l->next
		free(l->name);
		free(l);
		l = next;
	}
	l = NULL;
}

/* insert to top of linked list
 * return new top of list
 */
list_t *list_insert( list_t* l, char* v )
{
	list_t* new = make_list( v );
	new->next = l;
	return l = new;
}

/* search list for value
 * return node if found
 */
list_t *list_search( list_t* l, char* v )
{
	list_t* current = l;
	while( current != NULL ) {
		if(strcmp(current->name, v) == 0)
			return current;
		current = current->next;
	}
	return NULL;
}
