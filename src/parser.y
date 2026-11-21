
%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

int yylex(void);
void yyerror(const char *message);

// Root of the Abstract Syntax Tree
ASTNode *ast_root = NULL;
%}

%code requires {
    #include "ast.h"
}

/* Semantic value types */
%union {
    int int_value;
    double float_value;
    char *string;
    ASTNode *node;
}

/* Keywords */
%token DHORO
%token DEKHAO
%token SHUNO
%token JODI
%token NOYTO
%token JOTOKKHON

/* Operators */
%token ASSIGN
%token PLUS MINUS MULTIPLY DIVIDE MODULO
%token EQ NE LT GT LE GE
%token AND OR NOT

/* Punctuation */
%token SEMICOLON
%token LPAREN RPAREN
%token LBRACE RBRACE

/* Values */
%token <int_value> INT_LITERAL
%token <float_value> FLOAT_LITERAL
%token <string> IDENTIFIER

%token INVALID

/* AST node types */
%type <node> program
%type <node> statement_list
%type <node> statement
%type <node> block
%type <node> expression

/* Operator precedence */
%nonassoc LOWER_THAN_NOYTO
%nonassoc NOYTO

%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT UMINUS

%start program

%%

/* Main program */

program:
    statement_list
    {
        ast_root = ast_new_program($1);
        $$ = ast_root;
    }
;

/* List of statements */

statement_list:
    %empty
    {
        $$ = NULL;
    }
    |
    statement_list statement
    {
        $$ = ast_append($1, $2);
    }
;

/* Statements */

statement:
    DHORO IDENTIFIER ASSIGN expression SEMICOLON
    {
        $$ = ast_new_var_decl($2, $4);
        free($2);
    }
    |
    IDENTIFIER ASSIGN expression SEMICOLON
    {
        $$ = ast_new_assign($1, $3);
        free($1);
    }
    |
    DEKHAO expression SEMICOLON
    {
        $$ = ast_new_print($2);
    }
    |
    SHUNO IDENTIFIER SEMICOLON
    {
        $$ = ast_new_input($2);
        free($2);
    }
    |
    JODI LPAREN expression RPAREN statement
    %prec LOWER_THAN_NOYTO
    {
        $$ = ast_new_if($3, $5, NULL);
    }
    |
    JODI LPAREN expression RPAREN statement
    NOYTO statement
    {
        $$ = ast_new_if($3, $5, $7);
    }
    |
    JOTOKKHON LPAREN expression RPAREN statement
    {
        $$ = ast_new_while($3, $5);
    }
    |
    block
    {
        $$ = $1;
    }
    |
    expression SEMICOLON
    {
        $$ = ast_new_expr_stmt($1);
    }
;

/* Code block */

block:
    LBRACE statement_list RBRACE
    {
        $$ = ast_new_block($2);
    }
;

/* Expressions */

expression:
    INT_LITERAL
    {
        $$ = ast_new_int($1);
    }
    |
    FLOAT_LITERAL
    {
        $$ = ast_new_float($1);
    }
    |
    IDENTIFIER
    {
        $$ = ast_new_identifier($1);
        free($1);
    }
    |
    LPAREN expression RPAREN
    {
        $$ = $2;
    }
    |
    expression PLUS expression
    {
        $$ = ast_new_binary(OP_ADD, $1, $3);
    }
    |
    expression MINUS expression
    {
        $$ = ast_new_binary(OP_SUB, $1, $3);
    }
    |
    expression MULTIPLY expression
    {
        $$ = ast_new_binary(OP_MUL, $1, $3);
    }
    |
    expression DIVIDE expression
    {
        $$ = ast_new_binary(OP_DIV, $1, $3);
    }
    |
    expression MODULO expression
    {
        $$ = ast_new_binary(OP_MOD, $1, $3);
    }
    |
    expression EQ expression
    {
        $$ = ast_new_binary(OP_EQ, $1, $3);
    }
    |
    expression NE expression
    {
        $$ = ast_new_binary(OP_NE, $1, $3);
    }
    |
    expression LT expression
    {
        $$ = ast_new_binary(OP_LT, $1, $3);
    }
    |
    expression GT expression
    {
        $$ = ast_new_binary(OP_GT, $1, $3);
    }
    |
    expression LE expression
    {
        $$ = ast_new_binary(OP_LE, $1, $3);
    }
    |
    expression GE expression
    {
        $$ = ast_new_binary(OP_GE, $1, $3);
    }
    |
    expression AND expression
    {
        $$ = ast_new_binary(OP_AND, $1, $3);
    }
    |
    expression OR expression
    {
        $$ = ast_new_binary(OP_OR, $1, $3);
    }
    |
    NOT expression
    {
        $$ = ast_new_unary(OP_NOT, $2);
    }
    |
    MINUS expression %prec UMINUS
    {
        $$ = ast_new_unary(OP_NEG, $2);
    }
;

%%

/* Syntax error handler */

void yyerror(const char *message) {
    fprintf(stderr, "Syntax Error: %s\n", message);
}
