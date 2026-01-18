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
%type <node> field_list_nonvide analyze_ops_nonvide stmt_list_nonvide


%%

program:
      stmt_list
    {
        root = ast_create(AST_PROGRAM, NULL, $1, NULL);
    }
;

stmt_list: /* vide */ {$$=NULL;}
    | stmt_list_nonvide {$$=$1;}
    ;

stmt_list_nonvide :stmt {$$=$1;}
    |stmt_list_nonvide stmt {$$=ast_cons($1,$2);}

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

field_list
  : /* vide */                  { $$ = NULL; }
  | field_list_nonvide          { $$ = $1; }
  ;

field_list_nonvide
  : field                        { $$ = $1; }
  | field_list_nonvide field     { $$ = ast_cons($1, $2); }
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
    char buf[512];
    snprintf(buf, sizeof(buf), "\"%s\"", $1);
    $$ = ast_create(AST_EXPRESSION, buf, NULL, NULL);
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
    JOIN IDENT '=' column_ref INNER JOIN column_ref ';'
    {
        $$ = ast_create(AST_JOIN, $2, $4, $7);
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

analyze_ops
  : /* vide */                       { $$ = NULL; }
  | analyze_ops_nonvide              { $$ = $1; }
  ;

analyze_ops_nonvide
  : analyze_op                        { $$ = $1; }
  | analyze_ops_nonvide analyze_op    { $$ = ast_cons($1, $2); }
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

