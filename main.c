#include <stdio.h>
#include "ast.h"

extern int yyparse();
extern ASTNode *root;

int main() {
    if (yyparse() == 0) {
        printf("=== AST ===\n");
        ast_print(root, 0);
    }
    return 0;
}

