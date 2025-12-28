#include "codegen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* -------------------------
   Mini "map" sources -> files
   ------------------------- */

typedef struct {
    char source_name[64];   // ex: "patients"
    char file_name[256];    // ex: "patients.csv"
} SourceFile;

static int collect_sources(ASTNode *root, SourceFile out[], int max_out) {
    int k = 0;
    for (ASTNode *n = root->left; n && k < max_out; n = n->next) {
        if (n->type == AST_SOURCE) {
            // n->text = source name
            // n->left->text = filename (STRING_LITERAL)
            strncpy(out[k].source_name, n->text, sizeof(out[k].source_name));
            out[k].source_name[sizeof(out[k].source_name) - 1] = '\0';

            const char *fname = (n->left ? n->left->text : "");
            strncpy(out[k].file_name, fname, sizeof(out[k].file_name));
            out[k].file_name[sizeof(out[k].file_name) - 1] = '\0';

            k++;
        }
    }
    return k;
}

static const char *lookup_file(SourceFile sources[], int n, const char *source_name) {
    for (int i = 0; i < n; i++) {
        if (strcmp(sources[i].source_name, source_name) == 0) {
            return sources[i].file_name;
        }
    }
    return NULL;
}

/* -------------------------
   C helpers emitted in afstat.c
   ------------------------- */

static void emit_helpers(FILE *out) {
    fprintf(out,
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <string.h>\n"
        "#include <math.h>\n\n"
        "#define MAX_LINE 4096\n"
        "#define MAX_COLS 256\n"
        "#define MAX_CATS 256\n"
        "#define MAX_STR  128\n\n"
        "static int split_csv_line(char *line, char *outv[], int max_out) {\n"
        "  int n = 0;\n"
        "  char *p = strtok(line, \",\");\n"
        "  while (p && n < max_out) {\n"
        "    size_t L = strlen(p);\n"
        "    while (L > 0 && (p[L-1] == '\\n' || p[L-1] == '\\r')) p[--L] = '\\0';\n"
        "    outv[n++] = p;\n"
        "    p = strtok(NULL, \",\");\n"
        "  }\n"
        "  return n;\n"
        "}\n\n"
        "static int find_col_index(const char *header_line, const char *colname) {\n"
        "  char tmp[MAX_LINE];\n"
        "  strncpy(tmp, header_line, sizeof(tmp));\n"
        "  tmp[sizeof(tmp)-1] = '\\0';\n"
        "  char *cols[MAX_COLS];\n"
        "  int n = split_csv_line(tmp, cols, MAX_COLS);\n"
        "  for (int i = 0; i < n; i++) {\n"
        "    if (strcmp(cols[i], colname) == 0) return i;\n"
        "  }\n"
        "  return -1;\n"
        "}\n\n"
    );
}

static void emit_main_begin(FILE *out) {
    fprintf(out, "int main(void) {\n");
    fprintf(out, "  printf(\"=== AFSTAT results ===\\n\");\n");
}

static void emit_main_end(FILE *out) {
    fprintf(out, "  return 0;\n");
    fprintf(out, "}\n");
}

/* -------------------------
   Analyze ops parsing: "mean age"
   ------------------------- */
static void parse_analyze_op(const char *text, char *op, size_t op_sz, char *col, size_t col_sz) {
    (void)op_sz; (void)col_sz;
    op[0] = '\0';
    col[0] = '\0';
    sscanf(text, "%31s %127s", op, col);
}

/* -------------------------
   Emit real computations in C
   ------------------------- */

static void emit_real_mean(FILE *out, const char *csv_file, const char *col) {
    fprintf(out,
        "  {\n"
        "    FILE *f = fopen(\"%s\", \"r\");\n"
        "    if (!f) { perror(\"fopen\"); return 1; }\n"
        "    char line[MAX_LINE];\n"
        "    if (!fgets(line, sizeof(line), f)) { fclose(f); return 1; }\n"
        "    int idx = find_col_index(line, \"%s\");\n"
        "    if (idx < 0) { printf(\"colonne introuvable: %s\\n\"); fclose(f); return 1; }\n"
        "    double sum = 0.0; long count = 0;\n"
        "    while (fgets(line, sizeof(line), f)) {\n"
        "      char tmp[MAX_LINE];\n"
        "      strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1] = '\\0';\n"
        "      char *cols[MAX_COLS];\n"
        "      int n = split_csv_line(tmp, cols, MAX_COLS);\n"
        "      if (idx < n && cols[idx][0] != '\\0') {\n"
        "        sum += atof(cols[idx]);\n"
        "        count++;\n"
        "      }\n"
        "    }\n"
        "    fclose(f);\n"
        "    printf(\"mean(%s) = %%f\\n\", (count > 0 ? sum/(double)count : 0.0));\n"
        "  }\n",
        csv_file, col, col, col
    );
}

static void emit_real_summary(FILE *out, const char *csv_file, const char *col) {
    fprintf(out,
        "  {\n"
        "    FILE *f = fopen(\"%s\", \"r\");\n"
        "    if (!f) { perror(\"fopen\"); return 1; }\n"
        "    char line[MAX_LINE];\n"
        "    if (!fgets(line, sizeof(line), f)) { fclose(f); return 1; }\n"
        "    int idx = find_col_index(line, \"%s\");\n"
        "    if (idx < 0) { printf(\"colonne introuvable: %s\\n\"); fclose(f); return 1; }\n"
        "    long nobs = 0;\n"
        "    double mean = 0.0, M2 = 0.0;\n"
        "    double minv = 0.0, maxv = 0.0;\n"
        "    while (fgets(line, sizeof(line), f)) {\n"
        "      char tmp[MAX_LINE];\n"
        "      strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1] = '\\0';\n"
        "      char *cols[MAX_COLS];\n"
        "      int n = split_csv_line(tmp, cols, MAX_COLS);\n"
        "      if (idx < n && cols[idx][0] != '\\0') {\n"
        "        double x = atof(cols[idx]);\n"
        "        if (nobs == 0) { minv = maxv = x; }\n"
        "        if (x < minv) minv = x;\n"
        "        if (x > maxv) maxv = x;\n"
        "        nobs++;\n"
        "        double delta = x - mean;\n"
        "        mean += delta / (double)nobs;\n"
        "        double delta2 = x - mean;\n"
        "        M2 += delta * delta2;\n"
        "      }\n"
        "    }\n"
        "    fclose(f);\n"
        "    double var = (nobs > 1 ? M2 / (double)(nobs - 1) : 0.0);\n"
        "    double stddev = sqrt(var);\n"
        "    printf(\"summary(%s): min=%%f max=%%f mean=%%f std_dev=%%f\\n\", minv, maxv, mean, stddev);\n"
        "  }\n",
        csv_file, col, col, col
    );
}

static void emit_real_histogram(FILE *out, const char *csv_file, const char *col) {
    fprintf(out,
        "  {\n"
        "    FILE *f = fopen(\"%s\", \"r\");\n"
        "    if (!f) { perror(\"fopen\"); return 1; }\n"
        "    char line[MAX_LINE];\n"
        "    if (!fgets(line, sizeof(line), f)) { fclose(f); return 1; }\n"
        "    int idx = find_col_index(line, \"%s\");\n"
        "    if (idx < 0) { printf(\"colonne introuvable: %s\\n\"); fclose(f); return 1; }\n"
        "    char cats[MAX_CATS][MAX_STR];\n"
        "    int counts[MAX_CATS];\n"
        "    int k = 0;\n"
        "    while (fgets(line, sizeof(line), f)) {\n"
        "      char tmp[MAX_LINE];\n"
        "      strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1] = '\\0';\n"
        "      char *cols[MAX_COLS];\n"
        "      int n = split_csv_line(tmp, cols, MAX_COLS);\n"
        "      if (idx < n) {\n"
        "        const char *v = cols[idx];\n"
        "        if (!v) continue;\n"
        "        int found = -1;\n"
        "        for (int i = 0; i < k; i++) {\n"
        "          if (strcmp(cats[i], v) == 0) { found = i; break; }\n"
        "        }\n"
        "        if (found == -1) {\n"
        "          if (k < MAX_CATS) {\n"
        "            strncpy(cats[k], v, MAX_STR);\n"
        "            cats[k][MAX_STR-1] = '\\0';\n"
        "            counts[k] = 1;\n"
        "            k++;\n"
        "          }\n"
        "        } else {\n"
        "          counts[found]++;\n"
        "        }\n"
        "      }\n"
        "    }\n"
        "    fclose(f);\n"
        "    printf(\"histogram(%s):\\n\");\n"
        "    for (int i = 0; i < k; i++) {\n"
        "      printf(\"  %%s: %%d\\n\", cats[i], counts[i]);\n"

        "    }\n"
        "  }\n",
        csv_file, col, col, col
    );
}

/* Fallback placeholder */
static void emit_placeholder(FILE *out, const char *dataset, const char *optext) {
    fprintf(out, "  printf(\"[dataset: %s] %s\\n\");\n", dataset, optext);
    fprintf(out, "  printf(\"  (valeurs fictives / dataset non associé à un CSV)\\n\");\n");
}

/* -------------------------
   Emit analyze block
   ------------------------- */

static void emit_analyze(FILE *out,
                         ASTNode *an,
                         SymbolTable *symtab,
                         SourceFile sources[],
                         int sources_n)
{
    const char *dataset = an->text;

    // On ne calcule réel que si dataset est un schema avec une source associée
    Schema *s = symtab_get_schema(symtab, dataset);
    const char *csv_file = NULL;

    if (s && s->source[0] != '\0') {
        csv_file = lookup_file(sources, sources_n, s->source);
    }

    fprintf(out, "  printf(\"[dataset: %s]\\n\");\n", dataset);

    for (ASTNode *opnode = an->left; opnode; opnode = opnode->next) {
        if (opnode->type != AST_ANALYZE_OP) continue;

        char op[32], col[128];
        parse_analyze_op(opnode->text, op, sizeof(op), col, sizeof(col));

        // Si pas de CSV (join/filter), on laisse placeholder
        if (!csv_file || csv_file[0] == '\0') {
            emit_placeholder(out, dataset, opnode->text);
            continue;
        }

        if (strcmp(op, "mean") == 0) {
            emit_real_mean(out, csv_file, col);
        } else if (strcmp(op, "summary") == 0) {
            emit_real_summary(out, csv_file, col);
        } else if (strcmp(op, "histogram") == 0) {
            emit_real_histogram(out, csv_file, col);
        } else {
            // Pour median/min/max/std_dev (pas demandé dans ta démo), on peut placeholder
            emit_placeholder(out, dataset, opnode->text);
        }
    }
}

/* -------------------------
   Public entrypoint
   ------------------------- */

int generate_c(ASTNode *root, SymbolTable *symtab, const char *out_c_path)
{
    if (!root || !symtab || !out_c_path) return 1;

    // Collecter sources -> fichiers depuis AST_SOURCE
    SourceFile sources[64];
    int sources_n = collect_sources(root, sources, 64);

    FILE *out = fopen(out_c_path, "w");
    if (!out) {
        perror("fopen(generate_c)");
        return 1;
    }

    emit_helpers(out);
    emit_main_begin(out);

    for (ASTNode *n = root->left; n; n = n->next) {
        if (n->type == AST_ANALYZE) {
            emit_analyze(out, n, symtab, sources, sources_n);
        }
    }

    emit_main_end(out);
    fclose(out);
    return 0;
}
