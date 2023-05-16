#ifndef TREE_H
#define TREE_H

#include "hashmap.h"

typedef struct tree_s {
	int type; /* */
	int rank;
	union{
		int ival;   /* INUM */
		float rval; /* RNUM */
		bucket_node_t* sval; /* ID */
		char* str_val; /* STR_VAL */
		int opval;  /* ADDOP, RELOP, MULOP */
		int typeval; /* INTEGRAL, RATIONAL */
	} attribute; /* */

	struct tree_s* left;
	struct tree_s* right;
} tree_t;

tree_t* make_tree( int v, tree_t* l, tree_t* r );
tree_t* make_id( bucket_node_t* b );
tree_t* make_inum(int i);
tree_t* make_rnum(float r);
tree_t* make_str_val(char* s);

void tree_print( const tree_t* t );
void tree_calculate_rank( tree_t* t, bool is_left );

#endif
