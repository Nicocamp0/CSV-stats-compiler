#include <stdio.h>
#include <string.h>
#include "semantic.h"
#include "ast.h"
#include "symbol_table.h"

static int error_count = 0;
static char g_current_dataset[64] = "";

/* Fonction qui vérifie si un nom de colonne existe dans une ligne d’en-tête CSV
 * @param1 *header: Pointeur vers le l'entete d'un fichier csv 
 * @param2 *colname: le nom associé au noeud (peut être NULL)
 * @return: 1 si la colonne est trouvé; à sinon
 */
static int header_has_col(const char *header, const char *colname) {
    // header = "a,b,c\n"
    char tmp[4096];
    strncpy(tmp, header, sizeof(tmp)-1);
    tmp[sizeof(tmp)-1] = '\0';

    char *save = NULL;
    //génère des tokens découpé par , ou retour a la ligne ou espace
    for (char *tok = strtok_r(tmp, ",\r\n", &save); tok; tok = strtok_r(NULL, ",\r\n", &save)) {
        if (strcmp(tok, colname) == 0) return 1;
    }
    return 0;
}

/* Fonction qui lit la première ligne d’un fichier CSV et la stocke dans out_header
 * @param1 *file: Pointeur vers le l'entete d'un fichier csv 
 * @param2 *out_header: le nom associé au noeud (peut être NULL)
 * @param3 cap: La taille de la chaine de caractère à lire
 * @return: 1 si elle a réussit à lire l'entete et la stoqué, 0 sinon
 */
static int read_csv_header(const char *file, char *out_header, size_t cap) {
    FILE *f = fopen(file, "r");
    if (!f) return 0;
    if (!fgets(out_header, (int)cap, f)) { fclose(f); return 0; }
    fclose(f);
    return 1;
}

// cette structue sera utilisé pour stoquer les informations du fichiers csv qui sont les colonne des schéma en afs
typedef struct {
    char name[64];
    char type[16];
} VCol;

typedef struct {
    int n;
    VCol cols[256];
} VSchema; //un schéma complet d’un ensemble de colonnes


typedef struct {
    char name[64];
    char type[16];
} ComputedCol;

static ComputedCol g_comp[128]; //tableau globale de colonne pour l'operation compute
static int g_comp_n = 0; // le nombre de colonne qui sera définit pour compute

/* Fonction qui cherche une colonne par son nom dans table globale des schéma du fichier csv
 * @param1 *name: Pointeur vers le nom de la colonne
 * @return: le type de la colonne, NULL sinon
 */
static const char *computed_get_type(const char *name) {
    for (int i = 0; i < g_comp_n; i++) {
        if (strcmp(g_comp[i].name, name) == 0) return g_comp[i].type;
    }
    return NULL;
}

/* Procédure qui ajoute une colonne avec son nom et son type dans le tableau globale et incrémente le nombre de colonne définit
 * @param1 *name: Pointeur vers le l'entete d'un fichier csv 
 * @param2 *type: le nom associé au noeud (peut être NULL)
 */
static void computed_add(const char *name, const char *type) {
    if (g_comp_n >= 128) return; //Si le tableau est full on n'ajoute pas
    strncpy(g_comp[g_comp_n].name, name, sizeof(g_comp[g_comp_n].name)-1);
    g_comp[g_comp_n].name[sizeof(g_comp[g_comp_n].name)-1] = '\0'; //Marquer l'arret de la chaine
    strncpy(g_comp[g_comp_n].type, type, sizeof(g_comp[g_comp_n].type)-1);
    g_comp[g_comp_n].type[sizeof(g_comp[g_comp_n].type)-1] = '\0';
    g_comp_n++;
}


typedef struct {
    char dataset[64];
    VSchema schema;
    int has_schema; // 0/1
} DatasetSchema;

static DatasetSchema g_ds[128]; //tableau globale du dataset des schémas
static int g_ds_n = 0;

/* Fonction qui cherche si un dataset existe déjà dans g_ds à travers son nom
 * @param1 *dataset: Le nom du dataset 
 * @return le dataset si trouvé, 0 sinon
 */
static DatasetSchema* ds_find(const char *dataset) {
    for (int i=0;i<g_ds_n;i++)
        if (strcmp(g_ds[i].dataset, dataset)==0) return &g_ds[i];
    return NULL;
}

/* Fonction qui récupère un dataset s'il existe ou le crée vide sinon
 * @param1 *DatasetSchema: Pointeur vers le nom de la colonne
 * @return: Un pointeur vers un dataset existant ou nouveau
 */
static DatasetSchema* ds_get_or_create(const char *dataset) {
    DatasetSchema *e = ds_find(dataset);
    if (e) return e;
    if (g_ds_n >= 128) return NULL;
    memset(&g_ds[g_ds_n], 0, sizeof(g_ds[g_ds_n]));
    strncpy(g_ds[g_ds_n].dataset, dataset, sizeof(g_ds[g_ds_n].dataset)-1);
    return &g_ds[g_ds_n++];
}

/* Fonction qui récupère un dataset ou le crée vide
 * @param1 *name: Pointeur vers le nom de la colonne
 * @return: Un dataset
 */
static const char* vschema_get_type(VSchema *s, const char *col) {
    for (int i=0;i<s->n;i++)
        if (strcmp(s->cols[i].name, col)==0) return s->cols[i].type;
    return NULL;
}

/* Procédure qui ajoute une colonne à un schéma avec son nom et type
 * @param1 *s: Pointeur vers le schéma
 * @param2 *name: Pointeur vers le nom de la colonne
 * @param3 *s: Pointeur vers le type de la colonne
 */
static void vschema_add(VSchema *s, const char *name, const char *type) {
    if (s->n >= 256) return;
    if (vschema_get_type(s, name)) return;
    strncpy(s->cols[s->n].name, name, 63);
    strncpy(s->cols[s->n].type, type, 15);
    s->cols[s->n].name[63]='\0';
    s->cols[s->n].type[15]='\0';
    s->n++;
}

/* Procédure qui rassemble les colonnes de deux schémas
 * @param1 *a: Pointeur vers le schéma a
 * @param1 *b: Pointeur vers le schéma a
 * @return un schéma out combinant les deux 
 */
static VSchema vschema_merge(VSchema *a, VSchema *b) {
    VSchema out;
    memset(&out, 0, sizeof(out));

    for (int i=0;i<a->n;i++) vschema_add(&out, a->cols[i].name, a->cols[i].type);
    for (int i=0;i<b->n;i++) vschema_add(&out, b->cols[i].name, b->cols[i].type);
    return out;
}



static Schema *g_current_schema = NULL;

//Affichage d'un message d'ereur pou identifier que c'est au niveau de lecture du fichier csv
static void sem_error(const char *msg) {
    fprintf(stderr, "[ERREUR SEMANTIQUE] %s\n", msg);
    error_count++;
}

/* Fonction récursive qui dit s'il ya une valeur numérique dans l'arbre de noeuds dans le type et nom  
 * @param1 *e: Pointeur vers un noeud d'arbre
 * @param1 *b: la table des symboles
 * @return 1 s'il existe une valeur numérique, 0 sinon
 */
static int expr_is_numeric(ASTNode *e, SymbolTable *symtab) {
    if (!e) return 1; //arbe null

    if (!e->left && !e->right) {
        const char *t = e->text ? e->text : "";

        if ((t[0] >= '0' && t[0] <= '9') || t[0] == '-' ) return 1; //valeur numeric
        if (!g_current_schema) return 0; //schema null

        const char *ctype = schema_get_col_type(g_current_schema, t);
        if (!ctype) ctype = computed_get_type(t); //type de schema dans la table globale de schema

        if (!ctype) return 0;
        if (strcmp(ctype, "string") == 0) return 0;
        return 1;
    }

    // opérateur binaire
    return expr_is_numeric(e->left, symtab) && expr_is_numeric(e->right, symtab);
}

/* Procédure qui copie toutes les colonnes d’un schéma s vers un schéma out
 * @param1 *out: Pointeur vers le schéma
 * @param2 *s: Pointeur vers le nom de la colonne
 */
static void vschema_from_schema(VSchema *out, Schema *s) {
    out->n = 0;
    if (!s) return;
    for (int i = 0; i < s->column_count && out->n < 256; i++) {
        strncpy(out->cols[out->n].name, s->columns[i].name, sizeof(out->cols[out->n].name)-1);
        out->cols[out->n].name[sizeof(out->cols[out->n].name)-1] = '\0'; //empêche les dépassements mémoire

        strncpy(out->cols[out->n].type, s->columns[i].type, sizeof(out->cols[out->n].type)-1);
        out->cols[out->n].type[sizeof(out->cols[out->n].type)-1] = '\0';

        out->n++;
    }
}

/* Procédure qui vérifie le bon fonctionnement de compute selon des critères cité
 * @param1 *node: Pointeur vers le schéma node
 * @param2 *symtab: Pointeur vers la table des symbole
 */
static void check_compute(ASTNode *node, SymbolTable *symtab) {
    (void)symtab;

    if (!g_current_schema) { //ne peut être utilisé que juste après la création d'un schema
        sem_error("compute utilisé sans schema associé juste avant");
        return;
    }

    const char *newcol = node->text;
    ASTNode *expr = node->left; 

    if (!expr_is_numeric(expr, symtab)) {
        sem_error("compute: expression invalide (colonne inconnue ou string dans un calcul)");
        return;
    }
    computed_add(newcol, "float"); //Ajouter colonne calculée dans la table globale
    DatasetSchema *E = ds_find(g_current_dataset); // Mettre à jour le schéma du dataset
    if (E && E->has_schema) {
        vschema_add(&E->schema, newcol, "float");
    }

}

/* Séparer une chaine du type dataset.colonne en deux parties, out_ds le nom du dataset et 
 out_col le nom de la colonne
 * @param1 *q: Pointeur vers la chaine à séparer
 * @param2 *out_ds: Pointeur vers le nom du dataset
 * @param2 *out_col: Pointeur vers le nom de la colonne
 */
static void split_qual(const char *q, char *out_ds, char *out_col) {
    out_ds[0]=out_col[0]='\0';//initialiser à vide
    if (!q) return;
    sscanf(q, "%63[^.].%63s", out_ds, out_col); // limite à 63 caractères pour éviter les débordements mémoire
}

/* Procédure qui vérifie que la condition de filter est sémantiquement correcte
 * @param1 *node: Pointeur vers l'abre de noeuds
 * @param2 *symtab: Pointeur vers la table de symbole
 */
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

    DatasetSchema *E = ds_find(ds); //On cherche le dataset dans le
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
        // si string, on n'autorise que == / !=
        if (!is_eq_op) sem_error("filter: opérateur invalide sur string");
    } else {
        // lhs numérique donc interdit ==/!= avec string literal
    }
}


/* Procédure qui vérifie que la condition de analyze est sémantiquement correcte, 
c'est à dire que le schéma existe, n'est pas vide et qu;on applique une opérationde calcul dessus et pas un autre schéma
 * @param1 *node: Pointeur vers l'abre de noeuds
 * @param2 *symtab: Pointeur vers la table de symbole
 */
static void check_analyze(ASTNode *node, SymbolTable *symtab) {
    const char *name = node->text;
    DatasetSchema *ds = ds_find(name);
    VSchema *vs = NULL;
    VSchema tmp;

    if (ds && ds->has_schema) { // On extrait le nom du schéma du dataset
        vs = &ds->schema;
    } else {
        Schema *schema = symtab_get_schema(symtab, name);
        if (!schema) {
            sem_error("analyze: identifiant inconnu (ni dataset ni schema)");
            return;
        }
        vschema_from_schema(&tmp, schema); // On crée ce schéma
        vs = &tmp;
    }

    ASTNode *ops = node->left;
    while (ops) {
        char op[64] = {0}, col[64] = {0};
        sscanf(ops->text, "%63s %63s", op, col);

        const char *ctype = vschema_get_type(vs, col);
        if (!ctype) ctype = computed_get_type(col);
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



/* Fonction qui vérifie que la condition de analyze, filter, compute sont sémantiquement correcte sur tout 
les schéma su lequel on fait des calcls dessus
 * @param1 *root: Le premier noeud de l'arbre
 * @param2 *symtab: La table de symbole
 * @return: le nombre d'erreur sémantique
 */
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
                const char *source_name = node->left ? node->left->text : NULL;

                Schema *s = symtab_get_schema(symtab, schema_name);
                if (!s) { sem_error("associate sur un schema inconnu"); break; }
                if (!source_name) { sem_error("associate: source manquante"); break; }
                const char *csv = symtab_get_source_file(symtab, source_name);
                if (!csv) { sem_error("associate: source non définie (pas de 'source ... \"file\"')"); break; }
                char header[4096];
                if (!read_csv_header(csv, header, sizeof(header))) {
                    sem_error("associate: impossible d'ouvrir ou lire le header du CSV");
                    break;
                }
                for (int i = 0; i < s->column_count; i++) {
                    const char *col = s->columns[i].name;
                    if (!header_has_col(header, col)) {
                        sem_error("associate: colonne du schema absente dans le CSV");
                    }
                }
                symtab_associate(symtab, schema_name, source_name);
                DatasetSchema *e = ds_get_or_create(source_name);
                if (e) {
                    vschema_from_schema(&e->schema, s);
                    e->has_schema = 1;
                }
                g_current_schema = s;
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

