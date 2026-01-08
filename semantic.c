#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "ast.h"
#include "symbol_table.h"

static int error_count = 0;

typedef struct {
    char name[64];
    char type[16];
} ComputedCol;

static ComputedCol g_comp[128];
static int g_comp_n = 0;

static const char *computed_get_type(const char *name) {
    for (int i = 0; i < g_comp_n; i++) {
        if (strcmp(g_comp[i].name, name) == 0) return g_comp[i].type;
    }
    return NULL;
}

static void computed_add(const char *name, const char *type) {
    if (g_comp_n >= 128) return;
    strncpy(g_comp[g_comp_n].name, name, sizeof(g_comp[g_comp_n].name)-1);
    g_comp[g_comp_n].name[sizeof(g_comp[g_comp_n].name)-1] = '\0';
    strncpy(g_comp[g_comp_n].type, type, sizeof(g_comp[g_comp_n].type)-1);
    g_comp[g_comp_n].type[sizeof(g_comp[g_comp_n].type)-1] = '\0';
    g_comp_n++;
}

static Schema *g_current_schema = NULL;

static void sem_error(const char *msg) {
    fprintf(stderr, "[ERREUR SEMANTIQUE] %s\n", msg);
    error_count++;
}

static const char *get_column_type(const char *fullcol, SymbolTable *symtab) {
    char left[128], col[128];
    if (sscanf(fullcol, "%127[^.].%127s", left, col) != 2) return NULL;

    // Cas A: "schema.col"
    Schema *schema = symtab_get_schema(symtab, left);
    if (schema) return schema_get_col_type(schema, col);

    // Cas B: "source.col"
    schema = symtab_get_schema_by_source(symtab, left);
    if (!schema) return NULL;
    return schema_get_col_type(schema, col);
}


static int expr_is_numeric(ASTNode *e, SymbolTable *symtab) {
    if (!e) return 1;

    if (!e->left && !e->right) {
        const char *t = e->text ? e->text : "";

        if ((t[0] >= '0' && t[0] <= '9') || t[0] == '-' ) return 1;
        if (!g_current_schema) return 0;

        const char *ctype = schema_get_col_type(g_current_schema, t);
        if (!ctype) ctype = computed_get_type(t);

        if (!ctype) return 0;
        if (strcmp(ctype, "string") == 0) return 0;
        return 1;
    }

    // opérateur binaire
    return expr_is_numeric(e->left, symtab) && expr_is_numeric(e->right, symtab);
}

static void check_compute(ASTNode *node, SymbolTable *symtab) {
    (void)symtab;

    if (!g_current_schema) {
        sem_error("compute utilisé sans schema associé juste avant");
        return;
    }

    const char *newcol = node->text;
    ASTNode *expr = node->left; 

    if (!expr_is_numeric(expr, symtab)) {
        sem_error("compute: expression invalide (colonne inconnue ou string dans un calcul)");
        return;
    }
    computed_add(newcol, "float");
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
    const char *name = node->text;

    Schema *schema = symtab_get_schema(symtab, name);
    if (!schema) return;

    ASTNode *ops = node->left;
    while (ops) {
        const char *opname = ops->text;
        char op[64] = {0}, col[64] = {0};
        sscanf(opname, "%63s %63s", op, col);

        const char *ctype = schema_get_col_type(schema, col);
        if (!ctype) ctype = computed_get_type(col);

        if (!ctype) {  // <-- NE PLUS BLOQUER
            ops = ops->next;
            continue;
        }

        if (
            (!strcmp(op, "mean") || !strcmp(op, "median")
            || !strcmp(op, "std_dev") || !strcmp(op, "summary")
            || !strcmp(op, "min") || !strcmp(op, "max"))
            && !strcmp(ctype, "string"))
        {
            sem_error("Analyse numérique appliquée sur une colonne de type string");
        }
        ops = ops->next;
    }
}



int semantic_check(ASTNode *root, SymbolTable *symtab) {
    error_count = 0;
    g_comp_n = 0;
    g_current_schema = NULL;


    ASTNode *node = root->left;
    while (node) {
        switch (node->type) {
            case AST_JOIN:
                check_join(node, symtab);
                break;

            case AST_ANALYZE:
                check_analyze(node, symtab);
                break;

            case AST_ASSOCIATE:
                g_current_schema = symtab_get_schema(symtab, node->text);
                if (!g_current_schema) sem_error("associate sur un schema inconnu");
                break;
            
            case AST_COMPUTE:
                check_compute(node, symtab);
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

