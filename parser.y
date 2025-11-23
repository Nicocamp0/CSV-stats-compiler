%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

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
%type <node> expression condition
%type <node> column_ref   /* <<< nouveau non-terminal pour src.col */

%%

program:
      stmt_list
    {
        root = ast_create(AST_PROGRAM, NULL, $1, NULL);
    }
;

stmt_list:
      stmt_list stmt
    {
        $$ = ast_cons($1, $2);
    }
    | stmt
    {
        $$ = $1;
    }
    | /* vide */
    {
        $$ = NULL;
    }
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

/* source nom "fichier.csv"; */
source_stmt:
    SOURCE IDENT STRING_LITERAL ';'
    {
        ASTNode *file = ast_create(AST_EXPRESSION, $3, NULL, NULL);
        $$ = ast_create(AST_SOURCE, $2, file, NULL);
    }
;

/* schema nom { champs } */
schema_stmt:
    SCHEMA IDENT '{' field_list '}'
    {
        $$ = ast_create(AST_SCHEMA, $2, $4, NULL);
    }
;

/* liste de champs */
field_list:
      field_list field    { $$ = ast_cons($1, $2); }
    | field               { $$ = $1; }
    | /* vide */          { $$ = NULL; }
;

/* champ : nom type; */
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

/* compute nouvelle_colonne = expression; */
compute_stmt:
    COMPUTE IDENT '=' expression ';'
    {
        $$ = ast_create(AST_COMPUTE, $2, $4, NULL);
    }
;

/* Référence de colonne : source.colonne */
column_ref:
    IDENT '.' IDENT
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "%s.%s", $1, $3);
        $$ = ast_create(AST_EXPRESSION, buf, NULL, NULL);
    }
;

/* expressions (arith, ident, src.col, string, …) */
expression:
      INUMBER
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "%d", $1);
        $$ = ast_create(AST_EXPRESSION, buf, NULL, NULL);
    }
    | FNUMBER
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", $1);
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
    | column_ref   /* <<< on réutilise la règle dédiée */
    {
        $$ = $1;
    }
    | '(' expression ')'
    {
        $$ = $2;
    }
    | expression '+' expression
    {
        $$ = ast_create(AST_EXPRESSION, "+", $1, $3);
    }
    | expression '-' expression
    {
        $$ = ast_create(AST_EXPRESSION, "-", $1, $3);
    }
    | expression '*' expression
    {
        $$ = ast_create(AST_EXPRESSION, "*", $1, $3);
    }
    | expression '/' expression
    {
        $$ = ast_create(AST_EXPRESSION, "/", $1, $3);
    }
;

/* join nom = s1.c1 inner join s2.c2; */
join_stmt:
    JOIN IDENT '=' column_ref INNER JOIN column_ref ';'
    {
        $$ = ast_create(AST_JOIN, $2, $4, $7);
    }

;

/* filter nom = src where condition; */
filter_stmt:
    FILTER IDENT '=' IDENT WHERE condition ';'
    {
        ASTNode *src = ast_create(AST_EXPRESSION, $4, NULL, NULL);
        $$ = ast_create(AST_FILTER, $2, src, $6);
    }
;

/* condition: expr op expr */
condition:
      expression EQ  expression
    {
        $$ = ast_create(AST_CONDITION, "==", $1, $3);
    }
    | expression NEQ expression
    {
        $$ = ast_create(AST_CONDITION, "!=", $1, $3);
    }
    | expression LT  expression
    {
        $$ = ast_create(AST_CONDITION, "<", $1, $3);
    }
    | expression GT  expression
    {
        $$ = ast_create(AST_CONDITION, ">", $1, $3);
    }
    | expression LE  expression
    {
        $$ = ast_create(AST_CONDITION, "<=", $1, $3);
    }
    | expression GE  expression
    {
        $$ = ast_create(AST_CONDITION, ">=", $1, $3);
    }
;

/* analyze schema_name { opérations } */
analyze_stmt:
    ANALYZE IDENT '{' analyze_ops '}' 
    {
        $$ = ast_create(AST_ANALYZE, $2, $4, NULL);
    }
;

analyze_ops:
      analyze_ops analyze_op    { $$ = ast_cons($1, $2); }
    | analyze_op                { $$ = $1; }
    | /* vide */                { $$ = NULL; }
;

/* mean age;  summary weight; etc. */
analyze_op:
      KW_MEAN IDENT ';'
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "mean %s", $2);
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
    | KW_HISTOGRAM IDENT ';'
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "histogram %s", $2);
        $$ = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
    | KW_SUMMARY IDENT ';'
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "summary %s", $2);
        $$ = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "[ERREUR SYNTAXE] %s (ligne %d)\n", s, yylineno);
}

int main(void) {
    if (yyparse() == 0) {
        printf("Analyse syntaxique réussie\n");
        printf("=== AST ===\n");
        ast_print(root, 0);
    } else {
        printf("Échec de l'analyse syntaxique\n");
    }
    return 0;
}

