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
        } else *q++ = *p++;
    }
    *q = '\0';
    return out;
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
\n            { yylineno++; yycolumn = 1; }


"//".*        { /* skip */ }

"/*"          { BEGIN(COMMENT); }

<COMMENT>{
    "*/"      { BEGIN(INITIAL); }
    \n        { yylineno++; yycolumn = 1; }
    .         { /* skip */ }
}


source        { adv_col(yytext); return SOURCE; }
schema        { adv_col(yytext); return SCHEMA; }
associate     { adv_col(yytext); return ASSOCIATE; }
with          { adv_col(yytext); return WITH; }
compute       { adv_col(yytext); return COMPUTE; }
analyze       { adv_col(yytext); return ANALYZE; }
join          { adv_col(yytext); return JOIN; }
inner         { adv_col(yytext); return INNER; }
filter        { adv_col(yytext); return FILTER; }
where         { adv_col(yytext); return WHERE; }

integer       { adv_col(yytext); return TYPE_INTEGER; }
float         { adv_col(yytext); return TYPE_FLOAT; }
string        { adv_col(yytext); return TYPE_STRING; }

mean          { adv_col(yytext); return KW_MEAN; }
median        { adv_col(yytext); return KW_MEDIAN; }
std_dev       { adv_col(yytext); return KW_STDDEV; }
min           { adv_col(yytext); return KW_MIN; }
max           { adv_col(yytext); return KW_MAX; }
histogram     { adv_col(yytext); return KW_HISTOGRAM; }
summary       { adv_col(yytext); return KW_SUMMARY; }


{FLOAT1}      { yylval.fval = atof(yytext); adv_col(yytext); return FNUMBER; }
{FLOAT2}      { yylval.fval = atof(yytext); adv_col(yytext); return FNUMBER; }
{EXPONLY}     { yylval.fval = atof(yytext); adv_col(yytext); return FNUMBER; }
{DIGIT}+      { yylval.ival = atoi(yytext); adv_col(yytext); return INUMBER; }


{STRING}      { yylval.str = unescape_and_strip_quotes(yytext);
                adv_col(yytext);
                return STRING_LITERAL; }


{ID}          { yylval.str = strdup(yytext); adv_col(yytext); return IDENT; }


"==" { adv_col(yytext); return EQ; }
"!=" { adv_col(yytext); return NEQ; }
"<=" { adv_col(yytext); return LE; }
">=" { adv_col(yytext); return GE; }
"<"  { adv_col(yytext); return LT; }
">"  { adv_col(yytext); return GT; }


"+"  { adv_col(yytext); return '+'; }
"-"  { adv_col(yytext); return '-'; }
"*"  { adv_col(yytext); return '*'; }
"/"  { adv_col(yytext); return '/'; }


"="  { adv_col(yytext); return '='; }
"{"  { adv_col(yytext); return '{'; }
"}"  { adv_col(yytext); return '}'; }
"("  { adv_col(yytext); return '('; }
")"  { adv_col(yytext); return ')'; }
";"  { adv_col(yytext); return ';'; }
","  { adv_col(yytext); return ','; }
"."  { adv_col(yytext); return '.'; }


. {
    fprintf(stderr, "[LEX] caractère inattendu '%s' à la ligne %d\n", yytext, yylineno);
    adv_col(yytext);
}

%%

