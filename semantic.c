#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "ast.h"
#include "symbol_table.h"

static int error_count = 0;
static char g_current_dataset[64] = "";

typedef struct {
    char name[64];
    char type[16];
} VCol;

typedef struct {
    int n;
    VCol cols[256];
} VSchema;


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

typedef struct {
    char dataset[64];
    VSchema schema;
    int has_schema; // 0/1
} DatasetSchema;

static DatasetSchema g_ds[128];
static int g_ds_n = 0;

static DatasetSchema* ds_find(const char *dataset) {
    for (int i=0;i<g_ds_n;i++)
        if (strcmp(g_ds[i].dataset, dataset)==0) return &g_ds[i];
    return NULL;
}

static DatasetSchema* ds_get_or_create(const char *dataset) {
    DatasetSchema *e = ds_find(dataset);
    if (e) return e;
    if (g_ds_n >= 128) return NULL;
    memset(&g_ds[g_ds_n], 0, sizeof(g_ds[g_ds_n]));
    strncpy(g_ds[g_ds_n].dataset, dataset, sizeof(g_ds[g_ds_n].dataset)-1);
    return &g_ds[g_ds_n++];
}

static const char* vschema_get_type(VSchema *s, const char *col) {
    for (int i=0;i<s->n;i++)
        if (strcmp(s->cols[i].name, col)==0) return s->cols[i].type;
    return NULL;
}

static void vschema_add(VSchema *s, const char *name, const char *type) {
    if (s->n >= 256) return;
    if (vschema_get_type(s, name)) return;
    strncpy(s->cols[s->n].name, name, 63);
    strncpy(s->cols[s->n].type, type, 15);
    s->cols[s->n].name[63]='\0';
    s->cols[s->n].type[15]='\0';
    s->n++;
}

static VSchema vschema_merge(VSchema *a, VSchema *b) {
    VSchema out;
    memset(&out, 0, sizeof(out));

    for (int i=0;i<a->n;i++) vschema_add(&out, a->cols[i].name, a->cols[i].type);
    for (int i=0;i<b->n;i++) vschema_add(&out, b->cols[i].name, b->cols[i].type);
    return out;
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

static void vschema_from_schema(VSchema *out, Schema *s) {
    out->n = 0;
    if (!s) return;
    for (int i = 0; i < s->column_count && out->n < 256; i++) {
        strncpy(out->cols[out->n].name, s->columns[i].name, sizeof(out->cols[out->n].name)-1);
        out->cols[out->n].name[sizeof(out->cols[out->n].name)-1] = '\0';

        strncpy(out->cols[out->n].type, s->columns[i].type, sizeof(out->cols[out->n].type)-1);
        out->cols[out->n].type[sizeof(out->cols[out->n].type)-1] = '\0';

        out->n++;
    }
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
    DatasetSchema *E = ds_find(g_current_dataset);
    if (E && E->has_schema) {
        vschema_add(&E->schema, newcol, "float");
    }

}

static void split_qual(const char *q, char *out_ds, char *out_col) {
    out_ds[0]=out_col[0]='\0';
    if (!q) return;
    sscanf(q, "%63[^.].%63s", out_ds, out_col);
}

static void check_filter(ASTNode *node, SymbolTable *symtab) {
    (void)symtab;

    if (!node->right || node->right->type != AST_CONDITION) {
        sem_error("filter: condition invalide");
        return;
    }

    ASTNode *lhs = node->right->left;
    ASTNode *rhs = node->right->right;
    const char *op = node->right->text ? node->right->text : "";

    if (!lhs || !rhs || !lhs->text || !rhs->text) {
        sem_error("filter: condition incomplète");
        return;
    }

    // On suppose que lhs est une colonne qualifiée "dataset.col"
    char ds[64]={0}, col[64]={0};
    split_qual(lhs->text, ds, col);

    DatasetSchema *E = ds_find(ds);
    if (!E || !E->has_schema) {
        sem_error("filter: colonne sur dataset sans schema (manque associate/join avant)");
        return;
    }

    const char *t_lhs = vschema_get_type(&E->schema, col);
    if (!t_lhs) {
        sem_error("filter: colonne inexistante dans le dataset");
        return;
    }

    int is_num_op = (!strcmp(op, ">") || !strcmp(op, "<") || !strcmp(op, ">=") || !strcmp(op, "<="));
    int is_eq_op  = (!strcmp(op, "==") || !strcmp(op, "!="));

    if (!strcmp(t_lhs, "string")) {
        if (is_num_op) sem_error("filter: comparaison numérique sur colonne string");
        // si string, on n'autorise que == / != (à toi selon l'énoncé)
        if (!is_eq_op) sem_error("filter: opérateur invalide sur string");
    } else {
        // lhs numérique → interdire ==/!= avec string literal (si tu veux)
        // (optionnel) vérifier rhs est bien numérique si littéral
    }
}



static void check_analyze(ASTNode *node, SymbolTable *symtab) {
    const char *name = node->text;
    printf("DEBUG analyze on %s: ds=%p has=%d sym=%p\n",name, (void*)g_ds, g_ds?g_ds->has_schema:0, (void*)symtab_get_schema(symtab,name));
 

    // 1) priorité: dataset (join/filter/associate)
    DatasetSchema *ds = ds_find(name);
    VSchema *vs = NULL;
    VSchema tmp;

    if (ds && ds->has_schema) {
        vs = &ds->schema;
    } else {
        // 2) sinon: schema déclaré
        Schema *schema = symtab_get_schema(symtab, name);
        if (!schema) {
            sem_error("analyze: identifiant inconnu (ni dataset ni schema)");
            return;
        }
        vschema_from_schema(&tmp, schema);
        vs = &tmp;
    }

    ASTNode *ops = node->left;
    while (ops) {
        char op[64] = {0}, col[64] = {0};
        sscanf(ops->text, "%63s %63s", op, col);

        const char *ctype = vschema_get_type(vs, col);
        if (!ctype) ctype = computed_get_type(col);

        // ✅ ici on rebloque vraiment
        if (!ctype) {
            sem_error("Analyse sur colonne inexistante");
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
    g_ds_n = 0;


    ASTNode *node = root->left;
    while (node) {
        switch (node->type) {
            case AST_JOIN: {
                const char *out_ds = node->text;
                char left_ds[64], left_col[64], right_ds[64], right_col[64];
                split_qual(node->left ? node->left->text : NULL, left_ds, left_col );
                split_qual(node->right? node->right->text: NULL, right_ds, right_col);
            
                DatasetSchema *L = ds_find(left_ds);
                DatasetSchema *R = ds_find(right_ds);
                if (!L || !L->has_schema || !R || !R->has_schema) {
                    sem_error("join: schema inconnu pour un des datasets (manque associate avant join)");
                    break;
                }
            
                // check types des clés
                const char *t1 = vschema_get_type(&L->schema, left_col);
                const char *t2 = vschema_get_type(&R->schema, right_col);
                if (!t1 || !t2) { sem_error("join: colonne clé inconnue"); break; }
                if (strcmp(t1,t2)!=0) sem_error("join: types incompatibles sur les clés");
            
                DatasetSchema *O = ds_get_or_create(out_ds);
                O->schema = vschema_merge(&L->schema, &R->schema);
                O->has_schema = 1;
                break;
            }

            case AST_ANALYZE:
                check_analyze(node, symtab);
                break;

            case AST_ASSOCIATE: {
                const char *schema_name = node->text;
                const char *dataset = node->left ? node->left->text : NULL;
                Schema *s = symtab_get_schema(symtab, schema_name);
                if (!s) { sem_error("associate sur un schema inconnu"); break; }
                if (!dataset) { sem_error("associate: dataset manquant"); break; }                
                DatasetSchema *e = ds_get_or_create(dataset);
                if (!e) { sem_error("trop de datasets"); break; }
                vschema_from_schema(&e->schema, s);
                e->has_schema = 1;
                
                g_current_schema = s;
                strncpy(g_current_dataset, dataset, sizeof(g_current_dataset)-1);
                g_current_dataset[sizeof(g_current_dataset)-1] = '\0';
                break;
            }
                
            
            case AST_COMPUTE:
                check_compute(node, symtab);
                break;
            

            case AST_FILTER: {
                check_filter(node, symtab);
                const char *out_ds = node->text;
                const char *in_ds  = node->left ? node->left->text : NULL;
                DatasetSchema *I = ds_find(in_ds);
                if (!I || !I->has_schema) {
                    sem_error("filter: dataset d'entrée sans schema (manque associate/join avant)");
                    break;
                }
                DatasetSchema *O = ds_get_or_create(out_ds);
                O->schema = I->schema;
                O->has_schema = 1;
                break;
            }


            default:
                break;
        }

        node = node->next;
    }

    return error_count;
}

