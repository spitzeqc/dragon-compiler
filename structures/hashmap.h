#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>

#define PRIME 211

typedef struct bucket_node_s {
	char* name;  /* identifier name */
	int class;   /* simple variable, function name, procecure name, array name, matrix name */
	int type;    /* INTEGRAL, RATIONAL, STRING, ARRAY, MATRIX: incomplete */
	
	/* additional information */
	/* ARRAY: lower and upper bounds */
	int lower_bound;
	int upper_bound;

	/* FUNC, PROC: sequence of expected types (which includes #args) */
	void* args;  /* arguments for a function/procedure */
	void* scope; /* scope of variables within a function/procedure */

	/* code generation: offest */
	int offset; 

	union {
		int ival;       /* INTEGRAL val */
		float rval;     /* RATIONAL val */
		char* sval;     /* STRING val */
		int* iaval;     /* INTEGRAL ARRAY val */
		float* raval;   /* RATIONAL ARRAY val */
	} value; /* current value identifier is holding, if a FUNC this is the final return value */
	struct bucket_node_s* next;
} bucket_node_t;

bucket_node_t** hashmap_new();
void hashmap_free( bucket_node_t** h );

bucket_node_t* hashmap_add_node( bucket_node_t** h, char* name );
bucket_node_t* hashmap_get_node( bucket_node_t** h, char* name );
bool hashmap_remove( bucket_node_t** h, char* name );
char* hashmap_get_value( bucket_node_t** h, char* name );

void hashmap_print( bucket_node_t** h );
#endif
