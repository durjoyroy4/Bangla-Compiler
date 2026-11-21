
#include <stdio.h>
#include <stdlib.h>

#include "codegen.h"

static void generate_expression(FILE *output, ASTNode *node);
static void generate_statement(FILE *output, ASTNode *node);
static void generate_statement_list(FILE *output, ASTNode *node);

static void generate_expression(FILE *output, ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_INT:
            fprintf(output, "%d", node->int_value.value);
            break;

        case NODE_FLOAT:
            fprintf(output, "%g", node->float_value.value);
            break;

        case NODE_IDENTIFIER:
            fprintf(output, "%s", node->identifier.name);
            break;

        case NODE_BINARY: {
            const char *operators[] = {
                "+", "-", "*", "/", "%",
                "==", "!=", "<", ">", "<=", ">=",
                "&&", "||"
            };

            fprintf(output, "(");
            generate_expression(output, node->binary.left);
            fprintf(output, " %s ", operators[node->binary.op]);
            generate_expression(output, node->binary.right);
            fprintf(output, ")");
            break;
        }

        case NODE_UNARY:
            if (node->unary.op == OP_NOT)
                fprintf(output, "!");
            else
                fprintf(output, "-");

            generate_expression(output, node->unary.operand);
            break;

        default:
            fprintf(stderr, "Code Generation Error: Invalid expression.\n");
            break;
    }
}

static void generate_statement(FILE *output, ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_VAR_DECL:
            if (node->var_decl.initializer &&
                node->var_decl.initializer->type == NODE_FLOAT) {
                fprintf(output, "float ");
            } else {
                fprintf(output, "int ");
            }

            fprintf(output, "%s = ", node->var_decl.name);
            generate_expression(output, node->var_decl.initializer);
            fprintf(output, ";\n");
            break;

        case NODE_ASSIGN:
            fprintf(output, "%s = ", node->assign.name);
            generate_expression(output, node->assign.value);
            fprintf(output, ";\n");
            break;

        case NODE_PRINT:
            fprintf(output, "printf(\"%%g\\n\", (double)(");
            generate_expression(output, node->print.expression);
            fprintf(output, "));\n");
            break;

        case NODE_INPUT:
            fprintf(output, "scanf(\"%%d\", &%s);\n", node->input.name);
            break;

        case NODE_EXPR_STMT:
            generate_expression(output, node->expr_stmt.expression);
            fprintf(output, ";\n");
            break;

        case NODE_BLOCK:
            fprintf(output, "{\n");
            generate_statement_list(output, node->block.statements);
            fprintf(output, "}\n");
            break;

        case NODE_IF:
            fprintf(output, "if (");
            generate_expression(output, node->if_stmt.condition);
            fprintf(output, ") ");
            generate_statement(output, node->if_stmt.then_branch);

            if (node->if_stmt.else_branch) {
                fprintf(output, "else ");
                generate_statement(output, node->if_stmt.else_branch);
            }
            break;

        case NODE_WHILE:
            fprintf(output, "while (");
            generate_expression(output, node->while_stmt.condition);
            fprintf(output, ") ");
            generate_statement(output, node->while_stmt.body);
            break;

        default:
            fprintf(stderr, "Code Generation Error: Invalid statement.\n");
            break;
    }
}

static void generate_statement_list(FILE *output, ASTNode *node) {
    while (node) {
        generate_statement(output, node);
        node = node->next;
    }
}

int generate_c_code(ASTNode *root, const char *filename) {
    FILE *output = fopen(filename, "w");

    if (!output) {
        perror("Code Generation Error");
        return 0;
    }

    fprintf(output, "#include <stdio.h>\n\n");
    fprintf(output, "int main() {\n");

    if (root && root->type == NODE_PROGRAM) {
        generate_statement_list(output, root->program.statements);
    } else {
        fprintf(stderr, "Code Generation Error: Invalid AST.\n");
        fclose(output);
        return 0;
    }

    fprintf(output, "return 0;\n");
    fprintf(output, "}\n");

    fclose(output);

    printf("C Code Generated Successfully: %s\n", filename);
    return 1;
}
