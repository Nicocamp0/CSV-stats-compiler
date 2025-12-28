#include "codegen.h"
#include <stdio.h>
#include <string.h>

static void emit_prelude(FILE *out) {
    fprintf(out, "#include <stdio.h>\n\n");
}

static void emit_main_begin(FILE *out) {
    fprintf(out, "int main(void) {\n");
    fprintf(out, "  printf(\"=== AFSTAT results ===\\n\");\n");
}

static void emit_main_end(FILE *out) {
    fprintf(out, "  return 0;\n");
    fprintf(out, "}\n");
}

static void parse_analyze_op(const char *text, char *op, size_t op_sz, char *col, size_t col_sz) {
    // text format: "mean age" / "summary weight" / "histogram gender"
    op[0] = col[0] = '\0';
    sscanf(text, "%31s %127s", op, col);
    (void)op_sz; (void)col_sz;
}

static void emit_analyze(FILE *out, ASTNode *an, SymbolTable *symtab) {
    const char *dataset = an->text;
    fprintf(out, "  printf(\"[dataset: %s]\\n\");\n", dataset);

    // Optionnel : vérifier que dataset existe (normalement déjà fait en sémantique)
    Schema *s = symtab_get_schema(symtab, dataset);
    if (!s) {
        // On ne stoppe pas : on génère quand même un C compilable
        fprintf(out, "  printf(\"(warning) dataset '%s' introuvable dans la table des symboles\\n\");\n", dataset);
    }

    for (ASTNode *opnode = an->left; opnode; opnode = opnode->next) {
        if (opnode->type != AST_ANALYZE_OP) continue;

        char op[32], col[128];
        parse_analyze_op(opnode->text, op, sizeof(op), col, sizeof(col));

        if (strcmp(op, "mean") == 0) {
            fprintf(out, "  printf(\"mean(%s) = %%f\\n\", 0.0);\n", col);
        } else if (strcmp(op, "summary") == 0) {
            fprintf(out,
                "  printf(\"summary(%s): min=%%f max=%%f mean=%%f std_dev=%%f\\n\", 0.0, 0.0, 0.0, 0.0);\n",
                col
            );
        } else if (strcmp(op, "histogram") == 0) {
            fprintf(out, "  printf(\"histogram(%s):\\n\");\n", col);
            fprintf(out, "  printf(\"  A: %%d\\n\", 0);\n");
            fprintf(out, "  printf(\"  B: %%d\\n\", 0);\n");
        } else if (strcmp(op, "median") == 0) {
            fprintf(out, "  printf(\"median(%s) = %%f\\n\", 0.0);\n", col);
        } else if (strcmp(op, "std_dev") == 0) {
            fprintf(out, "  printf(\"std_dev(%s) = %%f\\n\", 0.0);\n", col);
        } else if (strcmp(op, "min") == 0) {
            fprintf(out, "  printf(\"min(%s) = %%f\\n\", 0.0);\n", col);
        } else if (strcmp(op, "max") == 0) {
            fprintf(out, "  printf(\"max(%s) = %%f\\n\", 0.0);\n", col);
        } else {
            fprintf(out, "  printf(\"(not implemented) %s\\n\");\n", opnode->text);
        }
    }
}

int generate_c(ASTNode *root, SymbolTable *symtab, const char *out_c_path) {
    if (!root || !out_c_path) return 1;

    FILE *out = fopen(out_c_path, "w");
    if (!out) {
        perror("fopen(generate_c)");
        return 1;
    }

    emit_prelude(out);
    emit_main_begin(out);

    // Parcours des statements: root->left est la liste (cons)
    for (ASTNode *n = root->left; n; n = n->next) {
        if (n->type == AST_ANALYZE) {
            emit_analyze(out, n, symtab);
        }
    }

    emit_main_end(out);
    fclose(out);
    return 0;
}
