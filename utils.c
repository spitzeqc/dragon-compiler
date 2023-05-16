#include "utils.h"

#include <stdlib.h>
#include <assert.h>

#include "structures/queue.h"
#include "structures/stack.h"
#include "structures/tree.h"

stack_t* utils_queue_to_stack( queue_t* q ) {
	stack_t* ret = NULL;
	queue_t* current_q_node = q;

	do
	{
		current_q_node = current_q_node->prev;
		stack_push( &ret, current_q_node->value );
	} while(current_q_node != q);	

	return ret;
}

queue_t* utils_stack_to_queue( stack_t* s ) {
	queue_t* ret = NULL;
	stack_t* current_s_node = s;

	while( s != NULL ) {
		queue_push( &ret, stack_top(current_s_node) );
		current_s_node = current_s_node->next;
	}

	return ret;
}

/* convert a parameter_list tree to a queue and free each tree node (does not free value) */
queue_t* utils_param_tree_to_args_queue( tree_t* args_tree ) {
	stack_t* temp = NULL;
	tree_t* temp2 = NULL;
	tree_t* current_param_node = args_tree;	
	tree_t* current_id_node;

	/* build argument stack */
	while( current_param_node != NULL ) {
		current_id_node = current_param_node->right;
		while(current_id_node != NULL) {
			stack_push( &temp, current_id_node->right->attribute.sval );

			free(current_id_node->right);
			temp2 = current_id_node->left;
			free(current_id_node);

			current_id_node = temp2;
		}

		free( current_param_node->right );
		temp2 = current_param_node->left;
		free( current_param_node );

		current_param_node = temp2;
	}

	queue_t* ret = utils_stack_to_queue( temp ); /* convert stack to queue */

	stack_free(&temp);
	return ret;
}