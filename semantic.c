#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "ast.h"
#include "symbol_table.h"

static int error_count = 0;

static void sem_error(const char *msg) {
    fprintf(stderr, "[ERREUR SEMANTIQUE] %s\n", msg);
    error_count++;
}

/* Obtenir le type d'une colonne ex: patients.age */
static const char *get_column_type(const char *fullcol, SymbolTable *symtab) {
    char src[128], col[128];
    if (sscanf(fullcol, "%127[^.].%127s", src, col) != 2) return NULL;

    Schema *schema = symtab_get_schema_by_source(symtab, src);
    if (!schema) return NULL;

    return schema_get_col_type(schema, col);
}

static void check_join(ASTNode *node, SymbolTable *symtab) {
    const char *t1 = get_column_type(node->left->text,  symtab);
    const char *t2 = get_column_type(node->right->text, symtab);

    if (!t1 || !t2) {
        sem_error("Jointure sur colonne inconnue");
        return;
    }

    if (strcmp(t1, t2) != 0) {
        sem_error("Jointure entre colonnes de types incompatibles");
    }
}

static void check_analyze(ASTNode *node, SymbolTable *symtab) {
    const char *schema_name = node->text;
    Schema *schema = symtab_get_schema(symtab, schema_name);

    if (!schema) {
        sem_error("Analyse sur un schema inconnu");
        return;
    }

    ASTNode *ops = node->left;
    while (ops) {
        const char *opname = ops->text;  // ex: "mean age"
        char op[64], col[64];
        sscanf(opname, "%s %s", op, col);

        const char *ctype = schema_get_col_type(schema, col);
        if (!ctype) {
            sem_error("Analyse sur colonne inexistante");
        } else if (
            (!strcmp(op, "mean") || !strcmp(op, "median")
             || !strcmp(op, "std_dev") || !strcmp(op, "summary"))
            && !strcmp(ctype, "string"))
        {
            sem_error("Analyse numérique appliquée sur une colonne de type string");
        }

        ops = ops->next;
    }
}

int semantic_check(ASTNode *root, SymbolTable *symtab) {
    error_count = 0;

    ASTNode *node = root->left;
    while (node) {
        switch (node->type) {
            case AST_JOIN:
                check_join(node, symtab);
                break;

            case AST_ANALYZE:
                check_analyze(node, symtab);
                break;

            case AST_FILTER:
                // TODO: ajouter plus tard
                break;

            default:
                break;
        }

        node = node->next;
    }

    return error_count;
}

