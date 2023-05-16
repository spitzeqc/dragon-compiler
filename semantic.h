#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdbool.h>

#include "structures/tree.h"
#include "scope.h"

bucket_node_t* semantic_lookup( scope_t* top_scope, char* name );
bucket_node_t* semantic_scope_insert( scope_t* top_scope, char* name );
void semantic_set_types( tree_t* id_list, int type_val );
void semantic_set_classes( tree_t* id_list, int class_val );
void semantic_set_args( tree_t* id_list, tree_t* args_list );
void semantic_set_scope( tree_t* t, scope_t* s );
bool semantic_verify_args( tree_t* args, tree_t* expected_args );
int semantic_type_of( tree_t* t );
bool semantic_check_has_return( tree_t* t, char* return_name );
bool semantic_check_global_update( tree_t* t, scope_t* top_scope, char* return_name );

#endif