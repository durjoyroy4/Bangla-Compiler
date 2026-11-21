
#ifndef AST_H
#define AST_H

typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,

    // Statements
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_PRINT,
    NODE_INPUT,
    NODE_IF,
    NODE_WHILE,
    NODE_EXPR_STMT,

    // Expressions
    NODE_INT,
    NODE_FLOAT,
    NODE_IDENTIFIER,
    NODE_BINARY,
    NODE_UNARY
} NodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,

    OP_EQ,
    OP_NE,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,

    OP_AND,
    OP_OR,
    OP_NOT,
    OP_NEG
} Operator;

typedef struct ASTNode ASTNode;

struct ASTNode {
    NodeType type;

    // Next statement in a list
    ASTNode *next;

    union {
        struct {
            ASTNode *statements;
        } program;

        struct {
            ASTNode *statements;
        } block;

        struct {
            char *name;
            ASTNode *initializer;
        } var_decl;

        struct {
            char *name;
            ASTNode *value;
        } assign;

        struct {
            ASTNode *expression;
        } print;

        struct {
            char *name;
        } input;

        struct {
            ASTNode *condition;
            ASTNode *then_branch;
            ASTNode *else_branch;
        } if_stmt;

        struct {
            ASTNode *condition;
            ASTNode *body;
        } while_stmt;

        struct {
            ASTNode *expression;
        } expr_stmt;

        struct {
            int value;
        } int_value;

        struct {
            double value;
        } float_value;

        struct {
            char *name;
        } identifier;

        struct {
            Operator op;
            ASTNode *left;
            ASTNode *right;
        } binary;

        struct {
            Operator op;
            ASTNode *operand;
        } unary;
    };
};

// AST creation functions
ASTNode *ast_new_program(ASTNode *statements);
ASTNode *ast_new_block(ASTNode *statements);

ASTNode *ast_new_var_decl(char *name, ASTNode *initializer);
ASTNode *ast_new_assign(char *name, ASTNode *value);
ASTNode *ast_new_print(ASTNode *expression);
ASTNode *ast_new_input(char *name);

ASTNode *ast_new_if(
    ASTNode *condition,
    ASTNode *then_branch,
    ASTNode *else_branch
);

ASTNode *ast_new_while(
    ASTNode *condition,
    ASTNode *body
);

ASTNode *ast_new_expr_stmt(ASTNode *expression);

ASTNode *ast_new_int(int value);
ASTNode *ast_new_float(double value);
ASTNode *ast_new_identifier(char *name);

ASTNode *ast_new_binary(
    Operator op,
    ASTNode *left,
    ASTNode *right
);

ASTNode *ast_new_unary(
    Operator op,
    ASTNode *operand
);

// Statement list helper
ASTNode *ast_append(ASTNode *list, ASTNode *node);

#endif
