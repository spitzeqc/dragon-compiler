#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdbool.h>

#include "structures/list.h"
#include "structures/hashmap.h"

#define OFFSET_SIZE 4

#define EOS '\0'

typedef struct scope_s { /* stacktable scope */
	bucket_node_t** table;
	bool is_copy;
	int scope_var_size;
	int scope_arg_size;
	struct scope_s* next;
} scope_t;

scope_t* scope_new();
void scope_free( scope_t* s );

scope_t* scope_push( scope_t* s ); /* push a new scope on to the stack */
scope_t* scope_pop( scope_t* s ); /* pop top scope from the stack */

void scope_link( scope_t* new, scope_t** top ); /* link a scope to the top of the stack */
void scope_unlink( scope_t** top );             /* unlink a scope from the top of the stack */

bucket_node_t* scope_insert( scope_t* s, char* v ); /* insert name into top level scope */
bucket_node_t* scope_search( scope_t* s, char* v ); /* search name in top scope */
bucket_node_t* scope_global_search( scope_t* s, char* v ); /* search name in all scopes (starting at the top) */
void scope_print( scope_t* s ); /* print the scope */
#endif
