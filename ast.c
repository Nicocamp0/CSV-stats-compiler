#include "ast.h"

ASTNode *ast_create(ASTType type, const char *name, const char *arg, const char *col_type) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = type;
    node->name = name ? strdup(name) : NULL;
    node->arg = arg ? strdup(arg) : NULL;
    node->column = col_type ? strdup(col_type) : NULL;
    node->children = NULL;
    node->n_children = 0;
    return node;
}

void ast_add_child(ASTNode *parent, ASTNode *child) {
    parent->children = (ASTNode **)realloc(parent->children,
        sizeof(ASTNode*) * (parent->n_children + 1));
    parent->children[parent->n_children] = child;
    parent->n_children++;
}

void ast_print(ASTNode *node, int indent) {
    if (!node) return;
    for(int i=0;i<indent;i++) printf("  ");
    switch(node->type) {
        case AST_SOURCE:   printf("Source: %s -> %s\n", node->name, node->arg); break;
        case AST_SCHEMA:   printf("Schema: %s\n", node->name); break;
        case AST_ASSOCIATE:printf("Associate: %s\n", node->name); break;
        case AST_ANALYZE:  printf("Analyze: %s\n", node->name); break;
        case AST_FIELD: printf("Field: %s (%s)\n", node->name, node->column); break;
    }
    for(int i=0;i<node->n_children;i++)
        ast_print(node->children[i], indent+1);
}

void ast_free(ASTNode *node) {
    if (!node) return;
    if (node->name) free(node->name);
    if (node->arg) free(node->arg);
    if (node->column) free(node->column);
    for(int i=0;i<node->n_children;i++)
        ast_free(node->children[i]);
    free(node->children);
    free(node);
}

