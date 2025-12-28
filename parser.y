%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"
#include "codegen.h"

extern int yylex(void);
extern int yylineno;
extern int yycolumn;

void yyerror(const char *s);
ASTNode *root = NULL;
%}

%union {
    int ival;
    double fval;
    char *str;
    ASTNode *node;
}

/* TOKENS */
%token SOURCE SCHEMA ASSOCIATE WITH COMPUTE ANALYZE
%token JOIN INNER FILTER WHERE
%token TYPE_INTEGER TYPE_FLOAT TYPE_STRING
%token KW_MEAN KW_MEDIAN KW_STDDEV KW_MIN KW_MAX KW_HISTOGRAM KW_SUMMARY
%token EQ NEQ LT GT LE GE

%token <str> IDENT STRING_LITERAL
%token <ival> INUMBER
%token <fval> FNUMBER

%left '+' '-'
%left '*' '/'

%type <node> program stmt_list stmt
%type <node> source_stmt schema_stmt associate_stmt compute_stmt join_stmt filter_stmt analyze_stmt
%type <node> field_list field
%type <node> analyze_ops analyze_op
%type <node> expression condition column_ref

%%

program:
      stmt_list
    {
        root = ast_create(AST_PROGRAM, NULL, $1, NULL);
    }
;

stmt_list:
      stmt_list stmt     { $$ = ast_cons($1, $2); }
    | stmt               { $$ = $1; }
    | /* vide */         { $$ = NULL; }
;

stmt:
      source_stmt
    | schema_stmt
    | associate_stmt
    | compute_stmt
    | join_stmt
    | filter_stmt
    | analyze_stmt
;

/* source ident "file"; */
source_stmt:
    SOURCE IDENT STRING_LITERAL ';'
    {
        ASTNode *file = ast_create(AST_EXPRESSION, $3, NULL, NULL);
        $$ = ast_create(AST_SOURCE, $2, file, NULL);
    }
;

/* schema name { field... } */
schema_stmt:
    SCHEMA IDENT '{' field_list '}'
    {
        $$ = ast_create(AST_SCHEMA, $2, $4, NULL);
    }
;

field_list:
      field_list field   { $$ = ast_cons($1, $2); }
    | field              { $$ = $1; }
    | /* vide */         { $$ = NULL; }
;

field:
      IDENT TYPE_INTEGER ';'
    {
        ASTNode *t = ast_create(AST_FIELD, "integer", NULL, NULL);
        $$ = ast_create(AST_FIELD, $1, t, NULL);
    }
    | IDENT TYPE_FLOAT ';'
    {
        ASTNode *t = ast_create(AST_FIELD, "float", NULL, NULL);
        $$ = ast_create(AST_FIELD, $1, t, NULL);
    }
    | IDENT TYPE_STRING ';'
    {
        ASTNode *t = ast_create(AST_FIELD, "string", NULL, NULL);
        $$ = ast_create(AST_FIELD, $1, t, NULL);
    }
;

/* associate schema with source; */
associate_stmt:
    ASSOCIATE IDENT WITH IDENT ';'
    {
        ASTNode *src = ast_create(AST_EXPRESSION, $4, NULL, NULL);
        $$ = ast_create(AST_ASSOCIATE, $2, src, NULL);
    }
;

/* compute col = expr; */
compute_stmt:
    COMPUTE IDENT '=' expression ';'
    {
        $$ = ast_create(AST_COMPUTE, $2, $4, NULL);
    }
;

/* column reference source.col */
column_ref:
    IDENT '.' IDENT
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "%s.%s", $1, $3);
        $$ = ast_create(AST_EXPRESSION, buf, NULL, NULL);
    }
;

/* Expressions */
expression:
      INUMBER
    {
        char buf[64]; snprintf(buf, sizeof(buf), "%d", $1);
        $$ = ast_create(AST_EXPRESSION, buf, NULL, NULL);
    }
    | FNUMBER
    {
        char buf[64]; snprintf(buf, sizeof(buf), "%g", $1);
        $$ = ast_create(AST_EXPRESSION, buf, NULL, NULL);
    }
    | STRING_LITERAL
    {
        $$ = ast_create(AST_EXPRESSION, $1, NULL, NULL);
    }
    | IDENT
    {
        $$ = ast_create(AST_EXPRESSION, $1, NULL, NULL);
    }
    | column_ref
    {
        $$ = $1;
    }
    | '(' expression ')'  { $$ = $2; }
    | expression '+' expression  { $$ = ast_create(AST_EXPRESSION, "+", $1, $3); }
    | expression '-' expression  { $$ = ast_create(AST_EXPRESSION, "-", $1, $3); }
    | expression '*' expression  { $$ = ast_create(AST_EXPRESSION, "*", $1, $3); }
    | expression '/' expression  { $$ = ast_create(AST_EXPRESSION, "/", $1, $3); }
;

/* JOIN name = a.x INNER b.y; (pas INNER JOIN !) */
join_stmt:
    JOIN IDENT '=' column_ref INNER column_ref ';'
    {
        $$ = ast_create(AST_JOIN, $2, $4, $6);
    }
;

/* filter name = src where condition; */
filter_stmt:
    FILTER IDENT '=' IDENT WHERE condition ';'
    {
        ASTNode *src = ast_create(AST_EXPRESSION, $4, NULL, NULL);
        $$ = ast_create(AST_FILTER, $2, src, $6);
    }
;

/* conditions */
condition:
      expression EQ expression   { $$ = ast_create(AST_CONDITION, "==", $1, $3); }
    | expression NEQ expression  { $$ = ast_create(AST_CONDITION, "!=", $1, $3); }
    | expression LT expression   { $$ = ast_create(AST_CONDITION, "<", $1, $3); }
    | expression GT expression   { $$ = ast_create(AST_CONDITION, ">", $1, $3); }
    | expression LE expression   { $$ = ast_create(AST_CONDITION, "<=", $1, $3); }
    | expression GE expression   { $$ = ast_create(AST_CONDITION, ">=", $1, $3); }
;

/* analyze dataset { ops } */
analyze_stmt:
    ANALYZE IDENT '{' analyze_ops '}'
    {
        $$ = ast_create(AST_ANALYZE, $2, $4, NULL);
    }
;

analyze_ops:
      analyze_ops analyze_op   { $$ = ast_cons($1, $2); }
    | analyze_op               { $$ = $1; }
    | /* vide */               { $$ = NULL; }
;

analyze_op:
      KW_MEAN IDENT ';'
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "mean %s", $2);
        $$ = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
    | KW_SUMMARY IDENT ';'
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "summary %s", $2);
        $$ = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
    | KW_HISTOGRAM IDENT ';'
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "histogram %s", $2);
        $$ = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
    | KW_MEDIAN IDENT ';'
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "median %s", $2);
        $$ = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
    | KW_STDDEV IDENT ';'
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "std_dev %s", $2);
        $$ = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
    | KW_MIN IDENT ';'
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "min %s", $2);
        $$ = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
    | KW_MAX IDENT ';'
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "max %s", $2);
        $$ = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "[ERREUR SYNTAXE] %s (ligne %d)\n", s, yylineno);
}

int main(void)
{
    SymbolTable *symtab = symtab_create();

    if (yyparse() == 0) {
        printf("Analyse syntaxique réussie\n\n=== AST ===\n");
        ast_print(root, 0);

        printf("\n=== Construction de la table des symboles ===\n");

        /* 1ère passe : schemas + fields + associate */
        ASTNode *n = root->left;
        while (n) {

            if (n->type == AST_SCHEMA) {
                symtab_add_schema(symtab, n->text, "");
                ASTNode *f = n->left;
                while (f) {
                    symtab_add_field(symtab, n->text, f->text, f->left->text);
                    f = f->next;
                }
            }

            if (n->type == AST_ASSOCIATE) {
                symtab_associate(symtab, n->text, n->left->text);
            }

            n = n->next;
        }

        symtab_print(symtab);

        /* 2e passe : datasets virtuels (JOIN/FILTER) */
        ASTNode *m = root->left;
        while (m) {

            if (m->type == AST_JOIN) {
                char s1[128], c1[128], s2[128], c2[128];

                /* m->left->text  = "src.col" */
                /* m->right->text = "src.col" */
                sscanf(m->left->text,  "%127[^.].%127[^ \t\r\n;]", s1, c1);
                sscanf(m->right->text, "%127[^.].%127[^ \t\r\n;]", s2, c2);

                Schema *A = symtab_get_schema_by_source(symtab, s1);
                Schema *B = symtab_get_schema_by_source(symtab, s2);

                if (A && B) {
                    Schema *J = symtab_add_empty_schema(symtab, m->text);

                    for (int i = 0; i < A->column_count; i++)
                        schema_add_column(J, A->columns[i].name, A->columns[i].type);

                    for (int i = 0; i < B->column_count; i++)
                        schema_add_column(J, B->columns[i].name, B->columns[i].type);
                }
            }

            if (m->type == AST_FILTER) {
                const char *src = m->left->text;

                Schema *base = symtab_get_schema(symtab, src);
                if (!base) base = symtab_get_schema_by_source(symtab, src);

                if (base) {
                    symtab_clone_schema(symtab, m->text, base);
                }
            }

            /* AST_COMPUTE : pas encore utilisé côté symtab (sera fait plus tard si besoin) */

            m = m->next;
        }

        printf("\n=== TABLE DES SYMBOLES (après JOIN/FILTER) ===\n");
        symtab_print(symtab);

        printf("\n=== Analyse sémantique ===\n");
        if (semantic_check(root, symtab) == 0) {
            printf("✓ Aucun problème sémantique détecté.\n");

            printf("\n=== Génération du code C ===\n");
            if (generate_c(root, symtab, "afstat.c") == 0) {
                printf("✓ Fichier généré : afstat.c\n");
            } else {
                printf("✗ Échec génération C.\n");
            }

        } else {
            printf("✗ Erreurs sémantiques détectées.\n");
        }
    }

    return 0;
}
