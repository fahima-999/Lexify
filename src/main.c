/* Lexify driver: lexer -> parser -> (AST -> semantic -> TAC, to be added) */
#include <stdio.h>
#include <string.h>
#include "ast/ast.h"

extern FILE *yyin;
extern int yyparse(void);
extern int lexical_errors;
extern int syntax_errors;
extern Node *ast_root;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source.mc> [--ast]\n", argv[0]);
        return 2;
    }
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror(argv[1]);
        return 2;
    }
    yyparse();
    fclose(yyin);

    if (lexical_errors || syntax_errors) {
        fprintf(stderr, "Compilation failed: %d lexical error(s), %d syntax error(s)\n",
                lexical_errors, syntax_errors);
        return 1;
    }
    if (argc > 2 && strcmp(argv[2], "--ast") == 0)
        ast_print(ast_root, 0);
    /* TODO: semantic analysis (ast_root), then TAC generation. */
    printf("Parsing successful: no lexical or syntax errors.\n");
    return 0;
}
