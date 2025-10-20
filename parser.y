%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "parser.tab.h"
    extern int yylex(void);
    extern int yylineno;
    extern int yycolumn;
    void yyerror(const char *s);
%}

%union {
    int ival;
    double fval;
    char *str;
    struct ASTNode *node;
}

%token SOURCE SCHEMA ASSOCIATE WITH ANALYZE COMPUTE
%token JOIN INNER FILTER WHERE
%token <str> IDENT STRING_LITERAL
%token TYPE_INTEGER TYPE_FLOAT TYPE_STRING
%token KW_MEAN KW_MEDIAN KW_STDDEV KW_MIN KW_MAX KW_HISTOGRAM KW_SUMMARY
%token FNUMBER INUMBER
%token EQ NEQ LE GE LT GT
%left '+' '-'
%left '*' '/'

%type <node> source_stmt schema_stmt associate_stmt analyze_stmt


%%

/* ======== SECTION GRAMMAIRE ======== */

/* Programme complet */
program:
      stmts
    ;

stmts:
      /* vide */
    | stmts stmt
    ;

stmt:
      source_stmt
    | schema_stmt
    | associate_stmt
    | analyze_stmt
    ;

/* --- Déclarations de base --- */

source_stmt:
      SOURCE IDENT STRING_LITERAL ';'
    {
        printf("[PARSE] Source %s -> %s\n", $2, $3);
    }
    ;

schema_stmt:
      SCHEMA IDENT '{' schema_fields '}'
    {
        printf("[PARSE] Schema %s {...}\n", $2);
    }
    ;

associate_stmt:
      ASSOCIATE IDENT WITH IDENT ';'
    {
        printf("[PARSE] Associate %s with %s\n", $2, $4);
    }
    ;

analyze_stmt:
      ANALYZE IDENT '{' analyze_ops '}'
    {
        printf("[PARSE] Analyze %s {...}\n", $2);
    }
    ;

/* --- Règles simplifiées pour les champs et analyses --- */

schema_fields:
      /* à compléter plus tard */
    ;

analyze_ops:
      /* à compléter plus tard */
    ;

%%

/* ======== SECTION CODE C (bas du fichier) ======== */

void yyerror(const char *s) {
    fprintf(stderr, "[ERREUR SYNTAXE] %s (ligne %d, col %d)\n", s, yylineno, yycolumn);
}

int main(int argc, char **argv) {
    if (yyparse() == 0)
        printf("Analyse syntaxique réussie\n");
    else
        printf("Échec de l'analyse syntaxique\n");
    return 0;
}

