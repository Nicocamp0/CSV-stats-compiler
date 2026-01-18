#ifndef AST_H
#define AST_H

typedef enum {
    AST_PROGRAM,
    AST_SOURCE,
    AST_SCHEMA,
    AST_FIELD,
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
    char *text;
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *next;
} ASTNode;

/* Fonction qui crée un nouveau noeud AST
 * @param1 type: le type du noeud
 * @param2 *text: le nom associé au noeud (peut être NULL)
 * @param3 *left: l'argument associé au noeud gauche(peut être NULL)
 * @param4 *right: l'argument associé au noeud droite(peut être NULL)
 * @returns: un pointeur vers le nouveau noeud AST
 */
ASTNode *ast_create(NodeType type, const char *text, ASTNode *left, ASTNode *right);

/* Procédure qui ajoute un enfant à un noeud AST
 * @param1 first: le noeud parent
 * @param2 rest: le premier noeud des noeuds suivants
 */
ASTNode *ast_cons(ASTNode *first, ASTNode *rest);

/* Procédure qui affiche l'arbre AST de manière indentée
 * @param1 node: le noeud racine de l'AST
 * @param2 indent: le niveau d'indentation actuel pour indiquer l'imbrication des noeuds left et right 
   dans le noeud couant
 */
void ast_print(ASTNode *node, int indent);

#endif

