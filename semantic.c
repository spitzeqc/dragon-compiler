#include "semantic.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "structures/tree.h"

#include "scope.h"
#include "dragon.tab.h"

/* verify identifier is in global scope */
bucket_node_t* semantic_lookup( scope_t* top_scope, char* name ) {
	bucket_node_t* p = scope_global_search( top_scope, name );
	if( p == NULL ) {
		fprintf(stderr, "ERROR: name(%s) used but never defined\n", name);
		exit(1);
	}
	return p;
}

/* verify identifier does not already exist in current scope */
bucket_node_t* semantic_scope_insert( scope_t* top_scope, char* name ) {
	bucket_node_t* p = scope_search( top_scope, name );
	if( p != NULL ) {
		fprintf(stderr, "ERROR: name(%s) has already been defined\n", p->name);
		exit(1);
	}

	return scope_insert( top_scope, name );
}

/* verify identifier is in local scope */
bool semantic_is_local( scope_t* top_scope, char* name ) {
	semantic_lookup( top_scope, name ); /* see if entry exists at all */
	bucket_node_t* p = scope_search( top_scope, name ); /* entry exists, is it local? */

	return p!=NULL;
}

/* set types for all variables in an identifier list */
void semantic_set_types( tree_t* id_list, int type_val ) {
	if( id_list == NULL ) return;

	tree_t *left, *right;
	left = id_list->left;
	right = id_list->right;

	if( left == NULL && right == NULL ) { /* last node */
		((id_list->attribute).sval)->type = type_val;
		fprintf(stderr, "[Setting %s to type %d (final node)]\n", ((id_list->attribute).sval)->name, ((id_list->attribute).sval)->type);
	}
	else if( right != NULL ) { /* set type of right child (ID) */
		((right->attribute).sval)->type = type_val;
		fprintf(stderr, "[Setting %s to type %d]\n", ((right->attribute).sval)->name, ((right->attribute).sval)->type);
	} else {
		fprintf(stderr, "[Right child of %s is NULL]", id_list->attribute.sval->name);
	}
	semantic_set_types( left, type_val );
}

/* set class for an identifier list */
void semantic_set_classes( tree_t* id_list, int class_val ) {
	if( id_list == NULL ) return;

	tree_t *left, *right;
	left = id_list->left;
	right = id_list->right;

	if( left == NULL && right == NULL ) { /* last node */
		((id_list->attribute).sval)->class = class_val;
		fprintf(stderr, "[Setting %s to class %d (final node)]\n", ((id_list->attribute).sval)->name, ((id_list->attribute).sval)->class);
	}
	else if( right != NULL ) { /* set type of right child (ID) */
		((right->attribute).sval)->class = class_val;
		fprintf(stderr, "[Setting %s to class %d]\n", ((right->attribute).sval)->name, ((right->attribute).sval)->class);
	} else {
		fprintf(stderr, "[Right child of %s is NULL]", id_list->attribute.sval->name);
	}
	semantic_set_types( left, class_val );
}

/* set scope for a function/procedure */
void semantic_set_scope( tree_t* t, scope_t* s ) {
	fprintf(stderr, "Setting scope\n");
	t->attribute.sval->scope = s;
}

/* set args for a identifier */
void semantic_set_args( tree_t* id, tree_t* args ) {
	if( id == NULL ) return;
	fprintf(stderr, "Setting args\n");
	((id->attribute).sval)->args = args;
}

/* get the "type" of a tree */
int semantic_type_of( tree_t* t ) {
	if(t == NULL) return TYPE_ERROR;
	int left_type, right_type;

	switch( t->type ) {
	case ID:
		return t->attribute.sval->type;
	case INTEGRAL:
	case INUM:
		return INTEGRAL;
	case RATIONAL:
	case RNUM:
		return RATIONAL;
	case STRING:
	case STR_VAL:
		return STRING;
	case MULOP:
	case ADDOP:
		left_type = semantic_type_of( t->left );
		right_type = semantic_type_of( t->right );

		if( left_type != right_type ) {
			fprintf(stderr, "ERROR: type mismatch in %s, recieved %d and %d\n", ((t->type) == ADDOP ? "ADDOP" : "MULOP"), left_type, right_type );
			exit(1);
		}

		return left_type;
	case RELOP:
	case AND:
	case OR:
		return BOOL;
	case ARRAY_INTEGRAL:
		return INTEGRAL;
	case ARRAY_RATIONAL:
		return RATIONAL;
	case ARRAY_STRING:
		return STRING;
	case ARRAY_ACCESS:
		int array_type = t->left->attribute.sval->type;
		if(array_type == ARRAY_INTEGRAL)	return INTEGRAL;
		else if(array_type == ARRAY_RATIONAL)	return RATIONAL;
		else if(array_type == ARRAY_STRING)	return STRING;
		return array_type;
	case FUNCTION_CALL:
		return semantic_type_of( t->left );
	default:
		return TYPE_ERROR;
	}
}

/* check if function has a return statement */
bool semantic_check_has_return( tree_t* t, char* return_name ) {
	if(t == NULL) return false;

	tree_t* current_node = t;
	tree_t* previous_node = NULL;

	while(1) {
		if(current_node == NULL) break;
		switch (current_node->type) {
			case SUBPROGRAM_DECLARE:
				previous_node = current_node->right->right;
				current_node = current_node->right->right->right;
				break;
			case COMPOUND_STATEMENT:
				previous_node = current_node;
				current_node = current_node->left;
				break;
			case STATEMENT_LIST:
				previous_node = current_node;
				current_node = current_node->right;
				break;
			case ASSIGNOP:
				if( current_node->left->type == ID && strcmp( current_node->left->attribute.sval->name, return_name ) == 0) return true;
				current_node = previous_node->left;
				break;
			case IF:
				current_node = current_node->right;
				break;
			case THEN:
				if( semantic_check_has_return( current_node->right, return_name ) || semantic_check_has_return( current_node->left, return_name ) ) return true;

				current_node = previous_node->left;
				break;
			case PROCEDURE_CALL:
				current_node = previous_node->left;
				break;
			default:
				fprintf(stderr, "Return check unknown node type %d, failing test\n", current_node->type);
				return false;
		}
	}
	return false;
}

/* checks if function updates global variable (that is not the function name) */
bool semantic_check_global_update( tree_t* t, scope_t* top_scope, char* return_name ) {
	if(t == NULL) return false;

	tree_t* current_node = t;
	tree_t* previous_node = NULL;

	while(1) {
		if(current_node == NULL) break;
		switch (current_node->type) {
			case SUBPROGRAM_DECLARE:
				previous_node = current_node->right->right;
				current_node = current_node->right->right->right;
				break;
			case COMPOUND_STATEMENT:
				previous_node = current_node;
				current_node = current_node->left;
				break;
			case STATEMENT_LIST:
				previous_node = current_node;
				current_node = current_node->right;
				break;
			case ASSIGNOP:
				if( !semantic_is_local( top_scope, current_node->left->attribute.sval->name ) && !(strcmp( current_node->left->attribute.sval->name, return_name ) == 0) ) return true;
				current_node = previous_node;
				if(current_node != NULL) current_node = current_node->left;
				break;
			case IF:
				current_node = current_node->right;
				break;
			case THEN:
				if( semantic_check_global_update( current_node->right, top_scope, return_name ) || semantic_check_global_update( current_node->left, top_scope, return_name ) ) return true;

				current_node = previous_node->left;
				break;
			case PROCEDURE_CALL:
				current_node = previous_node->left;
				break;
			default:
				fprintf(stderr, "Update check unknown node type %d, failing test\n", current_node->type);
				return false;
		}
	}
	return false;
}

/* verify number and type of args match */
bool semantic_verify_args( tree_t* args, tree_t* expected_args ) {
	fprintf(stderr, "Args tree: ");
	tree_print(args);

	fprintf(stderr, "Expected args tree: ");
	tree_print(expected_args);

	if(args == NULL && expected_args == NULL) return true; /* no args provided and no args expected */
	if(args == NULL || expected_args == NULL) return false; /* either no args expected or no args provided, one of these is incorrect */

	tree_t* current_arg = args;
	tree_t* current_param = expected_args;
	tree_t* current_expected_arg = NULL;

	int provided_type, expected_type;

	while( current_param != NULL ) {
		current_expected_arg = current_param->right;
		while( current_expected_arg != NULL ) {
			if(current_arg == NULL) return false; /* end of provided, not end of expected */
			provided_type = semantic_type_of( current_arg->right );
			expected_type = semantic_type_of( current_expected_arg->right );

			fprintf(stderr, "\nExpected: %d, Provided: %d\n", expected_type, provided_type);
			if( provided_type != expected_type ) return false; /* types do not match */			

			current_expected_arg = current_expected_arg->left;
			current_arg = current_arg->left;
		}
		current_param = current_param->left;
	}
	if( current_arg != NULL ) return false; /* we should be NULL if all parameters lined up */

	return true;
}
