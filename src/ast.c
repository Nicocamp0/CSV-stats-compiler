#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode *ast_create(NodeType type, const char *text, ASTNode *left, ASTNode *right) {
    ASTNode *n = malloc(sizeof(ASTNode));
    n->type = type;
    n->left = left;
    n->right = right;
    n->next = NULL;
    n->text = text ? strdup(text) : NULL;
    return n;
}

ASTNode *ast_cons(ASTNode *list, ASTNode *rest) {
    if (!list) return rest;

    ASTNode *cur = list;
    while (cur->next) cur = cur->next;
    cur->next = rest;
    return list;
}


static void indent(int n) { for (int i = 0; i < n; i++) printf("  "); }

static const char *name(NodeType t) {
    switch(t) {
        case AST_PROGRAM: return "Program";
        case AST_SOURCE: return "Source";
        case AST_SCHEMA: return "Schema";
        case AST_FIELD: return "Field";
        case AST_ASSOCIATE: return "Associate";
        case AST_COMPUTE: return "Compute";
        case AST_JOIN: return "Join";
        case AST_FILTER: return "Filter";
        case AST_ANALYZE: return "Analyze";
        case AST_ANALYZE_OP: return "AnalyzeOp";
        case AST_EXPRESSION: return "Expression";
        case AST_CONDITION: return "Condition";
        default: return "Unknown";
    }
}

void ast_print(ASTNode *n, int d) {
    if (!n) return;
    indent(d);
    printf("%s", name(n->type));
    if (n->text) printf(" [%s]", n->text);
    printf("\n");

    ast_print(n->left, d+1);
    ast_print(n->right, d+1);
    ast_print(n->next, d);
}

