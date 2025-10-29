/**
 * @file ast.h
 * @brief Stockage de la représentation interne du programme AFSTAT.
 *
 * Définit la structure de données utilisée pour stocker la représentation interne
 * du programme AFSTAT après son analyse syntaxique.
 */

#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Field {
    char *name;       // nom de la colonne
    char *type;       // type: "integer", "float", "string"
    char *value;      // valeur (optionnelle, pour plus tard)
} Field;

typedef enum {
    AST_PROGRAM,
    AST_SOURCE,
    AST_SCHEMA,
    AST_ASSOCIATE,
    AST_ANALYZE,
    AST_FIELD
} ASTType;

// Structure contenant les informations necessaire pour chaque instruction AFSTAT
typedef struct ASTNode {
    ASTType type;
    char *name;           // identifiant (source, schema, etc.)
    char *arg;            // pour source : fichier CSV ; sinon NULL
    char *column;    //ex: integer, float, string (pour les columns de schema)
    struct ASTNode **children;
    int n_children;
} ASTNode;

// fonctions utilitaires

/* Fonction qui crée un nouveau noeud AST
 * @param1 type: le type du noeud
 * @param2 name: le nom associé au noeud (peut être NULL)
 * @param3 arg: l'argument associé au noeud (peut être NULL)
 * @returns: un pointeur vers le nouveau noeud AST
 */
ASTNode *ast_create(ASTType type, const char *name, const char *arg, const char *col_type);

/* Procédure qui ajoute un enfant à un noeud AST
 * @param1 parent: le noeud parent
 * @param2 child: le noeud enfant à ajouter
 */
void ast_add_child(ASTNode *parent, ASTNode *child);

/* Procédure qui affiche l'arbre AST de manière indentée
 * @param1 node: le noeud racine de l'AST
 * @param2 indent: le niveau d'indentation actuel
 */
void ast_print(ASTNode *node, int indent);

/* Procédure qui libère la mémoire allouée pour l'AST
 * @param node: le noeud racine de l'AST
 */
void ast_free(ASTNode *node);

#endif

