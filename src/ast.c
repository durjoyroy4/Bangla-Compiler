
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Allocate memory for a new AST node
static ASTNode *ast_create(NodeType type) {
    ASTNode *node = malloc(sizeof(ASTNode));

    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    memset(node, 0, sizeof(ASTNode));
    node->type = type;

    return node;
}

// Program node
ASTNode *ast_new_program(ASTNode *statements) {
    ASTNode *node = ast_create(NODE_PROGRAM);
    node->program.statements = statements;
    return node;
}

// Block node
ASTNode *ast_new_block(ASTNode *statements) {
    ASTNode *node = ast_create(NODE_BLOCK);
    node->block.statements = statements;
    return node;
}

// Variable declaration
ASTNode *ast_new_var_decl(char *name, ASTNode *initializer) {
    ASTNode *node = ast_create(NODE_VAR_DECL);

    node->var_decl.name = strdup(name);
    node->var_decl.initializer = initializer;

    return node;
}

// Assignment
ASTNode *ast_new_assign(char *name, ASTNode *value) {
    ASTNode *node = ast_create(NODE_ASSIGN);

    node->assign.name = strdup(name);
    node->assign.value = value;

    return node;
}

// Print statement
ASTNode *ast_new_print(ASTNode *expression) {
    ASTNode *node = ast_create(NODE_PRINT);
    node->print.expression = expression;
    return node;
}

// Input statement
ASTNode *ast_new_input(char *name) {
    ASTNode *node = ast_create(NODE_INPUT);
    node->input.name = strdup(name);
    return node;
}

// If statement
ASTNode *ast_new_if(
    ASTNode *condition,
    ASTNode *then_branch,
    ASTNode *else_branch
) {
    ASTNode *node = ast_create(NODE_IF);

    node->if_stmt.condition = condition;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.else_branch = else_branch;

    return node;
}

// While loop
ASTNode *ast_new_while(ASTNode *condition, ASTNode *body) {
    ASTNode *node = ast_create(NODE_WHILE);

    node->while_stmt.condition = condition;
    node->while_stmt.body = body;

    return node;
}

// Expression statement
ASTNode *ast_new_expr_stmt(ASTNode *expression) {
    ASTNode *node = ast_create(NODE_EXPR_STMT);
    node->expr_stmt.expression = expression;
    return node;
}

// Integer value
ASTNode *ast_new_int(int value) {
    ASTNode *node = ast_create(NODE_INT);
    node->int_value.value = value;
    return node;
}

// Floating-point value
ASTNode *ast_new_float(double value) {
    ASTNode *node = ast_create(NODE_FLOAT);
    node->float_value.value = value;
    return node;
}

// Identifier
ASTNode *ast_new_identifier(char *name) {
    ASTNode *node = ast_create(NODE_IDENTIFIER);
    node->identifier.name = strdup(name);
    return node;
}

// Binary expression
ASTNode *ast_new_binary(
    Operator op,
    ASTNode *left,
    ASTNode *right
) {
    ASTNode *node = ast_create(NODE_BINARY);

    node->binary.op = op;
    node->binary.left = left;
    node->binary.right = right;

    return node;
}

// Unary expression
ASTNode *ast_new_unary(Operator op, ASTNode *operand) {
    ASTNode *node = ast_create(NODE_UNARY);

    node->unary.op = op;
    node->unary.operand = operand;

    return node;
}

// Append a node to a statement list
ASTNode *ast_append(ASTNode *list, ASTNode *node) {
    if (node == NULL) {
        return list;
    }

    if (list == NULL) {
        return node;
    }

    ASTNode *current = list;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = node;

    return list;
}
