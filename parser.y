%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    #include "parser.tab.h"
    #include "symbol_table.h"
    #include "ast.h"
    
    extern int yylex(void);
    extern int yylineno;
    extern int yycolumn;
    void yyerror(const char *s);
    
    SymbolTable *global_symtab;
    ASTNode *global_root = NULL;

    int opt_tos = 0;
    char *output_file = NULL;
    extern ASTNode *global_root;
    //Null fo schema car pas de fichier csv lier
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

%type <node> program stmts stmt source_stmt schema_stmt associate_stmt analyze_stmt
%type <node> schema_fields field_decl  
%type <str> field_type
%start program
%%

/* ======== SECTION GRAMMAIRE ======== */


program:
      stmts
    {
        ASTNode *program_node = ast_create(AST_PROGRAM, NULL, NULL, NULL); 
        if ($1 != NULL) {
            ast_add_child(program_node, $1);
        }
        global_root = program_node; 
        $$ = program_node; 
    }
    ;

stmts:
      /* vide */
    {
        $$ = NULL;
    }
    | stmts stmt
        {
            if ($1 == NULL) {
            $$ = $2; 
        } else {
            ast_add_child($1, $2);
            $$ = $1; 
        }
    }
    
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
                                                    //Vérifie que le symbole n'a pas été déjà défini 
                                                    if (symtab_find(global_symtab, $2) != NULL) {
                                                        fprintf(stderr, "[ERREUR SÉMANTIQUE] L'identifiant '%s' est déjà défini. Redéfinition de source.\n", $2);
                                                        YYERROR; 
                                                    }
                                                    symtab_add(global_symtab, $2, "source");
                                                    $$ = ast_create(AST_SOURCE, $2, $3,NULL);
                                                    printf("[PARSE] Source %s -> %s\n", $2, $3);
                                                }
    ;

schema_stmt:
    SCHEMA IDENT '{' schema_fields '}'          
                                                {
                                                    //Vérifie que le symbole n'a pas déjà un schema défini 
                                                    if (symtab_find(global_symtab, $2) != NULL) {
                                                        fprintf(stderr, "[ERREUR SÉMANTIQUE] L'identifiant '%s' est déjà défini.\n", $2);
                                                        YYERROR; 
                                                    }
                                                    symtab_add(global_symtab, $2, "schema");
                                                    $$ = ast_create(AST_SCHEMA, $2, NULL,NULL);
                                                    if ($4 != NULL) {
                                                        ast_add_child($$, $4); // $4 est la racine de la liste d'enfants
                                                    }
                                                    printf("[PARSE] Schema %s {...}\n", $2);
                                                }
    ;

associate_stmt:
    ASSOCIATE IDENT WITH IDENT ';'              
                                                {
                                                    // Le premier IDENT ($2) est le nom du schéma
                                                    // Le second IDENT ($4) est le nom de la source

                                                    printf("[PARSE] Associate %s with %s\n", $2, $4);

                                                    Symbol *schema_sym = symtab_find(global_symtab, $2);
                                                    Symbol *source_sym = symtab_find(global_symtab, $4);

                                                    if (source_sym == NULL || (strcmp(source_sym->type, "source") != 0 && strcmp(source_sym->type, "join") != 0)) {
                                                        fprintf(stderr, "[ERREUR SÉMANTIQUE] Source non définie ou type incorrect: %s\n", $4);
                                                        YYERROR;
                                                    }

                                                    if (schema_sym == NULL || strcmp(schema_sym->type, "schema") != 0) {
                                                        fprintf(stderr, "[ERREUR SÉMANTIQUE] Schéma non défini ou type incorrect: %s\n", $2);
                                                        YYERROR;
                                                    }

                                                    $$ = ast_create(AST_ASSOCIATE, $2, $4,NULL);
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
    /* vide */
                                                    {
                                                        $$ = NULL;
                                                    }
    | field_decl schema_fields 
                                                    {
                                                        // $1 est le nœud AST_FIELD (la colonne)
                                                        // $2 est le résultat (liste) de l'appel récursif suivant
                                                        
                                                        if ($2 != NULL) {
                                                            ast_add_child($2, $1);
                                                            $$ = $2;
                                                        } else {
                                                            $$ = $1;
                                                        }
                                                    }
    ;    

analyze_ops:
      /* à compléter plus tard */
    ;

field_decl:
    IDENT field_type ';' 
                                                    {
                                                        $$ = ast_create(AST_FIELD, $1, NULL,$2);
                                                    }
    ;
field_type:
      TYPE_INTEGER   { $$ = strdup("integer"); }
    | TYPE_FLOAT     { $$ = strdup("float"); }
    | TYPE_STRING    { $$ = strdup("string"); }
    ;
%%

/* ======== SECTION CODE C (bas du fichier) ======== */

void yyerror(const char *s) {
    fprintf(stderr, "[ERREUR SYNTAXE] %s (ligne %d, col %d)\n", s, yylineno, yycolumn);
}

int main(int argc, char **argv) {
    global_symtab = symtab_create();
    if (yyparse() == 0){
        printf("Analyse syntaxique réussie\n");
        printf("\n=== Arbre Syntaxique Abstrait ===\n");
        if (global_root) {
            ast_print(global_root, 0); // Appeler ast_print sur la racine
        } else {
            printf("(AST Vide)\\n");
        }
        symtab_free(global_symtab);
    }else{
        symtab_free(global_symtab);
        return 1;
    }
    return 0;
}

