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

typedef struct {
    char name[64];     // nom colonne calculée (ex: bmi)
    ASTNode *expr;     // AST de l’expression
} ComputedDef;

static ComputedDef g_computed[128];
static int g_computed_n = 0;

static void add_computed_def(const char *name, ASTNode *expr) {
    if (g_computed_n >= 128) return;
    strncpy(g_computed[g_computed_n].name, name, sizeof(g_computed[g_computed_n].name)-1);
    g_computed[g_computed_n].name[sizeof(g_computed[g_computed_n].name)-1] = '\0';
    g_computed[g_computed_n].expr = expr;
    g_computed_n++;
}

typedef struct {
    char dataset[64];
    char csv[256];      
} DatasetCSV;

static const char *ds_lookup(DatasetCSV m[], int n, const char *dataset) {
    for (int i = 0; i < n; i++) {
        if (strcmp(m[i].dataset, dataset) == 0) return m[i].csv;
    }
    return NULL;
}

static void ds_set(DatasetCSV m[], int *n, int maxn, const char *dataset, const char *csv) {
    for (int i = 0; i < *n; i++) {
        if (strcmp(m[i].dataset, dataset) == 0) {
            strncpy(m[i].csv, csv, sizeof(m[i].csv));
            m[i].csv[sizeof(m[i].csv)-1] = '\0';
            return;
        }
    }
    if (*n >= maxn) return;
    strncpy(m[*n].dataset, dataset, sizeof(m[*n].dataset));
    m[*n].dataset[sizeof(m[*n].dataset)-1] = '\0';
    strncpy(m[*n].csv, csv, sizeof(m[*n].csv));
    m[*n].csv[sizeof(m[*n].csv)-1] = '\0';
    (*n)++;
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
        "static int header_find_index(const char *header_line, const char *colname) {\n"
        "  char tmp[MAX_LINE];\n"
        "  strncpy(tmp, header_line, sizeof(tmp));\n"
        "  tmp[sizeof(tmp)-1] = '\\0';\n"
        "  int idx = 0;\n"
        "  char *save = NULL;\n"
        "  for (char *tok = strtok_r(tmp, \",\\r\\n\", &save); tok; tok = strtok_r(NULL, \",\\r\\n\", &save)) {\n"
        "    if (strcmp(tok, colname) == 0) return idx;\n"
        "    idx++;\n"
        "  }\n"
        "  return -1;\n"
        "}\n\n"
        "static int csv_split_line(char *line, char *cols[], int max_cols) {\n"
        "  int n = 0;\n"
        "  char *save = NULL;\n"
        "  for (char *tok = strtok_r(line, \",\\r\\n\", &save); tok && n < max_cols; tok = strtok_r(NULL, \",\\r\\n\", &save)) {\n"
        "    cols[n++] = tok;\n"
        "  }\n"
        "  return n;\n"
        "}\n\n"
        "static int cmp_double(const void *a, const void *b) {\n"
        "  double da = *(const double*)a;\n"
        "  double db = *(const double*)b;\n"
        "  return (da > db) - (da < db);\n"
        "}\n\n"
        "typedef double (*ComputeFn)(const char *header, char *cols[], int n);\n"
        "typedef struct { const char *name; ComputeFn fn; } ComputeEntry;\n"
        "static ComputeEntry g_comp[128];\n"
        "static int g_comp_n = 0;\n"
        "static void register_comp(const char *name, ComputeFn fn) {\n"
        "  if (g_comp_n < 128) { g_comp[g_comp_n].name = name; g_comp[g_comp_n].fn = fn; g_comp_n++; }\n"
        "}\n"
        "static ComputeFn find_comp(const char *name) {\n"
        "  for (int i=0;i<g_comp_n;i++) if (strcmp(g_comp[i].name,name)==0) return g_comp[i].fn;\n"
        "  return NULL;\n"
        "}\n"
        "static int get_value(const char *header, char *cols[], int n, const char *colname, double *outv) {\n"
        "  int idx = find_col_index(header, colname);\n"
        "  if (idx >= 0) {\n"
        "    if (idx < n && cols[idx] && cols[idx][0] != '\\0') { *outv = atof(cols[idx]); return 1; }\n"
        "    return 0;\n"
        "  }\n"
        "  ComputeFn fn = find_comp(colname);\n"
        "  if (fn) { *outv = fn(header, cols, n); return 1; }\n"
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

static void emit_expr_as_c(FILE *out, ASTNode *e) {
    if (!e) { fprintf(out, "0.0"); return; }

    // feuilles: numbers / ident / string -> on supporte surtout number + ident
    if (e->left == NULL && e->right == NULL) {
        // Si c’est un opérateur stocké comme texte "+", etc, ça arrivera plutôt en non-feuille
        // Ici on suppose: e->text contient soit un nombre soit un ident
        // Heuristique simple: si ça commence par chiffre ou '-' -> nombre
        const char *t = e->text ? e->text : "0";
        if ((t[0] >= '0' && t[0] <= '9') || t[0] == '-' ) {
            fprintf(out, "%s", t);
        } else {
            // ident: on récupère via get_value(...)
            // On génère: ({ double __tmp; int __r=get_value(header,cols,n,"age",&__tmp); if(__r<=0){return 0.0;} __tmp; })
            fprintf(out,
              "({ double __tmp=0.0; int __r=get_value(header, cols, n, \"%s\", &__tmp);"
              " if(__r<=0){ return 0.0; } __tmp; })",
              t
            );
        }
        return;
    }

    // noeud opérateur: e->text = "+", "-", "*", "/"
    fprintf(out, "(");
    emit_expr_as_c(out, e->left);
    fprintf(out, " %s ", e->text ? e->text : "+");
    emit_expr_as_c(out, e->right);
    fprintf(out, ")");
}

static void emit_computed_functions(FILE *out) {
    for (int i = 0; i < g_computed_n; i++) {
        const char *name = g_computed[i].name;

        fprintf(out, "static double comp_%s(const char *header, char *cols[], int n) {\n", name);
        fprintf(out, "  (void)header; (void)cols; (void)n;\n");
        fprintf(out, "  return ");
        emit_expr_as_c(out, g_computed[i].expr);
        fprintf(out, ";\n}\n\n");
    }
}

static void emit_register_computed(FILE *out) {
    for (int i = 0; i < g_computed_n; i++) {
        fprintf(out, "  register_comp(\"%s\", comp_%s);\n", g_computed[i].name, g_computed[i].name);
    }
}

static void collect_computes(ASTNode *n) {
    if (!n) return;
    if (n->type == AST_COMPUTE) {
        // d’après ton parser: AST_COMPUTE, text=$2 (nom), left=$4 (expr)
        add_computed_def(n->text, n->left);
    }
    collect_computes(n->left);
    collect_computes(n->right);
    collect_computes(n->next);
}


/* -------------------------
   Emit real computations in C
   ------------------------- */

static void emit_real_min(FILE *out, const char *csv_file, const char *col) {
    fprintf(out,
        "  {\n"
        "    FILE *f = fopen(\"%s\", \"r\");\n"
        "    if (!f) { perror(\"fopen\"); return 1; }\n"
        "    char line[MAX_LINE];\n"
        "    if (!fgets(line, sizeof(line), f)) { fclose(f); return 1; }\n"
        "    char header_line[MAX_LINE];\n"
        "    strncpy(header_line, line, sizeof(header_line)); header_line[sizeof(header_line)-1] = '\\0';\n"
        "    long nobs = 0;\n"
        "    double minv = 0.0;\n"
        "    while (fgets(line, sizeof(line), f)) {\n"
        "      char tmp[MAX_LINE];\n"
        "      strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1] = '\\0';\n"
        "      char *cols[MAX_COLS];\n"
        "      int n = split_csv_line(tmp, cols, MAX_COLS);\n"
        "      double x = 0.0;\n"
        "      int r = get_value(header_line, cols, n, \"%s\", &x);\n"
        "      if (r == -1) { printf(\"colonne introuvable: %s\\n\"); fclose(f); return 1; }\n"
        "      if (r == 1) {\n"
        "        if (nobs == 0 || x < minv) minv = x;\n"
        "        nobs++;\n"
        "      }\n"
        "    }\n"
        "    fclose(f);\n"
        "    if (nobs == 0) printf(\"min(%s) = (no data)\\n\");\n"
        "    else printf(\"min(%s) = %%f\\n\", minv);\n"
        "  }\n",
        csv_file, col, col, col, col
    );
}


static void emit_real_max(FILE *out, const char *csv_file, const char *col) {
    fprintf(out,
        "  {\n"
        "    FILE *f = fopen(\"%s\", \"r\");\n"
        "    if (!f) { perror(\"fopen\"); return 1; }\n"
        "    char line[MAX_LINE];\n"
        "    if (!fgets(line, sizeof(line), f)) { fclose(f); return 1; }\n"
        "    char header_line[MAX_LINE];\n"
        "    strncpy(header_line, line, sizeof(header_line)); header_line[sizeof(header_line)-1] = '\\0';\n"
        "    long nobs = 0;\n"
        "    double maxv = 0.0;\n"
        "    while (fgets(line, sizeof(line), f)) {\n"
        "      char tmp[MAX_LINE];\n"
        "      strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1] = '\\0';\n"
        "      char *cols[MAX_COLS];\n"
        "      int n = split_csv_line(tmp, cols, MAX_COLS);\n"
        "      double x = 0.0;\n"
        "      int r = get_value(header_line, cols, n, \"%s\", &x);\n"
        "      if (r == -1) { printf(\"colonne introuvable: %s\\n\"); fclose(f); return 1; }\n"
        "      if (r == 1) {\n"
        "        if (nobs == 0 || x > maxv) maxv = x;\n"
        "        nobs++;\n"
        "      }\n"
        "    }\n"
        "    fclose(f);\n"
        "    if (nobs == 0) printf(\"max(%s) = (no data)\\n\");\n"
        "    else printf(\"max(%s) = %%f\\n\", maxv);\n"
        "  }\n",
        csv_file, col, col, col, col
    );
}


static void emit_real_stddev(FILE *out, const char *csv_file, const char *col) {
    fprintf(out,
        "  {\n"
        "    FILE *f = fopen(\"%s\", \"r\");\n"
        "    if (!f) { perror(\"fopen\"); return 1; }\n"
        "    char line[MAX_LINE];\n"
        "    if (!fgets(line, sizeof(line), f)) { fclose(f); return 1; }\n"
        "    char header_line[MAX_LINE];\n"
        "    strncpy(header_line, line, sizeof(header_line)); header_line[sizeof(header_line)-1] = '\\0';\n"
        "    long nobs = 0;\n"
        "    double mean = 0.0, M2 = 0.0;\n"
        "    while (fgets(line, sizeof(line), f)) {\n"
        "      char tmp[MAX_LINE];\n"
        "      strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1] = '\\0';\n"
        "      char *cols[MAX_COLS];\n"
        "      int n = split_csv_line(tmp, cols, MAX_COLS);\n"
        "      double x = 0.0;\n"
        "      int r = get_value(header_line, cols, n, \"%s\", &x);\n"
        "      if (r == -1) { printf(\"colonne introuvable: %s\\n\"); fclose(f); return 1; }\n"
        "      if (r == 1) {\n"
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
        "    printf(\"std_dev(%s) = %%f\\n\", stddev);\n"
        "  }\n",
        csv_file, col, col, col
    );
}


static void emit_real_mean(FILE *out, const char *csv_file, const char *col) {
    fprintf(out,
        "  {\n"
        "    FILE *f = fopen(\"%s\", \"r\");\n"
        "    if (!f) { perror(\"fopen\"); return 1; }\n"
        "    char line[MAX_LINE];\n"
        "    if (!fgets(line, sizeof(line), f)) { fclose(f); return 1; }\n"
        "    char header_line[MAX_LINE];\n"
        "    strncpy(header_line, line, sizeof(header_line)); header_line[sizeof(header_line)-1] = '\\0';\n"
        "    double sum = 0.0; long count = 0;\n"
        "    while (fgets(line, sizeof(line), f)) {\n"
        "      char tmp[MAX_LINE];\n"
        "      strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1] = '\\0';\n"
        "      char *cols[MAX_COLS];\n"
        "      int n = split_csv_line(tmp, cols, MAX_COLS);\n"
        "      double x = 0.0;\n"
        "      int r = get_value(header_line, cols, n, \"%s\", &x);\n"
        "      if (r == -1) { printf(\"colonne introuvable: %s\\n\"); fclose(f); return 1; }\n"
        "      if (r == 1) { sum += x; count++; }\n"
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
        "    char header_line[MAX_LINE];\n"
        "    strncpy(header_line, line, sizeof(header_line)); header_line[sizeof(header_line)-1] = '\\0';\n"
        "    long nobs = 0;\n"
        "    double mean = 0.0, M2 = 0.0;\n"
        "    double minv = 0.0, maxv = 0.0;\n"
        "    while (fgets(line, sizeof(line), f)) {\n"
        "      char tmp[MAX_LINE];\n"
        "      strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1] = '\\0';\n"
        "      char *cols[MAX_COLS];\n"
        "      int n = split_csv_line(tmp, cols, MAX_COLS);\n"
        "      double x = 0.0;\n"
        "      int r = get_value(header_line, cols, n, \"%s\", &x);\n"
        "      if (r == -1) { printf(\"colonne introuvable: %s\\n\"); fclose(f); return 1; }\n"
        "      if (r == 1) {\n"
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
        "    if (nobs == 0) { printf(\"summary(%s): (no data)\\n\"); }\n"
        "    else {\n"
        "      double var = (nobs > 1 ? M2 / (double)(nobs - 1) : 0.0);\n"
        "      double stddev = sqrt(var);\n"
        "      printf(\"summary(%s): min=%%f max=%%f mean=%%f std_dev=%%f\\n\", minv, maxv, mean, stddev);\n"
        "    }\n"
        "  }\n",
        csv_file, col, col, col, col
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
        "      if (idx < n && cols[idx]) {\n"
        "        const char *v = cols[idx];\n"
        "        int found = -1;\n"
        "        for (int i = 0; i < k; i++) if (strcmp(cats[i], v) == 0) { found = i; break; }\n"
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
        "    for (int i = 0; i < k; i++) printf(\"  %%s: %%d\\n\", cats[i], counts[i]);\n"
        "  }\n",
        csv_file, col, col, col
    );
}



static void emit_real_median(FILE *out, const char *csv_file, const char *col) {
    fprintf(out,
        "  {\n"
        "    FILE *f = fopen(\"%s\", \"r\");\n"
        "    if (!f) { perror(\"fopen\"); return 1; }\n"
        "    char line[MAX_LINE];\n"
        "    if (!fgets(line, sizeof(line), f)) { fclose(f); return 1; }\n"
        "    char header_line[MAX_LINE];\n"
        "    strncpy(header_line, line, sizeof(header_line)); header_line[sizeof(header_line)-1] = '\\0';\n"
        "    size_t cap = 128;\n"
        "    size_t nobs = 0;\n"
        "    double *vals = (double*)malloc(cap * sizeof(double));\n"
        "    if (!vals) { fclose(f); return 1; }\n"
        "    while (fgets(line, sizeof(line), f)) {\n"
        "      char tmp[MAX_LINE];\n"
        "      strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1] = '\\0';\n"
        "      char *cols[MAX_COLS];\n"
        "      int n = split_csv_line(tmp, cols, MAX_COLS);\n"
        "      double x = 0.0;\n"
        "      int r = get_value(header_line, cols, n, \"%s\", &x);\n"
        "      if (r == -1) { printf(\"colonne introuvable: %s\\n\"); free(vals); fclose(f); return 1; }\n"
        "      if (r == 1) {\n"
        "        if (nobs == cap) {\n"
        "          cap *= 2;\n"
        "          double *nv = (double*)realloc(vals, cap * sizeof(double));\n"
        "          if (!nv) { free(vals); fclose(f); return 1; }\n"
        "          vals = nv;\n"
        "        }\n"
        "        vals[nobs++] = x;\n"
        "      }\n"
        "    }\n"
        "    fclose(f);\n"
        "    if (nobs == 0) { printf(\"median(%s) = (no data)\\n\"); free(vals); }\n"
        "    else {\n"
        "      qsort(vals, nobs, sizeof(double), cmp_double);\n"
        "      double med;\n"
        "      if (nobs %% 2 == 1) med = vals[nobs/2];\n"
        "      else med = 0.5 * (vals[nobs/2 - 1] + vals[nobs/2]);\n"
        "      printf(\"median(%s) = %%f\\n\", med);\n"
        "      free(vals);\n"
        "    }\n"
        "  }\n",
        csv_file, col, col, col, col
    );
}

static void emit_join_csv(FILE *out,
    const char *out_csv,
    const char *left_csv,
    const char *right_csv,
    const char *left_key_col,
    const char *right_key_col)
{
    fprintf(out,
    "  {\n"
    "    // ---- JOIN (inner) ----\n"
    "    FILE *fl = fopen(\"%s\", \"r\");\n"
    "    FILE *fr = fopen(\"%s\", \"r\");\n"
    "    FILE *fo = fopen(\"%s\", \"w\");\n"
    "    if (!fl || !fr || !fo) { perror(\"fopen(join)\"); if(fl)fclose(fl); if(fr)fclose(fr); if(fo)fclose(fo); return 1; }\n"
    "\n"
    "    char hl[MAX_LINE], hr[MAX_LINE];\n"
    "    if (!fgets(hl, sizeof(hl), fl)) { fclose(fl); fclose(fr); fclose(fo); return 1; }\n"
    "    if (!fgets(hr, sizeof(hr), fr)) { fclose(fl); fclose(fr); fclose(fo); return 1; }\n"
    "\n"
    "    // header -> indices\n"
    "    int lk = header_find_index(hl, \"%s\");\n"
    "    int rk = header_find_index(hr, \"%s\");\n"
    "    if (lk < 0 || rk < 0) {\n"
    "      printf(\"join: key not found (left=%s right=%s)\\n\");\n"
    "      fclose(fl); fclose(fr); fclose(fo); return 1;\n"
    "    }\n"
    "\n"
    "    // write output header: left header + right header (simple)\n"
    "    // remove trailing newline from left header\n"
    "    { size_t L = strlen(hl); while (L>0 && (hl[L-1]=='\\n'||hl[L-1]=='\\r')) hl[--L]='\\0'; }\n"
    "    { size_t L = strlen(hr); while (L>0 && (hr[L-1]=='\\n'||hr[L-1]=='\\r')) hr[--L]='\\0'; }\n"
    "    fprintf(fo, \"%%s,%%s\\n\", hl, hr);\n"
    "\n"
    "    // Build a simple in-memory index for RIGHT file: key -> full line\n"
    "    // (naive array; OK for small datasets)\n"
    "    typedef struct { char key[128]; char line[MAX_LINE]; } Row;\n"
    "    Row *rows = NULL;\n"
    "    size_t cap = 0, nr = 0;\n"
    "    char line[MAX_LINE];\n"
    "    while (fgets(line, sizeof(line), fr)) {\n"
    "      char tmp[MAX_LINE]; strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1]='\\0';\n"
    "      char *cols[MAX_COLS];\n"
    "      int n = csv_split_line(tmp, cols, MAX_COLS);\n"
    "      if (rk >= n) continue;\n"
    "      const char *key = cols[rk];\n"
    "      if (nr == cap) { cap = (cap==0?128:cap*2); rows = (Row*)realloc(rows, cap*sizeof(Row)); if(!rows){ fclose(fl); fclose(fr); fclose(fo); return 1; } }\n"
    "      strncpy(rows[nr].key, key, sizeof(rows[nr].key)); rows[nr].key[sizeof(rows[nr].key)-1]='\\0';\n"
    "      // store original line without newline\n"
    "      { size_t L = strlen(line); while (L>0 && (line[L-1]=='\\n'||line[L-1]=='\\r')) line[--L]='\\0'; }\n"
    "      strncpy(rows[nr].line, line, sizeof(rows[nr].line)); rows[nr].line[sizeof(rows[nr].line)-1]='\\0';\n"
    "      nr++;\n"
    "    }\n"
    "\n"
    "    // For each LEFT row, find matching RIGHT row(s) and write join\n"
    "    while (fgets(line, sizeof(line), fl)) {\n"
    "      char leftline[MAX_LINE];\n"
    "      strncpy(leftline, line, sizeof(leftline)); leftline[sizeof(leftline)-1]='\\0';\n"
    "      // key extraction\n"
    "      char tmp[MAX_LINE]; strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1]='\\0';\n"
    "      char *cols[MAX_COLS];\n"
    "      int n = csv_split_line(tmp, cols, MAX_COLS);\n"
    "      if (lk >= n) continue;\n"
    "      const char *key = cols[lk];\n"
    "      // strip newline from leftline\n"
    "      { size_t L = strlen(leftline); while (L>0 && (leftline[L-1]=='\\n'||leftline[L-1]=='\\r')) leftline[--L]='\\0'; }\n"
    "\n"
    "      for (size_t i = 0; i < nr; i++) {\n"
    "        if (strcmp(rows[i].key, key) == 0) {\n"
    "          fprintf(fo, \"%%s,%%s\\n\", leftline, rows[i].line);\n"
    "        }\n"
    "      }\n"
    "    }\n"
    "\n"
    "    free(rows);\n"
    "    fclose(fl); fclose(fr); fclose(fo);\n"
    "  }\n",
    left_csv, right_csv, out_csv,
    left_key_col, right_key_col,
    left_key_col, right_key_col
    );
}


static void emit_filter_csv(FILE *out,
    const char *out_csv,
    const char *in_csv,
    const char *qualified_col,
    const char *op,           
    const char *rhs_literal)   
{
    const char *dot = strrchr(qualified_col, '.');
    const char *col = dot ? dot + 1 : qualified_col;

    fprintf(out,
        "  {\n"
        "    // ---- FILTER ----\n"
        "    FILE *fin = fopen(\"%s\", \"r\");\n"
        "    if (!fin) { perror(\"fopen(filter in)\"); return 1; }\n"
        "    FILE *fout = fopen(\"%s\", \"w\");\n"
        "    if (!fout) { perror(\"fopen(filter out)\"); fclose(fin); return 1; }\n"
        "    char header[MAX_LINE];\n"
        "    if (!fgets(header, sizeof(header), fin)) { fclose(fin); fclose(fout); return 1; }\n"
        "    fputs(header, fout);\n"
        "    int idx = header_find_index(header, \"%s\");\n"
        "    if (idx < 0) {\n"
        "      fprintf(stderr, \"filter: column not found:\\n\");\n"
        "      fclose(fin); fclose(fout); return 1;\n"
        "    }\n"
        "    char line[MAX_LINE];\n"
        "    while (fgets(line, sizeof(line), fin)) {\n"
        "      char tmp[MAX_LINE];\n"
        "      strncpy(tmp, line, sizeof(tmp)); tmp[sizeof(tmp)-1] = '\\0';\n"
        "      char *cols[MAX_COLS];\n"
        "      int n = csv_split_line(tmp, cols, MAX_COLS);\n"
        "      if (idx >= n) continue;\n"
        "      const char *v = cols[idx];\n"
        "      int keep = 0;\n"
        "      double x = atof(v);\n"
        "      double y = %s;\n"
        "      if (strcmp(\"%s\", \">\") == 0) keep = (x > y);\n"
        "      else if (strcmp(\"%s\", \"<\") == 0) keep = (x < y);\n"
        "      else if (strcmp(\"%s\", \">=\") == 0) keep = (x >= y);\n"
        "      else if (strcmp(\"%s\", \"<=\") == 0) keep = (x <= y);\n"
        "      else if (strcmp(\"%s\", \"==\") == 0) keep = (x == y);\n"
        "      else if (strcmp(\"%s\", \"!=\") == 0) keep = (x != y);\n"
        "      if (keep) fputs(line, fout);\n"
        "    }\n"
        "    fclose(fin); fclose(fout);\n"
        "  }\n",
        in_csv, out_csv,
        col,
        rhs_literal,                    
        op, op, op, op, op, op
    );
}


/* Fallback placeholder */
static void emit_placeholder(FILE *out, const char *dataset, const char *optext) {
    fprintf(out, "  printf(\"[dataset: %s] %s\\n\");\n", dataset, optext);
    fprintf(out, "  printf(\"  (valeurs fictives / dataset non associé à un CSV)\\n\");\n");
}


static void emit_analyze(FILE *out,
                                    ASTNode *an,
                                    SymbolTable *symtab,
                                    SourceFile sources[],
                                    int sources_n,
                                    DatasetCSV dsmap[],
                                    int dsmap_n)

{
    const char *dataset = an->text;
    const char *csv_file = ds_lookup(dsmap, dsmap_n, dataset);

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
        } else if (strcmp(op, "min") == 0) {
            emit_real_min(out, csv_file, col);
        } else if (strcmp(op, "max") == 0) {
            emit_real_max(out, csv_file, col);
        } else if (strcmp(op, "median") == 0) {
            emit_real_median(out, csv_file, col);
        } else if (strcmp(op, "std_dev") == 0 || strcmp(op, "stddev") == 0) {
            emit_real_stddev(out, csv_file, col);
        } else if (strcmp(op, "summary") == 0) {
            emit_real_summary(out, csv_file, col);
        } else if (strcmp(op, "histogram") == 0) {
            emit_real_histogram(out, csv_file, col);
        } else {
            emit_placeholder(out, dataset, opnode->text);
        }
        
    }
}

static void emit_pipeline(FILE *out,
    ASTNode *root,
    SymbolTable *symtab,
    SourceFile sources[],
    int sources_n)
{
    (void)symtab; (void)sources; (void)sources_n;

    DatasetCSV dsmap[128];
    int dsmap_n = 0;

    for (ASTNode *st = root->left; st; st = st->next) {
        if (st->type == AST_SOURCE) {
            const char *name = st->text;
            const char *fname = (st->left ? st->left->text : "");
            ds_set(dsmap, &dsmap_n, 128, name, fname);
        }

        else if (st->type == AST_ASSOCIATE) {
            const char *schema_name = st->text; 
            const char *source_name = (st->left ? st->left->text : "");
            const char *csv = ds_lookup(dsmap, dsmap_n, source_name);
            if (!csv) {
                fprintf(out, "  printf(\"associate: source not found: %s\\n\");\n", source_name);
                continue;
            }
            ds_set(dsmap, &dsmap_n, 128, schema_name, csv);
        }
        
        else if (st->type == AST_JOIN) {
            const char *out_ds = st->text;
        
            const char *left_ref  = (st->left  ? st->left->text  : "");
            const char *right_ref = (st->right ? st->right->text : ""); 
        
            char left_src[64] = {0}, left_col[64] = {0};
            char right_src[64] = {0}, right_col[64] = {0};
            sscanf(left_ref,  "%63[^.].%63s", left_src, left_col);
            sscanf(right_ref, "%63[^.].%63s", right_src, right_col);
        
            const char *left_csv  = ds_lookup(dsmap, dsmap_n, left_src);
            const char *right_csv = ds_lookup(dsmap, dsmap_n, right_src);
        
            if (!left_csv || !right_csv) {
                fprintf(out, "  printf(\"join: missing input csv (%s or %s)\\n\");\n", left_src, right_src);
                continue;
            }
        
            char tmpcsv[256];
            snprintf(tmpcsv, sizeof(tmpcsv), "__af_join_%s.csv", out_ds);
        
            emit_join_csv(out, tmpcsv, left_csv, right_csv, left_col, right_col);
            ds_set(dsmap, &dsmap_n, 128, out_ds, tmpcsv);
        }
        
        else if (st->type == AST_FILTER) {
            const char *out_ds = st->text;
        
            // in dataset = st->left is AST_EXPRESSION with text = dataset name
            const char *in_ds = (st->left && st->left->text) ? st->left->text : "";
        
            const char *in_csv = ds_lookup(dsmap, dsmap_n, in_ds);
            if (!in_csv) {
                fprintf(out, "  printf(\"filter: input dataset not found: %s\\n\");\n", in_ds);
                continue;
            }
        
            char tmpcsv[256];
            snprintf(tmpcsv, sizeof(tmpcsv), "__af_filter_%s.csv", out_ds);
        
            ASTNode *cond = st->right; // AST_CONDITION
            const char *op  = (cond && cond->text) ? cond->text : "==";
            const char *lhs = (cond && cond->left && cond->left->text) ? cond->left->text : "";
            const char *rhs = (cond && cond->right && cond->right->text) ? cond->right->text : "0";
        
            emit_filter_csv(out, tmpcsv, in_csv, lhs, op, rhs);
            ds_set(dsmap, &dsmap_n, 128, out_ds, tmpcsv);
        }
        
        else if (st->type == AST_ANALYZE) {
            emit_analyze(out, st, symtab, sources, sources_n, dsmap, dsmap_n);
        }
    }
}



/* -------------------------
   Public entrypoint
   ------------------------- */

int generate_c(ASTNode *root, SymbolTable *symtab, const char *out_c_path)
{
    g_computed_n = 0;
    collect_computes(root);
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
    emit_computed_functions(out);
    emit_main_begin(out);
    emit_register_computed(out);
    emit_pipeline(out, root, symtab, sources, sources_n);
    emit_main_end(out);
    fclose(out);
    return 0;
}
