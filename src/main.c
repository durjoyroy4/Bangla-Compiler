
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "semantic.h"
#include "codegen.h"

extern FILE *yyin;
extern int yyparse(void);
extern ASTNode *ast_root;

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: bangla_compiler <source_file.bnl>\n");
        return EXIT_FAILURE;
    }

    yyin = fopen(argv[1], "r");

    if (yyin == NULL) {
        perror("Error opening source file");
        return EXIT_FAILURE;
    }

    printf("Bangla Compiler Started...\n");

    int result = yyparse();
    fclose(yyin);

    if (result != 0) {
        printf("Compilation Failed!\n");
        return EXIT_FAILURE;
    }

    printf("Syntax Analysis Successful!\n");
    printf("AST Generated Successfully!\n");

    if (!semantic_analyze(ast_root)) {
        return EXIT_FAILURE;
    }

    if (!generate_c_code(ast_root, "output/generated.c")) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
