#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ast.h"
#include "symbol_table.h"
#include "semantic.h"
#include "codegen.h"

extern int yyparse(void);
extern ASTNode *root;

/* Le flag est défini dans lexer.l */
extern int print_tokens;

/* ------------------ Utils ------------------ */

static void usage(const char *prog) {
    fprintf(stderr,
        "Usage: %s [-version] [-tokens] [-tos] [-o <file.c>] < programme.afs\n"
        "  -version      Affiche les membres du projet\n"
        "  -tokens       Affiche les tokens produits par l'analyse lexicale\n"
        "  -tos          Affiche la table des symboles\n"
        "  -ast          Affiche l'AST\n"
        "  -o <file.c>   Fichier C genere (defaut: afstat.c)\n",
        prog
    );
}

static void print_version(void) {
    printf("Compilateur AFSTAT\n");
    printf("Membres du projet:\n");
    printf("- Ventadoux Nicolas\n");
    printf("- Hajjar Line\n");
    printf("- Akofa Carole DAPAH\n");
}

/* trim spaces inplace */
static void trim(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) s[--n] = '\0';
    size_t i = 0;
    while (s[i] && isspace((unsigned char)s[i])) i++;
    if (i > 0) memmove(s, s + i, strlen(s + i) + 1);
}

/* split "a:b" => a,b. returns 1 if split happened */
static int split_once(const char *in, char sep, char *a, size_t asz, char *b, size_t bsz) {
    const char *p = in ? strchr(in, sep) : NULL;
    if (!p) return 0;
    size_t la = (size_t)(p - in);
    size_t lb = strlen(p + 1);
    if (la + 1 > asz || lb + 1 > bsz) return 0;
    memcpy(a, in, la);
    a[la] = '\0';
    memcpy(b, p + 1, lb + 1);
    trim(a);
    trim(b);
    return 1;
}

/* ------------------ AST traversal helpers ------------------ */

static void walk(ASTNode *n, void (*fn)(ASTNode*, void*), void *ctx) {
    if (!n) return;
    fn(n, ctx);
    walk(n->left, fn, ctx);
    walk(n->right, fn, ctx);
    walk(n->next, fn, ctx);
}

typedef struct {
    SymbolTable *symtab;
} BuildCtx;

static void handle_source(ASTNode *n, BuildCtx *ctx) {
    if (!n || !ctx || !ctx->symtab) return;
    if (!n->text || !n->left || !n->left->text) return;
    symtab_add_source(ctx->symtab, n->text, n->left->text);
}

static void handle_schema(ASTNode *n, BuildCtx *ctx) {
    if (!n || !ctx || !ctx->symtab) return;
    const char *schema_name = n->text ? n->text : "";
    if (schema_name[0] == '\0') return;

    /* On cree le schema sans source pour l'instant (associe plus tard) */
    symtab_add_schema(ctx->symtab, schema_name, "");

    /* Champs */
    ASTNode *f = n->left; /* liste de fields via next */
    for (; f; f = f->next) {
        if (f->type != AST_FIELD) continue;

        char col[64] = {0}, typ[32] = {0};

        if (f->left && f->left->text) {
            /* format: FIELD(text="col", left->text="type") */
            snprintf(col, sizeof(col), "%s", f->text ? f->text : "");
            snprintf(typ, sizeof(typ), "%s", f->left->text);
            trim(col); trim(typ);
        } else if (f->text) {
            /* formats alternatifs: "col:type" ou "col type" */
            if (!split_once(f->text, ':', col, sizeof(col), typ, sizeof(typ))) {
                const char *sp = strchr(f->text, ' ');
                if (sp) {
                    size_t la = (size_t)(sp - f->text);
                    if (la < sizeof(col)) {
                        memcpy(col, f->text, la);
                        col[la] = '\0';
                        snprintf(typ, sizeof(typ), "%s", sp + 1);
                        trim(col); trim(typ);
                    }
                }
            }
        }

        if (col[0] && typ[0]) {
            symtab_add_field(ctx->symtab, schema_name, col, typ);
        }
    }
}

static void handle_associate(ASTNode *n, BuildCtx *ctx) {
    if (!n || !ctx || !ctx->symtab) return;

    char schema[64] = {0}, source[64] = {0};

    /* Cas principal: text=schema, left=source */
    if (n->text && n->left && n->left->text && (!n->right || !n->right->text)) {
        snprintf(schema, sizeof(schema), "%s", n->text);
        snprintf(source, sizeof(source), "%s", n->left->text);
        trim(schema); trim(source);
    }
    else if (n->left && n->left->text && n->right && n->right->text) {
        snprintf(schema, sizeof(schema), "%s", n->left->text);
        snprintf(source, sizeof(source), "%s", n->right->text);
        trim(schema); trim(source);
    }
    else if (n->text) {
        if (!split_once(n->text, '=', schema, sizeof(schema), source, sizeof(source)) &&
            !split_once(n->text, ':', schema, sizeof(schema), source, sizeof(source))) {

            const char *sp = strchr(n->text, ' ');
            if (sp) {
                size_t la = (size_t)(sp - n->text);
                if (la < sizeof(schema)) {
                    memcpy(schema, n->text, la);
                    schema[la] = '\0';
                    snprintf(source, sizeof(source), "%s", sp + 1);
                    trim(schema); trim(source);
                }
            }
        }
    }

    if (schema[0] && source[0]) {
        symtab_associate(ctx->symtab, schema, source);
    } else {
        fprintf(stderr, "Warning: could not parse ASSOC node (text='%s').\n",
                n->text ? n->text : "(null)");
    }
}

static void handle_virtual(ASTNode *n, BuildCtx *ctx) {
    if (!n || !ctx || !ctx->symtab) return;
    if (!n->text || n->text[0] == '\0') return;

    if (n->type == AST_JOIN || n->type == AST_FILTER) {
        if (!symtab_get_schema(ctx->symtab, n->text)) {
            symtab_add_empty_schema(ctx->symtab, n->text);
        }
    }
}

static void build_symtab_from_ast(ASTNode *root_ast, SymbolTable *symtab) {
    BuildCtx ctx = {.symtab = symtab};

    /* Pass 1: schemas + fields + sources */
    void pass1(ASTNode *n, void *c) {
        BuildCtx *bc = (BuildCtx*)c;
        if (n->type == AST_SCHEMA) handle_schema(n, bc);
        if (n->type == AST_SOURCE) handle_source(n, bc);
    }
    walk(root_ast, pass1, &ctx);

    /* Pass 2: associate */
    void pass2(ASTNode *n, void *c) {
        BuildCtx *bc = (BuildCtx*)c;
        if (n->type == AST_ASSOCIATE) handle_associate(n, bc);
    }
    walk(root_ast, pass2, &ctx);

    /* Pass 3: virtual datasets (join/filter placeholders) */
    void pass3(ASTNode *n, void *c) {
        BuildCtx *bc = (BuildCtx*)c;
        if (n->type == AST_JOIN || n->type == AST_FILTER) handle_virtual(n, bc);
    }
    walk(root_ast, pass3, &ctx);
}

/* ------------------ main ------------------ */

int main(int argc, char **argv) {
    int opt_version = 0;
    int opt_tos = 0;
    int opt_tokens = 0;
    int opt_ast = 0;
    const char *out_c = "afstat.c";

    /* Parse options */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-version") == 0) opt_version = 1;
        else if (strcmp(argv[i], "-tos") == 0) opt_tos = 1;
        else if (strcmp(argv[i], "-tokens") == 0) opt_tokens = 1;
        else if (strcmp(argv[i], "-ast") == 0) opt_ast = 1;
        else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Erreur: -o requiert un nom de fichier.\n");
                usage(argv[0]);
                return 2;
            }
            out_c = argv[++i];
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Option inconnue: %s\n", argv[i]);
            usage(argv[0]);
            return 2;
        }
    }

    /* Options sans lecture d'entrée */
    if (opt_version) {
        print_version();
        return 0;
    }

    /* Active l'affichage des tokens AVANT yyparse() */
    if (opt_tokens) {
        print_tokens = 1;
        printf("----------------------------------------------------\n");
        printf("TOKENS – Analyse lexicale\n");
        printf("----------------------------------------------------\n");
    }

    SymbolTable *symtab = symtab_create();
    if (!symtab) {
        fprintf(stderr, "Erreur: symtab_create() a echoue.\n");
        return 1;
    }

    /* Parse */
 /* Parse */
if (yyparse() != 0 || root == NULL) {
    fprintf(stderr, "Parsing failed.\n");
    return 1;
}

if (opt_ast) {
    printf("=== AST ===\n");
    ast_print(root, 0);
}



    if (opt_tokens) {
        printf("\n----------------------------------------------------\n");
    }

    /* Construire la table des symboles à partir de l’AST */
    build_symtab_from_ast(root, symtab);

    /* Analyse sémantique */
    if (semantic_check(root, symtab) != 0) {
        fprintf(stderr, "Semantic errors.\n");
        return 1;
    }

    /* Affichage TOS (si demandé) */
    if (opt_tos) {
        symtab_print(symtab);
    }

    /* Génération C */
    if (generate_c(root, symtab, out_c) != 0) {
        fprintf(stderr, "Code generation failed.\n");
        return 1;
    }

    printf("Generated C file: %s\n", out_c);
    return 0;
}
