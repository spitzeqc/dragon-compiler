#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "structures/stack.h"
#include "structures/queue.h"
#include "structures/tree.h"
#include "structures/hashmap.h"

#include "scope.h"

#include "dragon.tab.h"

#define max(a, b) a > b ? a : b

#define REGISTER_COUNT 4

#define BASE_OFFSET 12

char* base_register_stack[] = { "edx", "ecx", "ebx", "eax" }; /* what the initial stack looks like */

/* create preamble for file */
void generate_preamble(char* file_name) {
	fprintf(stdout, "        .file   \"%s\"\n", file_name);
	fprintf(stdout, "        .text\n");
}
/* create postamble for file */
void generate_postamble() {
	fprintf(stdout, "        .ident  \"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)\"\n");
	fprintf(stdout, "        .section        .note.GNU-stack,\"\",@progbits\n");
}
/* generate header for a function */
void generate_header(char* name) {
	fprintf(stdout, "        .globl  %s\n", name);
	fprintf(stdout, "        .type   %s, @function\n", name);
}
/* generate footer for a function */
void generate_footer(char* name) {
	fprintf(stdout, "        .size   %s, .-%s\n", name, name);
}

/* call printf */
void generate_print_code(int* offsets, int offsets_count, char* format) {
/*
movl -12(%ebp), %eax ; first arg loaded into eax
movl %eax, 4(%esp)   ; eax loaded into esp+4 to pass to printf
movl $.LC0 (%esp)    ; format string (.LC0) loaded into esp to pass to scanf
call printf
*/
	const int esp_offset_size = 4;
	int i;
	for(i=0; i<offsets_count; ++i) {
		fprintf(stdout, "movl %d(%%ebp), %%eax\n", offsets[i]);
		fprintf(stdout, "movl %%eax %d(%%esp)\n", (i+1)*esp_offset_size);
	}
	fprintf(stdout, "movl $.LC0 (%%esp)\n"); // replace hardcoded .LC0 with format
	fprintf(stdout, "call printf\n");
}

/* call scanf */
void generate_scan_code(int* offsets, int offsets_count, char* format) {
/* 
leal -12(%ebp) %eax ; address of first arg loaded into eax
movl $eax, 4(%esp)  ; eax loaded into esp+4 to pass to scanf
movl $.LC0, (%esp)  ; format string (.LC0) loaded into esp to pass to scanf
call scanf
*/
	const int esp_offset_size = 4;
	int i;
	for(i=0; i<offsets_count; ++i) {
		fprintf(stdout, "leal %d(%%ebp), %%eax\n", offsets[i]);
		fprintf(stdout, "movl %%eax %d(%%esp)\n", (i+1)*esp_offset_size);
	}
	fprintf(stdout, "movl $.LC0 (%%esp)\n"); // replace hardcoded .LC0 with format
	fprintf(stdout, "call scanf\n");
}


void aux_generate_code( tree_t* t, stack_t* registers );
void generate_code( tree_t* t ) {
	fprintf(stderr, "\n");
	tree_print(t);

	fprintf(stderr, "\n\nAMS CODE\n");
	stack_t* rstack = NULL; /* stack of available registers */
	int i;
	for(i=0; i<REGISTER_COUNT; ++i) {
		int* ptr = malloc( sizeof(int*) );
		(*ptr) = i;
		stack_push( &rstack, ptr );
	}

	generate_preamble("test_file_name"); /* TODO: replace hardcoded value */

	aux_generate_code( t, rstack );

	generate_postamble();

	stack_free(&rstack);
}

/* determine what operation we are currently working on */
void aux_generate_code( tree_t* t, stack_t* registers )
{
	if(t == NULL) return;

	tree_t* temp_tree = NULL;
	int current_offset;

	switch(t->type) {
		case DEF: /* beginning of program */
			current_offset = BASE_OFFSET;
			generate_header(t->left->attribute.sval->name);
			fprintf(stdout, "%s:\n", t->left->attribute.sval->name);
			fprintf(stdout, "	pushl	%%ebp\n" );
			fprintf(stdout, "	movl	%%esp, %%ebp\n" );
			fprintf(stdout, "	andl	$-16, %%esp\n" );
			fprintf(stdout, "	subl	$%d, %%esp\n", 32 );
			aux_generate_code(t->right, registers); /* allocate variables */
			aux_generate_code(t->right->right, registers); /* generate function code */
			fprintf(stdout, "	popl	%%ebp\n" );
			fprintf(stdout, "	ret\n" );
			generate_footer(t->left->attribute.sval->name);
			aux_generate_code(t->right->right->left, registers); /* generate subprograms */
			break;
		case SUBPROGRAM_DECLARE: /* function/procedure definition */
			current_offset = BASE_OFFSET;
			generate_header(t->left->left->attribute.sval->name);
			fprintf(stdout, "%s:\n", t->left->left->attribute.sval->name);
			fprintf(stdout, "	pushl	%%ebp\n" );
			fprintf(stdout, "	movl	%%esp, %%ebp\n" );
			aux_generate_code(t->right, registers); /* allocate variables */
			aux_generate_code(t->right->right, registers); /* generate function code */
			fprintf(stdout, "	popl	%%ebp\n" );
			fprintf(stdout, "	ret\n" );
			generate_footer(t->left->left->attribute.sval->name);
			aux_generate_code(t->right->right->left, registers); /* generate subprograms */
			break;
		case PROGRAM_DECLARATION:
			aux_generate_code(t->left, registers);
			break;
		case PROGRAM_DECLARATION_2:
			aux_generate_code(t->right, registers);
			break;
		case COMPOUND_STATEMENT:
			aux_generate_code(t->left, registers);
			break;
		case STATEMENT_LIST:
			aux_generate_code(t->left, registers);
			aux_generate_code(t->right, registers);
			break;
		case SUBPROGRAM_DECLARE_LIST:
			aux_generate_code(t->left, registers);
			aux_generate_code(t->right, registers);
			break;
		case DECLARATIONS:
			aux_generate_code(t->left, registers);
			temp_tree = t->right;
			while(temp_tree != NULL) {
				(temp_tree->right)->attribute.sval->offset = current_offset;
				current_offset += OFFSET_SIZE;
				temp_tree = temp_tree->left;
			}
			break;
		case ASSIGNOP:
			
			break;
/*		case ID:
			break;
		case LIST:
			aux_generate_code(t->left, registers);
			aux_generate_code(t->right, registers);
			break;
		case SUB_PROGRAM:
			aux_generate_code(t->left, registers);
			aux_generate_code(t->right, registers);
			break;
*/
		case PROCEDURE_CALL:
		case FUNCTION_CALL:
			/* load arguments onto stack */
			
			fprintf(stdout, "	call	%s\n", t->left->attribute.sval->name); /* call function */
			break;
		default:
			fprintf( stderr, "Unknown operation encountered: %d\n", t->type );
			exit(1);
			break;
	}
}

/* ARITHMETIC */
/* swaps top two elements on the stack */
void swap_stack( stack_t* s ) {
	void* v1 = stack_pop(&s);
	void* v2 = stack_pop(&s);

	stack_push(&s, v1);
	stack_push(&s, v2);
}
/* get opcode for arithmetic */
char* getop(int type) {
	switch (type) {
	case ADDOP:
		return "ADD";
	case MULOP:
		return "MUL";
	default:
		return "UNK";
	}
}

/* generate arithmetic code */
/* TODO: implement case 4 */
void arithmetic( tree_t* n, stack_t* rstack ) {
	/* generate rank for nodes in arithmetic */
	tree_calculate_rank( n, false );

	/* Case 0 */
	if(n->left == NULL && n->right == NULL && n->rank == 1) {
		fprintf( stderr, "MOV <name>, R%d", *(int*)stack_top(rstack) );
		return;
	}

	/* Case 1 */
	if( (n->right)->rank == 0 ) {
		arithmetic( n->left, rstack );
		fprintf( stderr, "%s, R%d", getop(n->type), *(int*)stack_top(rstack) );
	}
	/* Case 2 */
	/* does the < REG_COUNT need to be < remaining registers on stack? */
	else if( (1<=(n->left)->rank && (n->left)->rank < (n->right)->rank) && (n->left)->rank < REGISTER_COUNT ) {
		swap_stack(rstack);
		arithmetic(n->right, rstack);
		void* r = stack_pop(&rstack);
		arithmetic(n->left, rstack);
		fprintf( stderr, "%s R%d, R%d", getop(n->type), *(int*)r, *(int*)stack_top(rstack) );
		stack_push( &rstack, r );
		swap_stack(rstack);
	}
	/* Case 3 */
	else if( (1<=(n->right)->rank && (n->right)->rank <= (n->left)->rank) && (n->right)->rank < REGISTER_COUNT ) {
		arithmetic(n->left, rstack);
		void* r = stack_pop(&rstack);
		arithmetic(n->right, rstack);
		fprintf( stderr, "%s R%d, R%d", getop(n->type), *(int*)stack_top(rstack), *(int*)r );
		stack_push(&rstack, r);
	}
	/* Case 4 */
	else {
		fprintf(stderr, "OUT OF REGISTERS\n");
		exit(1);
		/*gencode(n->right, rstack, tstack);
		int t = stack_pop(&tstack);
		//fprintf( stderr, CASE4A, t, stack_top(rstack) );
		fprintf( stderr, CASE4A, t); 
		gencode(n->left, rstack, tstack);
		stack_push(&tstack, t);
		//fprintf( stderr, CASE4B, t, getop(n->type), stack_top(rstack), stack_top(rstack) );
		fprintf( stderr, CASE4B, getop(n->type), stack_top(rstack), stack_top(rstack));*/
	}
}