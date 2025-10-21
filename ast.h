#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    AST_SOURCE,
    AST_SCHEMA,
    AST_ASSOCIATE,
    AST_ANALYZE
} ASTType;

typedef struct ASTNode {
    ASTType type;
    char *name;           // identifiant (source, schema, etc.)
    char *arg;            // pour source : fichier CSV ; sinon NULL
    struct ASTNode **children;
    int n_children;
} ASTNode;

// fonctions utilitaires
ASTNode *ast_create(ASTType type, const char *name, const char *arg);
void ast_add_child(ASTNode *parent, ASTNode *child);
void ast_print(ASTNode *node, int indent);
void ast_free(ASTNode *node);

#endif

