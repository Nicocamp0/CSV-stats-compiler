%option noyywrap nodefault yylineno nounput noinput
%option caseless

%x COMMENT

%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "parser.tab.h"

extern int yylineno;
int yycolumn = 1;

/* --------- DEBUG TOKENS ---------
   Mettre print_tokens = 1 (depuis main.c via -tokens)
   pour afficher les tokens générés par le lexer.
*/
int print_tokens = 0;

static void adv_col(const char *yytext) {
    yycolumn += (int)strlen(yytext);
}

static char *unescape_and_strip_quotes(const char *yytext) {
    size_t n = strlen(yytext);
    if (n < 2) return strdup("");
    const char *p = yytext + 1;
    const char *end = yytext + n - 1;

    char *out = malloc(n + 1);
    char *q = out;

    while (p < end) {
        if (*p == '\\' && p + 1 < end) {
            ++p;
            switch (*p) {
                case 'n': *q++ = '\n'; break;
                case 't': *q++ = '\t'; break;
                case '\\': *q++ = '\\'; break;
                case '"': *q++ = '"'; break;
                default: *q++ = *p; break;
            }
            ++p;
        } else {
            *q++ = *p++;
        }
    }
    *q = '\0';
    return out;
}

/* Affichage uniforme des tokens */
static void tok(const char *name) {
    if (print_tokens) printf("[%s] ", name);
}
static void tok_s(const char *name, const char *val) {
    if (print_tokens) printf("[%s %s] ", name, val);
}
static void tok_i(const char *name, int v) {
    if (print_tokens) printf("[%s %d] ", name, v);
}
static void tok_f(const char *name, double v) {
    if (print_tokens) printf("[%s %g] ", name, v);
}
%}

/* ==== PATTERNS ==== */

DIGIT   [0-9]
ID      [A-Za-z_][A-Za-z0-9_]*
STRING  \"([^\\\n\"]|\\.)*\"

FLOAT1  {DIGIT}+\.{DIGIT}*([eE][+-]?{DIGIT}+)?
FLOAT2  {DIGIT}*\.{DIGIT}+([eE][+-]?{DIGIT}+)?
EXPONLY {DIGIT}+([eE][+-]?{DIGIT}+)

%%


[ \t\r]+      { adv_col(yytext); }
\n            {
                yylineno++;
                yycolumn = 1;
                if (print_tokens) printf("\n");
             }

"//".*        { /* skip */ adv_col(yytext); }

"/*"          { adv_col(yytext); BEGIN(COMMENT); }

<COMMENT>{
    "*/"      { adv_col(yytext); BEGIN(INITIAL); }
    \n        { yylineno++; yycolumn = 1; if (print_tokens) printf("\n"); }
    .         { adv_col(yytext); /* skip */ }
}

source        { adv_col(yytext); tok("SOURCE"); return SOURCE; }
schema        { adv_col(yytext); tok("SCHEMA"); return SCHEMA; }
associate     { adv_col(yytext); tok("ASSOCIATE"); return ASSOCIATE; }
with          { adv_col(yytext); tok("WITH"); return WITH; }
compute       { adv_col(yytext); tok("COMPUTE"); return COMPUTE; }
analyze       { adv_col(yytext); tok("ANALYZE"); return ANALYZE; }
join          { adv_col(yytext); tok("JOIN"); return JOIN; }
inner         { adv_col(yytext); tok("INNER"); return INNER; }
filter        { adv_col(yytext); tok("FILTER"); return FILTER; }
where         { adv_col(yytext); tok("WHERE"); return WHERE; }


integer       { adv_col(yytext); tok("TYPE_INTEGER"); return TYPE_INTEGER; }
float         { adv_col(yytext); tok("TYPE_FLOAT"); return TYPE_FLOAT; }
string        { adv_col(yytext); tok("TYPE_STRING"); return TYPE_STRING; }


mean          { adv_col(yytext); tok("KW_MEAN"); return KW_MEAN; }
median        { adv_col(yytext); tok("KW_MEDIAN"); return KW_MEDIAN; }
std_dev       { adv_col(yytext); tok("KW_STDDEV"); return KW_STDDEV; }
min           { adv_col(yytext); tok("KW_MIN"); return KW_MIN; }
max           { adv_col(yytext); tok("KW_MAX"); return KW_MAX; }
histogram     { adv_col(yytext); tok("KW_HISTOGRAM"); return KW_HISTOGRAM; }
summary       { adv_col(yytext); tok("KW_SUMMARY"); return KW_SUMMARY; }


{FLOAT1}      { yylval.fval = atof(yytext); adv_col(yytext); tok_f("FNUMBER", yylval.fval); return FNUMBER; }
{FLOAT2}      { yylval.fval = atof(yytext); adv_col(yytext); tok_f("FNUMBER", yylval.fval); return FNUMBER; }
{EXPONLY}     { yylval.fval = atof(yytext); adv_col(yytext); tok_f("FNUMBER", yylval.fval); return FNUMBER; }

{DIGIT}+      { yylval.ival = atoi(yytext); adv_col(yytext); tok_i("INUMBER", yylval.ival); return INUMBER; }


{STRING}      {
                yylval.str = unescape_and_strip_quotes(yytext);
                adv_col(yytext);
                tok_s("STRING", yylval.str);
                return STRING_LITERAL;
              }


{ID}          { yylval.str = strdup(yytext); adv_col(yytext); tok_s("IDENT", yytext); return IDENT; }


"==" { adv_col(yytext); tok("=="); return EQ; }
"!=" { adv_col(yytext); tok("!="); return NEQ; }
"<=" { adv_col(yytext); tok("<="); return LE; }
">=" { adv_col(yytext); tok(">="); return GE; }
"<"  { adv_col(yytext); tok("<");  return LT; }
">"  { adv_col(yytext); tok(">");  return GT; }


"+"  { adv_col(yytext); tok("+"); return '+'; }
"-"  { adv_col(yytext); tok("-"); return '-'; }
"*"  { adv_col(yytext); tok("*"); return '*'; }
"/"  { adv_col(yytext); tok("/"); return '/'; }


"="  { adv_col(yytext); tok("=");  return '='; }
"{"  { adv_col(yytext); tok("{");  return '{'; }
"}"  { adv_col(yytext); tok("}");  return '}'; }
"("  { adv_col(yytext); tok("(");  return '('; }
")"  { adv_col(yytext); tok(")");  return ')'; }
";"  { adv_col(yytext); tok(";");  return ';'; }
","  { adv_col(yytext); tok(",");  return ','; }
"."  { adv_col(yytext); tok(".");  return '.'; }


. {
    fprintf(stderr, "[LEX] caractère inattendu '%s' à la ligne %d\n", yytext, yylineno);
    adv_col(yytext);
}

%%
