#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantic.h"

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_ERROR
} DataType;

typedef struct Symbol {
    char *name;
    DataType type;
    struct Symbol *next;
} Symbol;

typedef struct Scope {
    Symbol *symbols;
    struct Scope *parent;
} Scope;

static Scope *current_scope = NULL;
static int error_count = 0;

/* Create a new scope */
static void push_scope(void) {
    Scope *scope = malloc(sizeof(Scope));

    if (!scope) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    scope->symbols = NULL;
    scope->parent = current_scope;
    current_scope = scope;
}

/* Remove the current scope */
static void pop_scope(void) {
    if (!current_scope)
        return;

    Symbol *symbol = current_scope->symbols;

    while (symbol) {
        Symbol *next = symbol->next;
        free(symbol->name);
        free(symbol);
        symbol = next;
    }

    Scope *parent = current_scope->parent;
    free(current_scope);
    current_scope = parent;
}

/* Find a variable in current or parent scopes */
static Symbol *find_symbol(const char *name) {
    Scope *scope = current_scope;

    while (scope) {
        Symbol *symbol = scope->symbols;

        while (symbol) {
            if (strcmp(symbol->name, name) == 0)
                return symbol;

            symbol = symbol->next;
        }

        scope = scope->parent;
    }

    return NULL;
}

/* Check duplicate declaration in current scope */
static int already_declared(const char *name) {
    Symbol *symbol = current_scope->symbols;

    while (symbol) {
        if (strcmp(symbol->name, name) == 0)
            return 1;

        symbol = symbol->next;
    }

    return 0;
}

/* Add a variable to the symbol table */
static void add_symbol(const char *name, DataType type) {
    Symbol *symbol = malloc(sizeof(Symbol));

    if (!symbol) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    symbol->name = strdup(name);
    symbol->type = type;
    symbol->next = current_scope->symbols;

    current_scope->symbols = symbol;
}

static const char *type_name(DataType type) {
    if (type == TYPE_INT)
        return "int";

    if (type == TYPE_FLOAT)
        return "float";

    return "unknown";
}

static int is_numeric(DataType type) {
    return type == TYPE_INT || type == TYPE_FLOAT;
}

static DataType check_expression(ASTNode *node);
static void check_statement_list(ASTNode *node);
/* Analyze an expression */
static DataType check_expression(ASTNode *node) {
    if (!node)
        return TYPE_ERROR;

    switch (node->type) {

        case NODE_INT:
            return TYPE_INT;

        case NODE_FLOAT:
            return TYPE_FLOAT;

        case NODE_IDENTIFIER: {
            Symbol *symbol = find_symbol(node->identifier.name);

            if (!symbol) {
                fprintf(stderr,
                    "Semantic Error: Variable '%s' is not declared.\n",
                    node->identifier.name);

                error_count++;
                return TYPE_ERROR;
            }

            return symbol->type;
        }

        case NODE_BINARY: {
            DataType left = check_expression(node->binary.left);
            DataType right = check_expression(node->binary.right);

            if (!is_numeric(left) || !is_numeric(right))
                return TYPE_ERROR;

            Operator op = node->binary.op;

            switch (op) {

                case OP_ADD:
                case OP_SUB:
                case OP_MUL:
                case OP_DIV:
                    return (left == TYPE_FLOAT || right == TYPE_FLOAT)
                        ? TYPE_FLOAT : TYPE_INT;

                case OP_MOD:
                    if (left != TYPE_INT || right != TYPE_INT) {
                        fprintf(stderr,
                            "Semantic Error: Modulo requires integer operands.\n");
                        error_count++;
                        return TYPE_ERROR;
                    }
                    return TYPE_INT;

                case OP_EQ:
                case OP_NE:
                case OP_LT:
                case OP_GT:
                case OP_LE:
                case OP_GE:
                case OP_AND:
                case OP_OR:
                    return TYPE_INT;

                default:
                    return TYPE_ERROR;
            }
        }

        case NODE_UNARY: {
            DataType operand = check_expression(node->unary.operand);

            if (!is_numeric(operand))
                return TYPE_ERROR;

            if (node->unary.op == OP_NOT)
                return TYPE_INT;

            return operand;
        }

        default:
            fprintf(stderr,
                "Semantic Error: Invalid expression node.\n");
            error_count++;
            return TYPE_ERROR;
    }
}

/* Analyze statements */
static void check_statement(ASTNode *node) {
    if (!node)
        return;

    switch (node->type) {

        case NODE_VAR_DECL: {
            const char *name = node->var_decl.name;

            if (already_declared(name)) {
                fprintf(stderr,
                    "Semantic Error: Variable '%s' is already declared.\n",
                    name);

                error_count++;
                break;
            }

            DataType type = check_expression(node->var_decl.initializer);

            if (type != TYPE_ERROR)
                add_symbol(name, type);

            break;
        }

        case NODE_ASSIGN: {
            const char *name = node->assign.name;
            Symbol *symbol = find_symbol(name);

            if (!symbol) {
                fprintf(stderr,
                    "Semantic Error: Variable '%s' is not declared.\n",
                    name);

                error_count++;
                break;
            }

            DataType value_type = check_expression(node->assign.value);

            if (value_type == TYPE_ERROR)
                break;

            if (symbol->type == TYPE_INT && value_type == TYPE_FLOAT) {
                fprintf(stderr,
                    "Semantic Error: Cannot assign float to int variable '%s'.\n",
                    name);

                error_count++;
            }

            break;
        }

        case NODE_PRINT:
            check_expression(node->print.expression);
            break;

        case NODE_INPUT: {
            if (!find_symbol(node->input.name)) {
                fprintf(stderr,
                    "Semantic Error: Variable '%s' is not declared.\n",
                    node->input.name);

                error_count++;
            }
            break;
        }

        case NODE_IF: {
            DataType condition =
                check_expression(node->if_stmt.condition);

            if (!is_numeric(condition)) {
                fprintf(stderr,
                    "Semantic Error: Invalid if condition.\n");
                error_count++;
            }

            check_statement(node->if_stmt.then_branch);
            check_statement(node->if_stmt.else_branch);
            break;
        }

        case NODE_WHILE: {
            DataType condition =
                check_expression(node->while_stmt.condition);

            if (!is_numeric(condition)) {
                fprintf(stderr,
                    "Semantic Error: Invalid while condition.\n");
                error_count++;
            }

            check_statement(node->while_stmt.body);
            break;
        }

        case NODE_BLOCK:
            push_scope();
            check_statement_list(node->block.statements);
            pop_scope();
            break;

        case NODE_EXPR_STMT:
            check_expression(node->expr_stmt.expression);
            break;

        default:
            fprintf(stderr,
                "Semantic Error: Unknown statement node.\n");
            error_count++;
            break;
    }
}

/* Analyze a linked list of statements */
static void check_statement_list(ASTNode *node) {
    while (node) {
        check_statement(node);
        node = node->next;
    }
}

/* Main semantic analysis function */
int semantic_analyze(ASTNode *root) {
    error_count = 0;

    push_scope();

    if (root && root->type == NODE_PROGRAM) {
        check_statement_list(root->program.statements);
    } else {
        fprintf(stderr, "Semantic Error: Invalid program AST.\n");
        error_count++;
    }

    pop_scope();

    if (error_count == 0) {
        printf("Semantic Analysis Successful!\n");
        return 1;
    }

    printf("Semantic Analysis Failed! Errors: %d\n", error_count);
    return 0;
}