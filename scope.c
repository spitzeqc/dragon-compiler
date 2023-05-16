#include "scope.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "structures/list.h"
#include "structures/hashmap.h"

/* create new empty scope */
scope_t* scope_new()
{
	scope_t* ret = malloc( sizeof(scope_t) );
	assert( ret != NULL );

	ret->next = NULL;
	ret->is_copy = false;
	ret->scope_var_size = 0;
	ret->scope_arg_size = 0;
	ret->table = hashmap_new();

	return ret;
}

/* free all scopes */
void scope_free( scope_t* s )
{
	while( s != NULL )
		s = scope_pop( s );
}

/* add scope to top */
scope_t* scope_push( scope_t* s )
{
	scope_t* new = scope_new();
	new->next = s;
	return new;
}

/* remove scope from top */
scope_t* scope_pop( scope_t* s )
{
	if(s == NULL) return NULL;
	scope_t* next = s->next;
	hashmap_free(s->table);
	free(s);
	return next;
}

/* link a scope to the top of the stack */
void scope_link( scope_t* new, scope_t** top ){
	if(new == NULL) return;

	if( new == *top ) {/* attempting to link scope to itself */
		scope_t* temp = malloc( sizeof(new) );
		memcpy( temp, new, sizeof(new) );
		temp->is_copy = true;
		new = temp;
	}
	new->next = (*top);
	(*top) = new;
}

/* unlink the top scope of the stack */
void scope_unlink( scope_t** top ){
	if(top == NULL) return;
	scope_t* temp = *top;
	*top = (*top)->next;
	temp->next = NULL;
	if(temp->is_copy) free(temp);
}

/* add element to top scope */
bucket_node_t* scope_insert( scope_t* s, char* v )
{
	if(s == NULL) return NULL;
	return hashmap_add_node( s->table, v );
}

/* search top scope */
bucket_node_t* scope_search( scope_t* s, char* v )
{
	if(s == NULL) return NULL;
	return hashmap_get_node( s->table, v );
}

/* search all scopes */
bucket_node_t* scope_global_search( scope_t* s, char* v )
{
	scope_t* current = s;
	bucket_node_t* ret = NULL;
	while(current != NULL) {
		ret = scope_search( current, v );
		if(ret != NULL) break;
		current = current->next;
	}
	return ret;
}

/* print scope */
void scope_print( scope_t* s )
{
	scope_t* current = s;
	int level = 0;
	while(current != NULL) {
		fprintf( stderr, "Level %d\n", level );
		hashmap_print( current->table );
		current = current->next;
		level++;
		fprintf( stderr, "\n" );
	}
}
