%option noyywrap nodefault yylineno nounput noinput
%option caseless

%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.tab.h" 

extern int yylineno;
int yycolumn = 1;


#ifdef YYLTYPE
extern YYLTYPE yylloc;
static void set_loc(void) {
    yylloc.first_line   = yylineno;
    yylloc.first_column = yycolumn;
    yylloc.last_line    = yylineno;
    yylloc.last_column  = yycolumn + (int) yyleng - 1;
}
#else
static void set_loc(void) { (void)0; }
#endif

static void adv_col(const char *yytext) {
    yycolumn += (int)strlen(yytext);
}

static char *unescape_and_strip_quotes(const char *yytext) {
    size_t n = strlen(yytext);
    if (n < 2) return strdup("");
    const char *p = yytext + 1;        
    const char *end = yytext + n - 1;

    char *out = (char*)malloc((n + 1) * sizeof(char));
    char *q = out;
    while (p < end) {
        if (*p == '\\' && p + 1 < end) {
            ++p;
            switch (*p) {
                case 'n':  *q++ = '\n'; break;
                case 't':  *q++ = '\t'; break;
                case '"':  *q++ = '"';  break;
                case '\\': *q++ = '\\'; break;
                default:   *q++ = *p;   break; /* copie tel quel (\r, \x, etc.) */
            }
            ++p;
        } else {
            *q++ = *p++;
        }
    }
    *q = '\0';
    return out;
}
%}

WS      [ \t\f]+
DIGIT   [0-9]
ID      [A-Za-z_][A-Za-z0-9_]*
SQUOTE  \"
ESC     \\.
STRCHR  ([^\n\\\"]|{ESC})
STRING  {SQUOTE}{STRCHR}*{SQUOTE}

FLOAT1  {DIGIT}+\.{DIGIT}*([eE][+-]?{DIGIT}+)?
FLOAT2  {DIGIT}*\.{DIGIT}+([eE][+-]?{DIGIT}+)?
EXPONLY {DIGIT}+([eE][+-]?{DIGIT}+)

%%


\r?\n           {yylineno++; yycolumn = 1;}
{WS}            {adv_col(yytext);}
"//"[^\n]*      {adv_col(yytext);}

source          { set_loc(); adv_col(yytext); return SOURCE; }
schema          { set_loc(); adv_col(yytext); return SCHEMA; }
associate       { set_loc(); adv_col(yytext); return ASSOCIATE; }
with            { set_loc(); adv_col(yytext); return WITH; }
compute         { set_loc(); adv_col(yytext); return COMPUTE; }
analyze         { set_loc(); adv_col(yytext); return ANALYZE; }
join            { set_loc(); adv_col(yytext); return JOIN; }
inner           { set_loc(); adv_col(yytext); return INNER; }
filter          { set_loc(); adv_col(yytext); return FILTER; }
where           { set_loc(); adv_col(yytext); return WHERE; }

integer         { set_loc(); adv_col(yytext); return TYPE_INTEGER; }
float           { set_loc(); adv_col(yytext); return TYPE_FLOAT; }
string          { set_loc(); adv_col(yytext); return TYPE_STRING; }


mean            { set_loc(); adv_col(yytext); return KW_MEAN; }
median          { set_loc(); adv_col(yytext); return KW_MEDIAN; }
std_dev         { set_loc(); adv_col(yytext); return KW_STDDEV; }
min             { set_loc(); adv_col(yytext); return KW_MIN; }
max             { set_loc(); adv_col(yytext); return KW_MAX; }
histogram       { set_loc(); adv_col(yytext); return KW_HISTOGRAM; }
summary         { set_loc(); adv_col(yytext); return KW_SUMMARY; }


{FLOAT1}        { set_loc(); yylval.fval = strtod(yytext, NULL); adv_col(yytext); return FNUMBER; }
{FLOAT2}        { set_loc(); yylval.fval = strtod(yytext, NULL); adv_col(yytext); return FNUMBER; }

{EXPONLY}       { set_loc(); yylval.fval = strtod(yytext, NULL); adv_col(yytext); return FNUMBER; }
{DIGIT}+        { set_loc(); yylval.ival = strtol(yytext, NULL, 10); adv_col(yytext); return INUMBER; }

{STRING}        {
                    set_loc();
                    yylval.str = unescape_and_strip_quotes(yytext);
                    adv_col(yytext);
                    return STRING_LITERAL;
                }


{ID}            { set_loc(); yylval.str = strdup(yytext); adv_col(yytext); return IDENT; }


"=="            { set_loc(); adv_col(yytext); return EQ; }
"!="            { set_loc(); adv_col(yytext); return NEQ; }
"<="            { set_loc(); adv_col(yytext); return LE; }
">="            { set_loc(); adv_col(yytext); return GE; }
"<"             { set_loc(); adv_col(yytext); return LT; }
">"             { set_loc(); adv_col(yytext); return GT; }

"+"             { set_loc(); adv_col(yytext); return '+'; }
"-"             { set_loc(); adv_col(yytext); return '-'; }
"*"             { set_loc(); adv_col(yytext); return '*'; }
"/"             { set_loc(); adv_col(yytext); return '/'; }

"="             { set_loc(); adv_col(yytext); return '='; }
"{"             { set_loc(); adv_col(yytext); return '{'; }
"}"             { set_loc(); adv_col(yytext); return '}'; }
"("             { set_loc(); adv_col(yytext); return '('; }
")"             { set_loc(); adv_col(yytext); return ')'; }
";"             { set_loc(); adv_col(yytext); return ';'; }
","             { set_loc(); adv_col(yytext); return ','; }
"."             { set_loc(); adv_col(yytext); return '.'; }

.               {
                    set_loc();
                    fprintf(stderr, "[LEX] caractère inattendu '%s' (ligne %d, col %d)\n",yytext, yylineno, yycolumn);adv_col(yytext);
                }

%%


