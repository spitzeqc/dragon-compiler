%{
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "structures/tree.h"

#include "scope.h"
#include "semantic.h"
#include "codegen.h"

int yylex(void);
int yyerror(char* s);

scope_t* top_scope;
bucket_node_t* id_ptr;
%}

%union { /* token attributes */
	int   ival;  /* INUM */
	float rval;  /* RNUM */
	char  *sval; /* ID, STRING */
	int   opval; /* MULOP, ADDOP, RELOP */
	int   typeval; /* INTEGRAL, RATIONAL */

	tree_t *tval; /* tree attribute */
}
%token DEF
%token <sval> ID
%token VAR
%token ARRAY OF
%token INTEGRAL RATIONAL
%token <ival> INUM
%token <rval> RNUM
%token BBEGIN END
%token ASSIGNOP
%token IF THEN ELSE
%token WHILE DO
%token REPEAT UNTIL
%token FOR
%token NOT
%token FUNC PROC

%token STRING
%token <sval> STR_VAL
%token PRINT
%token SCAN
%type <tval> str_expression_list
%type <tval> str_expression
%type <tval> built_in_function

%token <opval> RELOP
%token LT LE GT GE EQ NEQ
%token <opval> ADDOP
%token PLUS MINUS OR
%token <opval> MULOP
%token STAR SLASH DIV MOD AND

%token FUNCTION_CALL PROCEDURE_CALL
%token ARRAY_ACCESS 
%token LIST STATEMENT_LIST 
%token COMPOUND_STATEMENT
%token RANGE
%token FOR_PARAM

%token TYPE_ERROR
%token BOOL
%token ARRAY_INTEGRAL
%token ARRAY_RATIONAL
%token ARRAY_STRING

%type <tval> factor
%type <tval> expression_list
%type <tval> expression
%type <tval> simple_expression
%type <tval> term
%type <tval> var
%type <tval> procedure_statement
%type <tval> compound_statement
%type <tval> matched_statement
%type <tval> unmatched_statement
%type <tval> statement
%type <tval> optional_statements
%type <tval> statement_list
%type <tval> range
%type <ival> type
%type <typeval> standard_type

%type <tval> subprogram_head

%type <tval> identifier_list

%type <tval> program
%type <tval> declarations
%token DECLARATIONS
%type <tval> subprogram_declarations
%token SUB_PROGRAM

%type <tval> subprogram_declaration
%token SUBPROGRAM_DECLARE
%token SUBPROGRAM_DECLARE_LIST

%type <tval> program_declaration
%token PROGRAM_DECLARATION
%token PROGRAM_DECLARATION_2

%type <tval> args
%type <tval> parameter_list
%token PARAM_LIST

%%

program: DEF ID '(' identifier_list ')' ';'
	program_declaration
	'.'
	{ 
		/* ID */
		id_ptr = semantic_scope_insert( top_scope, $2 );
		tree_t* temp = make_id( id_ptr );
		semantic_set_types(temp, PROC);

		$$ = make_tree( DEF, temp, $7 );
		generate_code( $$ );
	}
	;

program_declaration: declarations
	subprogram_declarations
	compound_statement
	{
		$$ = make_tree( PROGRAM_DECLARATION, $1, make_tree( PROGRAM_DECLARATION_2, $2, $3 ) );
	}
	;

identifier_list: ID
	{
		id_ptr = semantic_scope_insert( top_scope, $1 );
		$$ = make_tree( LIST, NULL, make_id( id_ptr ) );
	}
	| identifier_list ',' ID
	{ 
		id_ptr = semantic_scope_insert( top_scope, $3 );
		$$ = make_tree( LIST, $1, make_id( id_ptr ) );
	}
	;

declarations: declarations VAR identifier_list ':' type ';'
	{
		semantic_set_types( $3, $5 ); 
		$$ = make_tree( DECLARATIONS, $1, $3 );
	}
	| /* empty */
	{ $$ = NULL; }
	;

type: standard_type
		{ $$ = $1; }
	| ARRAY '[' range ']' OF standard_type
		{ 
			if($6 == INTEGRAL)	$$ = ARRAY_INTEGRAL;
			else if($6 == RATIONAL)	$$ = ARRAY_RATIONAL;
			else if($6 == STRING)	$$ = ARRAY_STRING;
			else			$$ = TYPE_ERROR;
		}
	;

/*range: INUM '.' '.' INUM
	{ $$ = make_tree( RANGE, make_inum($1), make_inum($4) ); }
	;
*/
range: simple_expression '.' '.' simple_expression
	{
		int left_type = semantic_type_of($1);
		int right_type = semantic_type_of($4);
		if(left_type != right_type) {
			fprintf(stderr, "ERROR: lower and upper bounds on range must be of same type, recieved %d %d\n", left_type, right_type);
			exit(1);
		}
		if(left_type != INTEGRAL && left_type != RATIONAL) {
			fprintf(stderr, "ERROR: ranges can only be made of integrals or rationals, recieved %d\n", left_type);
			exit(1);
		}
		$$ = make_tree( RANGE, $1, $4 );
		$$->attribute.typeval = left_type;
	}

standard_type: INTEGRAL
		{ $$ = INTEGRAL; }
	| RATIONAL
		{ $$ = RATIONAL; }
	| STRING
		{ $$ = STRING; }
	;
	
subprogram_declarations: subprogram_declarations subprogram_declaration ';'
		{ $$ = make_tree(SUBPROGRAM_DECLARE_LIST, $1, $2); }
	| /* empty */ 
		{ $$ = NULL; }
	;
subprogram_declaration: 
	subprogram_head
	program_declaration
	{  
		$$ = make_tree( SUBPROGRAM_DECLARE, $1, $2 );
		if($$->left->type == FUNC && !semantic_check_has_return( $$, $$->left->left->attribute.sval->name )) { /* check if function has return statement */
			fprintf(stderr, "ERROR: function(%s) has no return statement\n", $$->left->left->attribute.sval->name);
			exit(1);
		}
		if($$->left->type == FUNC && semantic_check_global_update($$, top_scope, $$->left->left->attribute.sval->name )) { /* check if function updates any out of scope variables */
			fprintf(stderr, "ERROR: function(%s) updates non-local variable\n", $$->left->left->attribute.sval->name);
			exit(1);
		}
		if($$->left->type == PROC && semantic_check_has_return( $$, $$->left->left->attribute.sval->name )) { /* check if procedure has return statement */
			fprintf(stderr, "ERROR: procedure(%s) contains a return statement\n", $$->left->left->attribute.sval->name);
			exit(1);
		}
//		top_scope = scope_pop( top_scope ); /* leaving inner scope */
		scope_unlink( &top_scope );
	}
	;
subprogram_head: FUNC ID 
		{ 
			id_ptr = semantic_scope_insert( top_scope, $2 ); /* record function ID in current scope */
			top_scope = scope_push( top_scope ); /* create new scope */
		} 
		args ':' standard_type ';'
		{ 
			id_ptr = semantic_lookup( top_scope, $2 );
			tree_t* temp = make_id( id_ptr );
			semantic_set_types( temp, $6 );
			semantic_set_classes( temp, FUNC );
			semantic_set_args( temp, $4 );
			semantic_set_scope( temp, top_scope );
			$$ = make_tree( FUNC, temp, NULL );
		}
	| PROC ID 
		{ 
			id_ptr = semantic_scope_insert( top_scope, $2 ); /* record procedure ID in current scope */
			top_scope = scope_push( top_scope ); /* create new scope */
		}
		args ';'
		{
			id_ptr = semantic_lookup( top_scope, $2 );
			tree_t* temp = make_id( id_ptr );
			semantic_set_types( temp, PROC );
			semantic_set_classes( temp, PROC );
			semantic_set_args( temp, $4 );
			semantic_set_scope( temp, top_scope );
			$$ = make_tree( PROC, temp, NULL );
		}

args: '(' parameter_list ')'
	{ $$ = $2; }
	| /* empty */
	{ $$ = NULL; }
	;

parameter_list: identifier_list ':' type
	{ 
		semantic_set_types($1, $3); 
		$$ = make_tree(PARAM_LIST, NULL, $1);
	}
	| parameter_list ';' identifier_list ':' type
	{ 
		semantic_set_types($3, $5); 
		$$ = make_tree(PARAM_LIST, $1, $3);
	}
	;

compound_statement: 
	BBEGIN 
	optional_statements 
	END
	{ $$ = make_tree( COMPOUND_STATEMENT, $2, NULL ); }
	;

optional_statements: statement_list
		{ $$ = $1; }
	| /* empty */
		{ $$ = NULL; }
	;

statement_list: statement
		{ $$ = make_tree( STATEMENT_LIST, NULL, $1 ); }
	| statement_list ';' statement
		{ $$ = make_tree( STATEMENT_LIST, $1, $3 ); }
	;

statement: matched_statement
		{ $$ = $1; }
	| unmatched_statement
		{ $$ = $1; }
	;
matched_statement: IF expression THEN matched_statement ELSE matched_statement
		{ 
			if( semantic_type_of( $2 ) != BOOL ) {
				fprintf(stderr, "ERROR: if-statement expecting bool (relop)\n");
				exit(1);
			}
			$$ = make_tree( IF, $2, make_tree( THEN, $4, $6 ) ); 
		}
	| var ASSIGNOP expression
		{
			int var_type = semantic_type_of($1);
			/*if(var_type == ARRAY_INTEGRAL)	var_type = INTEGRAL;
			else if(var_type == ARRAY_RATIONAL)	var_type = RATIONAL;
			else if(var_type == ARRAY_STRING)	var_type = STRING;*/

			int assign_type = semantic_type_of($3);
			/*if(assign_type == ARRAY_INTEGRAL)	assign_type = INTEGRAL;
			else if(assign_type == ARRAY_RATIONAL)	assign_type = RATIONAL;
			else if(assign_type == ARRAY_STRING)	assign_type = STRING;*/
			if( assign_type == PROC ) {
				fprintf(stderr, "ERROR: unable to assign var(%s) to procedure\n", $1->attribute.sval->name);
			}
			else if( var_type != assign_type ) { /* check types match */
				fprintf(stderr, "ERROR: type mismatch in assignment statement %d %d\n", var_type, assign_type);
				exit(1);
			}

			$$ = make_tree( ASSIGNOP, $1, $3 ); 
		}
	| var ASSIGNOP STR_VAL
		{
			if( semantic_type_of($1) != STRING ) {
				fprintf(stderr, "ERROR: type mismatch in assignment statement %d %d\n", semantic_type_of($1), STRING);
				exit(1);
			}
			$$ = make_tree( ASSIGNOP, $1, make_str_val( $3 ) );
		}
	| procedure_statement
		{ $$ = $1; }
	| compound_statement
		{ $$ = $1; }
	| REPEAT statement UNTIL expression
		{ $$ = make_tree( REPEAT, $2, $4 ); }
	| built_in_function
		{ $$ = $1; }
	;

unmatched_statement: IF expression THEN statement
		{ 
			if( semantic_type_of( $2 ) != BOOL ) {
				fprintf(stderr, "ERROR: if-statement expecting bool (relop)\n");
				exit(1);
			}
			$$ = make_tree( IF, $2, make_tree( THEN, $4, NULL ) );
		}
	| IF expression THEN matched_statement ELSE unmatched_statement
		{ 
			if( semantic_type_of( $2 ) != BOOL ) {
				fprintf(stderr, "ERROR: if-statement expecting bool (relop)\n");
				exit(1);
			}
			$$ = make_tree( IF, $2, make_tree( THEN, $4, $6 ) );
		}
	| WHILE expression DO statement 
		{ 
			if( semantic_type_of( $2 ) != BOOL ) {
				fprintf(stderr, "ERROR: while-statement expecting bool (relop)\n");
				exit(1);
			}
			$$ = make_tree( WHILE, $2, $4 );
		}
	| FOR ID ASSIGNOP range DO statement
		{ 
			id_ptr = semantic_lookup( top_scope, $2 );
			$$ = make_tree( FOR, make_tree( FOR_PARAM, make_id( id_ptr ), $4), $6 ); 
		}
	;

var: ID
		{ $$ = make_id( semantic_lookup( top_scope, $1 ) ); }
	| ID '[' expression ']'
		{ 
			if( semantic_type_of($3) != INTEGRAL ) {
				fprintf(stderr, "ERROR: array indexing requires an integral, recieved %d\n", semantic_type_of($3));
				exit(1);
			}
			$$ = make_tree( ARRAY_ACCESS, make_id( semantic_lookup( top_scope, $1 ) ), $3 ); 
		}
	;

procedure_statement: ID
		{ $$ = make_id( semantic_lookup( top_scope, $1 ) ); }
	| ID '(' expression_list ')'
		{ 
			bucket_node_t* function = semantic_lookup( top_scope, $1 );
			scope_link( function->scope, &top_scope );
			if ( !semantic_verify_args( $3, (tree_t*)function->args ) ) {
				fprintf(stderr, "ERROR: invalid arguments passed to procedure(%s)\n", function->name);
				exit(1);
			}
			$$ = make_tree( PROCEDURE_CALL, make_id( semantic_lookup( top_scope, $1 ) ), $3 ); 
			scope_unlink( &top_scope );
		}
	;

expression_list: expression
		{ $$ = make_tree( LIST, NULL, $1 ); }
	| expression_list ',' expression
		{ $$ = make_tree( LIST, $1, $3 ); }
	;

expression: simple_expression
		{ $$ = $1; }
	| simple_expression RELOP simple_expression
		{ $$ = make_tree( RELOP, $1, $3 ); $$->attribute.opval = $2; }
	| expression AND simple_expression RELOP simple_expression
		{
			tree_t* temp = make_tree( RELOP, $1, $3 ); 
			temp->attribute.opval = $4;
			$$ = make_tree( AND, $1, temp );
		}
	| expression OR simple_expression RELOP simple_expression
		{
			tree_t* temp = make_tree( RELOP, $1, $3 ); 
			temp->attribute.opval = $4;
			$$ = make_tree( OR, $1, temp );
		}
	;


simple_expression: term
		{ $$ = $1; }
	| ADDOP term
		{ $$ = make_tree( ADDOP, $2, NULL ); $$->attribute.opval = $1;}
	| simple_expression ADDOP term
		{ $$ = make_tree( ADDOP, $1, $3 ); $$->attribute.opval = $2; }
	;

term: factor
		{ $$ = $1; }
	| term MULOP factor
		{ 
			$$ = make_tree( MULOP, $1, $3 );
			$$->attribute.opval = $2;
		}
	;

factor: var
		{ $$ = $1; }
	| ID '(' expression_list ')'
		{
			bucket_node_t* function = semantic_lookup( top_scope, $1 );
			/* relink function's scope */
			scope_link( function->scope, &top_scope );
			if ( !semantic_verify_args( $3, (tree_t*)function->args ) ) {
				fprintf(stderr, "ERROR: invalid arguments passed to function(%s)\n", function->name);
				exit(1);
			}
			$$ = make_tree( FUNCTION_CALL, make_id( function ), $3 ); 
			/* unlink function's scope */
			scope_unlink( &top_scope );
		}
	| INUM
		{ $$ = make_inum( $1 ); }
	| RNUM
		{ $$ = make_rnum( $1 ); }
	| '(' expression ')'
		{ $$ = $2; } 
	| NOT factor
		{ $$ = make_tree( NOT, $2, NULL ); }
	;

str_expression: expression
		{ $$ = $1; }
	| STR_VAL
		{ $$ = make_str_val( $1 ); }	
	;
str_expression_list: str_expression
		{ $$ = $1; }
	| str_expression_list ',' str_expression
		{ $$ = make_tree( LIST, $1, $3 ); }
	;

built_in_function: PRINT '(' STR_VAL ')'
		{ $$ = make_tree( PRINT, make_str_val( $3 ) , NULL ); }
	| PRINT '(' STR_VAL ',' str_expression_list ')'
		{ $$ = make_tree( PRINT, make_str_val( $3 ), $5 ); }
	| PRINT '(' ID ')'
		{
		if( semantic_type_of( make_id( semantic_lookup( top_scope, $3 ) ) ) != STRING ) {
			fprintf(stderr, "ERROR: name(%s) is not a string\n", $3);
			exit(1);
		}
		$$ = make_tree( PRINT, make_id( semantic_lookup( top_scope, $3 ) ), NULL ); 
		}
		
	| PRINT '(' ID ',' str_expression_list ')'
		{
		if( semantic_type_of( make_id( semantic_lookup( top_scope, $3 ) ) ) != STRING ) {
			fprintf(stderr, "ERROR: name(%s) is not a string\n", $3);
			exit(1);
		} 
		$$ = make_tree( PRINT, make_id( semantic_lookup( top_scope, $3 ) ), $5 ); 
		}
	| SCAN '(' STR_VAL ',' str_expression_list ')'
		{ $$ = make_tree( SCAN, make_str_val( $3 ), $5 ); }
	| SCAN '(' ID ',' str_expression_list ')'
		{ 
		if( semantic_type_of( make_id( semantic_lookup( top_scope, $3 ) ) ) != STRING ) {
			fprintf(stderr, "ERROR: name(%s) is not a string\n", $3);
			exit(1);
		}
		$$ = make_tree( SCAN, make_id( semantic_lookup( top_scope, $3 ) ), $5 );
		}
	;
%%

int main()
{
	top_scope = scope_new();
	id_ptr = NULL;
	yyparse();
}
