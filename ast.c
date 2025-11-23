#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode *ast_create(NodeType type, const char *text, ASTNode *left, ASTNode *right) {
    ASTNode *n = malloc(sizeof(ASTNode));
    if (!n) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    n->type  = type;
    n->left  = left;
    n->right = right;
    n->next  = NULL;
    n->text  = text ? strdup(text) : NULL;
    return n;
}

ASTNode *ast_cons(ASTNode *first, ASTNode *rest) {
    if (!first) return rest;
    first->next = rest;
    return first;
}

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) printf("  ");
}

static const char *node_type_name(NodeType t) {
    switch (t) {
        case AST_PROGRAM:     return "Program";
        case AST_SOURCE:      return "Source";
        case AST_SCHEMA:      return "Schema";
        case AST_FIELD:       return "Field";
        case AST_FIELD_LIST:  return "FieldList";
        case AST_ASSOCIATE:   return "Associate";
        case AST_COMPUTE:     return "Compute";
        case AST_JOIN:        return "Join";
        case AST_FILTER:      return "Filter";
        case AST_ANALYZE:     return "Analyze";
        case AST_ANALYZE_OP:  return "AnalyzeOp";
        case AST_EXPRESSION:  return "Expression";
        case AST_CONDITION:   return "Condition";
        default:              return "Unknown";
    }
}

void ast_print(ASTNode *node, int indent) {
    if (!node) return;

    print_indent(indent);
    printf("%s", node_type_name(node->type));
    if (node->text)
        printf(" [%s]", node->text);
    printf("\n");

    if (node->left)  ast_print(node->left,  indent + 1);
    if (node->right) ast_print(node->right, indent + 1);
    if (node->next)  ast_print(node->next,  indent);
}

