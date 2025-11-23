#ifndef AST_H
#define AST_H

typedef enum {
    AST_PROGRAM,
    AST_SOURCE,
    AST_SCHEMA,
    AST_FIELD,
    AST_FIELD_LIST,
    AST_ASSOCIATE,
    AST_COMPUTE,
    AST_JOIN,
    AST_FILTER,
    AST_ANALYZE,
    AST_ANALYZE_OP,
    AST_EXPRESSION,
    AST_CONDITION
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char *text;              /* petit label : nom, littéral, op… */
    struct ASTNode *left;    /* enfant gauche / principal */
    struct ASTNode *right;   /* enfant droit / secondaire */
    struct ASTNode *next;    /* frère suivant (listes chaînées) */
} ASTNode;

ASTNode *ast_create(NodeType type, const char *text, ASTNode *left, ASTNode *right);
ASTNode *ast_cons(ASTNode *first, ASTNode *rest);
void ast_print(ASTNode *node, int indent);

#endif

