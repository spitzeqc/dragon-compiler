#ifndef LIST_H
#define LIST_H

typedef struct list_s {
	char* name;
	struct list_s *next;
} list_t;

list_t* list_new( char* v );
void list_free( list_t* l );

list_t* list_insert( list_t* l, char* v );
list_t* list_search( list_t* l, char* v );

#endif
