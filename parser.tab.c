/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

extern int yylex(void);
extern int yylineno;
extern int yycolumn;

void yyerror(const char *s);

ASTNode *root = NULL;

#line 85 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SOURCE = 3,                     /* SOURCE  */
  YYSYMBOL_SCHEMA = 4,                     /* SCHEMA  */
  YYSYMBOL_ASSOCIATE = 5,                  /* ASSOCIATE  */
  YYSYMBOL_WITH = 6,                       /* WITH  */
  YYSYMBOL_COMPUTE = 7,                    /* COMPUTE  */
  YYSYMBOL_ANALYZE = 8,                    /* ANALYZE  */
  YYSYMBOL_JOIN = 9,                       /* JOIN  */
  YYSYMBOL_INNER = 10,                     /* INNER  */
  YYSYMBOL_FILTER = 11,                    /* FILTER  */
  YYSYMBOL_WHERE = 12,                     /* WHERE  */
  YYSYMBOL_TYPE_INTEGER = 13,              /* TYPE_INTEGER  */
  YYSYMBOL_TYPE_FLOAT = 14,                /* TYPE_FLOAT  */
  YYSYMBOL_TYPE_STRING = 15,               /* TYPE_STRING  */
  YYSYMBOL_KW_MEAN = 16,                   /* KW_MEAN  */
  YYSYMBOL_KW_MEDIAN = 17,                 /* KW_MEDIAN  */
  YYSYMBOL_KW_STDDEV = 18,                 /* KW_STDDEV  */
  YYSYMBOL_KW_MIN = 19,                    /* KW_MIN  */
  YYSYMBOL_KW_MAX = 20,                    /* KW_MAX  */
  YYSYMBOL_KW_HISTOGRAM = 21,              /* KW_HISTOGRAM  */
  YYSYMBOL_KW_SUMMARY = 22,                /* KW_SUMMARY  */
  YYSYMBOL_EQ = 23,                        /* EQ  */
  YYSYMBOL_NEQ = 24,                       /* NEQ  */
  YYSYMBOL_LT = 25,                        /* LT  */
  YYSYMBOL_GT = 26,                        /* GT  */
  YYSYMBOL_LE = 27,                        /* LE  */
  YYSYMBOL_GE = 28,                        /* GE  */
  YYSYMBOL_IDENT = 29,                     /* IDENT  */
  YYSYMBOL_STRING_LITERAL = 30,            /* STRING_LITERAL  */
  YYSYMBOL_INUMBER = 31,                   /* INUMBER  */
  YYSYMBOL_FNUMBER = 32,                   /* FNUMBER  */
  YYSYMBOL_33_ = 33,                       /* '+'  */
  YYSYMBOL_34_ = 34,                       /* '-'  */
  YYSYMBOL_35_ = 35,                       /* '*'  */
  YYSYMBOL_36_ = 36,                       /* '/'  */
  YYSYMBOL_37_ = 37,                       /* ';'  */
  YYSYMBOL_38_ = 38,                       /* '{'  */
  YYSYMBOL_39_ = 39,                       /* '}'  */
  YYSYMBOL_40_ = 40,                       /* '='  */
  YYSYMBOL_41_ = 41,                       /* '.'  */
  YYSYMBOL_42_ = 42,                       /* '('  */
  YYSYMBOL_43_ = 43,                       /* ')'  */
  YYSYMBOL_YYACCEPT = 44,                  /* $accept  */
  YYSYMBOL_program = 45,                   /* program  */
  YYSYMBOL_stmt_list = 46,                 /* stmt_list  */
  YYSYMBOL_stmt = 47,                      /* stmt  */
  YYSYMBOL_source_stmt = 48,               /* source_stmt  */
  YYSYMBOL_schema_stmt = 49,               /* schema_stmt  */
  YYSYMBOL_field_list = 50,                /* field_list  */
  YYSYMBOL_field = 51,                     /* field  */
  YYSYMBOL_associate_stmt = 52,            /* associate_stmt  */
  YYSYMBOL_compute_stmt = 53,              /* compute_stmt  */
  YYSYMBOL_column_ref = 54,                /* column_ref  */
  YYSYMBOL_expression = 55,                /* expression  */
  YYSYMBOL_join_stmt = 56,                 /* join_stmt  */
  YYSYMBOL_filter_stmt = 57,               /* filter_stmt  */
  YYSYMBOL_condition = 58,                 /* condition  */
  YYSYMBOL_analyze_stmt = 59,              /* analyze_stmt  */
  YYSYMBOL_analyze_ops = 60,               /* analyze_ops  */
  YYSYMBOL_analyze_op = 61                 /* analyze_op  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  25
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   123

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  18
/* YYNRULES -- Number of rules.  */
#define YYNRULES  52
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  116

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   287


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      42,    43,    35,    33,     2,    34,    41,    36,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    37,
       2,    40,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    38,     2,    39,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    46,    46,    53,    57,    62,    68,    69,    70,    71,
      72,    73,    74,    79,    88,    96,    97,    98,   103,   108,
     113,   122,   131,   139,   149,   155,   161,   165,   169,   173,
     177,   181,   185,   189,   197,   206,   215,   219,   223,   227,
     231,   235,   243,   250,   251,   252,   257,   263,   269,   275,
     281,   287,   293
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SOURCE", "SCHEMA",
  "ASSOCIATE", "WITH", "COMPUTE", "ANALYZE", "JOIN", "INNER", "FILTER",
  "WHERE", "TYPE_INTEGER", "TYPE_FLOAT", "TYPE_STRING", "KW_MEAN",
  "KW_MEDIAN", "KW_STDDEV", "KW_MIN", "KW_MAX", "KW_HISTOGRAM",
  "KW_SUMMARY", "EQ", "NEQ", "LT", "GT", "LE", "GE", "IDENT",
  "STRING_LITERAL", "INUMBER", "FNUMBER", "'+'", "'-'", "'*'", "'/'",
  "';'", "'{'", "'}'", "'='", "'.'", "'('", "')'", "$accept", "program",
  "stmt_list", "stmt", "source_stmt", "schema_stmt", "field_list", "field",
  "associate_stmt", "compute_stmt", "column_ref", "expression",
  "join_stmt", "filter_stmt", "condition", "analyze_stmt", "analyze_ops",
  "analyze_op", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-43)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      41,   -25,    -9,    -8,    -6,     9,    22,    50,    36,    41,
     -43,   -43,   -43,   -43,   -43,   -43,   -43,   -43,    51,    42,
      76,    43,    46,    45,    47,   -43,   -43,    49,    59,    60,
     -13,    37,    61,    62,   -43,    63,     8,   -43,    55,    52,
     -43,   -43,   -43,   -13,   -43,    34,    65,    66,    67,    68,
      69,    70,    71,   -11,   -43,    52,    91,    90,    72,    73,
      74,   -43,   -43,   -43,    75,   -21,   -13,   -13,   -13,   -13,
     -43,    77,    78,    79,    80,    81,    82,    83,   -43,   -43,
      94,   -13,   -43,   -43,   -43,   -43,   -43,   -33,   -33,   -43,
     -43,   -43,   -43,   -43,   -43,   -43,   -43,   -43,    61,     7,
      84,    85,   -13,   -13,   -13,   -13,   -13,   -13,   -43,   -43,
      39,    39,    39,    39,    39,    39
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       5,     0,     0,     0,     0,     0,     0,     0,     0,     2,
       4,     6,     7,     8,     9,    10,    11,    12,     0,     0,
       0,     0,     0,     0,     0,     1,     3,     0,    17,     0,
       0,    45,     0,     0,    13,     0,     0,    16,     0,    27,
      26,    24,    25,     0,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    44,     0,     0,     0,     0,     0,
       0,    14,    15,    21,     0,     0,     0,     0,     0,     0,
      22,     0,     0,     0,     0,     0,     0,     0,    42,    43,
       0,     0,    18,    19,    20,    23,    29,    30,    31,    32,
      33,    46,    47,    48,    49,    50,    51,    52,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    35,    34,
      36,    37,    38,    39,    40,    41
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -43,   -43,   -43,    96,   -43,   -43,   -43,    87,   -43,   -43,
     -32,   -42,   -43,   -43,   -43,   -43,   -43,    53
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     8,     9,    10,    11,    12,    36,    37,    13,    14,
      44,    45,    15,    16,   100,    17,    53,    54
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      56,    65,    68,    69,    18,    46,    47,    48,    49,    50,
      51,    52,    66,    67,    68,    69,    39,    40,    41,    42,
      19,    20,    86,    21,    87,    88,    89,    90,    78,    43,
     102,   103,   104,   105,   106,   107,    25,    35,    22,    99,
      66,    67,    68,    69,     1,     2,     3,    61,     4,     5,
       6,    23,     7,    46,    47,    48,    49,    50,    51,    52,
     110,   111,   112,   113,   114,   115,   101,    66,    67,    68,
      69,    70,    66,    67,    68,    69,    58,    59,    60,    24,
      28,    27,    29,    30,    31,    32,    34,    33,    35,    38,
      55,    57,    63,    64,    71,    72,    73,    74,    75,    76,
      77,    80,    81,    98,    85,    26,    79,     0,     0,    82,
      83,    84,     0,     0,    91,    92,    93,    94,    95,    96,
      97,   108,   109,    62
};

static const yytype_int8 yycheck[] =
{
      32,    43,    35,    36,    29,    16,    17,    18,    19,    20,
      21,    22,    33,    34,    35,    36,    29,    30,    31,    32,
      29,    29,    43,    29,    66,    67,    68,    69,    39,    42,
      23,    24,    25,    26,    27,    28,     0,    29,    29,    81,
      33,    34,    35,    36,     3,     4,     5,    39,     7,     8,
       9,    29,    11,    16,    17,    18,    19,    20,    21,    22,
     102,   103,   104,   105,   106,   107,    98,    33,    34,    35,
      36,    37,    33,    34,    35,    36,    13,    14,    15,    29,
      38,    30,     6,    40,    38,    40,    37,    40,    29,    29,
      29,    29,    37,    41,    29,    29,    29,    29,    29,    29,
      29,    10,    12,     9,    29,     9,    53,    -1,    -1,    37,
      37,    37,    -1,    -1,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    36
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     7,     8,     9,    11,    45,    46,
      47,    48,    49,    52,    53,    56,    57,    59,    29,    29,
      29,    29,    29,    29,    29,     0,    47,    30,    38,     6,
      40,    38,    40,    40,    37,    29,    50,    51,    29,    29,
      30,    31,    32,    42,    54,    55,    16,    17,    18,    19,
      20,    21,    22,    60,    61,    29,    54,    29,    13,    14,
      15,    39,    51,    37,    41,    55,    33,    34,    35,    36,
      37,    29,    29,    29,    29,    29,    29,    29,    39,    61,
      10,    12,    37,    37,    37,    29,    43,    55,    55,    55,
      55,    37,    37,    37,    37,    37,    37,    37,     9,    55,
      58,    54,    23,    24,    25,    26,    27,    28,    37,    37,
      55,    55,    55,    55,    55,    55
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    44,    45,    46,    46,    46,    47,    47,    47,    47,
      47,    47,    47,    48,    49,    50,    50,    50,    51,    51,
      51,    52,    53,    54,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    56,    57,    58,    58,    58,    58,
      58,    58,    59,    60,    60,    60,    61,    61,    61,    61,
      61,    61,    61
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     4,     5,     2,     1,     0,     3,     3,
       3,     5,     5,     3,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     8,     7,     3,     3,     3,     3,
       3,     3,     5,     2,     1,     0,     3,     3,     3,     3,
       3,     3,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: stmt_list  */
#line 47 "parser.y"
    {
        root = ast_create(AST_PROGRAM, NULL, (yyvsp[0].node), NULL);
    }
#line 1207 "parser.tab.c"
    break;

  case 3: /* stmt_list: stmt_list stmt  */
#line 54 "parser.y"
    {
        (yyval.node) = ast_cons((yyvsp[-1].node), (yyvsp[0].node));
    }
#line 1215 "parser.tab.c"
    break;

  case 4: /* stmt_list: stmt  */
#line 58 "parser.y"
    {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1223 "parser.tab.c"
    break;

  case 5: /* stmt_list: %empty  */
#line 62 "parser.y"
    {
        (yyval.node) = NULL;
    }
#line 1231 "parser.tab.c"
    break;

  case 13: /* source_stmt: SOURCE IDENT STRING_LITERAL ';'  */
#line 80 "parser.y"
    {
        ASTNode *file = ast_create(AST_EXPRESSION, (yyvsp[-1].str), NULL, NULL);
        (yyval.node) = ast_create(AST_SOURCE, (yyvsp[-2].str), file, NULL);
    }
#line 1240 "parser.tab.c"
    break;

  case 14: /* schema_stmt: SCHEMA IDENT '{' field_list '}'  */
#line 89 "parser.y"
    {
        (yyval.node) = ast_create(AST_SCHEMA, (yyvsp[-3].str), (yyvsp[-1].node), NULL);
    }
#line 1248 "parser.tab.c"
    break;

  case 15: /* field_list: field_list field  */
#line 96 "parser.y"
                          { (yyval.node) = ast_cons((yyvsp[-1].node), (yyvsp[0].node)); }
#line 1254 "parser.tab.c"
    break;

  case 16: /* field_list: field  */
#line 97 "parser.y"
                          { (yyval.node) = (yyvsp[0].node); }
#line 1260 "parser.tab.c"
    break;

  case 17: /* field_list: %empty  */
#line 98 "parser.y"
                          { (yyval.node) = NULL; }
#line 1266 "parser.tab.c"
    break;

  case 18: /* field: IDENT TYPE_INTEGER ';'  */
#line 104 "parser.y"
    {
        ASTNode *t = ast_create(AST_FIELD, "integer", NULL, NULL);
        (yyval.node) = ast_create(AST_FIELD, (yyvsp[-2].str), t, NULL);
    }
#line 1275 "parser.tab.c"
    break;

  case 19: /* field: IDENT TYPE_FLOAT ';'  */
#line 109 "parser.y"
    {
        ASTNode *t = ast_create(AST_FIELD, "float", NULL, NULL);
        (yyval.node) = ast_create(AST_FIELD, (yyvsp[-2].str), t, NULL);
    }
#line 1284 "parser.tab.c"
    break;

  case 20: /* field: IDENT TYPE_STRING ';'  */
#line 114 "parser.y"
    {
        ASTNode *t = ast_create(AST_FIELD, "string", NULL, NULL);
        (yyval.node) = ast_create(AST_FIELD, (yyvsp[-2].str), t, NULL);
    }
#line 1293 "parser.tab.c"
    break;

  case 21: /* associate_stmt: ASSOCIATE IDENT WITH IDENT ';'  */
#line 123 "parser.y"
    {
        ASTNode *src = ast_create(AST_EXPRESSION, (yyvsp[-1].str), NULL, NULL);
        (yyval.node) = ast_create(AST_ASSOCIATE, (yyvsp[-3].str), src, NULL);
    }
#line 1302 "parser.tab.c"
    break;

  case 22: /* compute_stmt: COMPUTE IDENT '=' expression ';'  */
#line 132 "parser.y"
    {
        (yyval.node) = ast_create(AST_COMPUTE, (yyvsp[-3].str), (yyvsp[-1].node), NULL);
    }
#line 1310 "parser.tab.c"
    break;

  case 23: /* column_ref: IDENT '.' IDENT  */
#line 140 "parser.y"
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "%s.%s", (yyvsp[-2].str), (yyvsp[0].str));
        (yyval.node) = ast_create(AST_EXPRESSION, buf, NULL, NULL);
    }
#line 1320 "parser.tab.c"
    break;

  case 24: /* expression: INUMBER  */
#line 150 "parser.y"
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "%d", (yyvsp[0].ival));
        (yyval.node) = ast_create(AST_EXPRESSION, buf, NULL, NULL);
    }
#line 1330 "parser.tab.c"
    break;

  case 25: /* expression: FNUMBER  */
#line 156 "parser.y"
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", (yyvsp[0].fval));
        (yyval.node) = ast_create(AST_EXPRESSION, buf, NULL, NULL);
    }
#line 1340 "parser.tab.c"
    break;

  case 26: /* expression: STRING_LITERAL  */
#line 162 "parser.y"
    {
        (yyval.node) = ast_create(AST_EXPRESSION, (yyvsp[0].str), NULL, NULL);
    }
#line 1348 "parser.tab.c"
    break;

  case 27: /* expression: IDENT  */
#line 166 "parser.y"
    {
        (yyval.node) = ast_create(AST_EXPRESSION, (yyvsp[0].str), NULL, NULL);
    }
#line 1356 "parser.tab.c"
    break;

  case 28: /* expression: column_ref  */
#line 170 "parser.y"
    {
        (yyval.node) = (yyvsp[0].node);
    }
#line 1364 "parser.tab.c"
    break;

  case 29: /* expression: '(' expression ')'  */
#line 174 "parser.y"
    {
        (yyval.node) = (yyvsp[-1].node);
    }
#line 1372 "parser.tab.c"
    break;

  case 30: /* expression: expression '+' expression  */
#line 178 "parser.y"
    {
        (yyval.node) = ast_create(AST_EXPRESSION, "+", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1380 "parser.tab.c"
    break;

  case 31: /* expression: expression '-' expression  */
#line 182 "parser.y"
    {
        (yyval.node) = ast_create(AST_EXPRESSION, "-", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1388 "parser.tab.c"
    break;

  case 32: /* expression: expression '*' expression  */
#line 186 "parser.y"
    {
        (yyval.node) = ast_create(AST_EXPRESSION, "*", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1396 "parser.tab.c"
    break;

  case 33: /* expression: expression '/' expression  */
#line 190 "parser.y"
    {
        (yyval.node) = ast_create(AST_EXPRESSION, "/", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1404 "parser.tab.c"
    break;

  case 34: /* join_stmt: JOIN IDENT '=' column_ref INNER JOIN column_ref ';'  */
#line 198 "parser.y"
    {
        (yyval.node) = ast_create(AST_JOIN, (yyvsp[-6].str), (yyvsp[-4].node), (yyvsp[-1].node));
    }
#line 1412 "parser.tab.c"
    break;

  case 35: /* filter_stmt: FILTER IDENT '=' IDENT WHERE condition ';'  */
#line 207 "parser.y"
    {
        ASTNode *src = ast_create(AST_EXPRESSION, (yyvsp[-3].str), NULL, NULL);
        (yyval.node) = ast_create(AST_FILTER, (yyvsp[-5].str), src, (yyvsp[-1].node));
    }
#line 1421 "parser.tab.c"
    break;

  case 36: /* condition: expression EQ expression  */
#line 216 "parser.y"
    {
        (yyval.node) = ast_create(AST_CONDITION, "==", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1429 "parser.tab.c"
    break;

  case 37: /* condition: expression NEQ expression  */
#line 220 "parser.y"
    {
        (yyval.node) = ast_create(AST_CONDITION, "!=", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1437 "parser.tab.c"
    break;

  case 38: /* condition: expression LT expression  */
#line 224 "parser.y"
    {
        (yyval.node) = ast_create(AST_CONDITION, "<", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1445 "parser.tab.c"
    break;

  case 39: /* condition: expression GT expression  */
#line 228 "parser.y"
    {
        (yyval.node) = ast_create(AST_CONDITION, ">", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1453 "parser.tab.c"
    break;

  case 40: /* condition: expression LE expression  */
#line 232 "parser.y"
    {
        (yyval.node) = ast_create(AST_CONDITION, "<=", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1461 "parser.tab.c"
    break;

  case 41: /* condition: expression GE expression  */
#line 236 "parser.y"
    {
        (yyval.node) = ast_create(AST_CONDITION, ">=", (yyvsp[-2].node), (yyvsp[0].node));
    }
#line 1469 "parser.tab.c"
    break;

  case 42: /* analyze_stmt: ANALYZE IDENT '{' analyze_ops '}'  */
#line 244 "parser.y"
    {
        (yyval.node) = ast_create(AST_ANALYZE, (yyvsp[-3].str), (yyvsp[-1].node), NULL);
    }
#line 1477 "parser.tab.c"
    break;

  case 43: /* analyze_ops: analyze_ops analyze_op  */
#line 250 "parser.y"
                                { (yyval.node) = ast_cons((yyvsp[-1].node), (yyvsp[0].node)); }
#line 1483 "parser.tab.c"
    break;

  case 44: /* analyze_ops: analyze_op  */
#line 251 "parser.y"
                                { (yyval.node) = (yyvsp[0].node); }
#line 1489 "parser.tab.c"
    break;

  case 45: /* analyze_ops: %empty  */
#line 252 "parser.y"
                                { (yyval.node) = NULL; }
#line 1495 "parser.tab.c"
    break;

  case 46: /* analyze_op: KW_MEAN IDENT ';'  */
#line 258 "parser.y"
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "mean %s", (yyvsp[-1].str));
        (yyval.node) = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
#line 1505 "parser.tab.c"
    break;

  case 47: /* analyze_op: KW_MEDIAN IDENT ';'  */
#line 264 "parser.y"
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "median %s", (yyvsp[-1].str));
        (yyval.node) = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
#line 1515 "parser.tab.c"
    break;

  case 48: /* analyze_op: KW_STDDEV IDENT ';'  */
#line 270 "parser.y"
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "std_dev %s", (yyvsp[-1].str));
        (yyval.node) = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
#line 1525 "parser.tab.c"
    break;

  case 49: /* analyze_op: KW_MIN IDENT ';'  */
#line 276 "parser.y"
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "min %s", (yyvsp[-1].str));
        (yyval.node) = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
#line 1535 "parser.tab.c"
    break;

  case 50: /* analyze_op: KW_MAX IDENT ';'  */
#line 282 "parser.y"
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "max %s", (yyvsp[-1].str));
        (yyval.node) = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
#line 1545 "parser.tab.c"
    break;

  case 51: /* analyze_op: KW_HISTOGRAM IDENT ';'  */
#line 288 "parser.y"
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "histogram %s", (yyvsp[-1].str));
        (yyval.node) = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
#line 1555 "parser.tab.c"
    break;

  case 52: /* analyze_op: KW_SUMMARY IDENT ';'  */
#line 294 "parser.y"
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "summary %s", (yyvsp[-1].str));
        (yyval.node) = ast_create(AST_ANALYZE_OP, buf, NULL, NULL);
    }
#line 1565 "parser.tab.c"
    break;


#line 1569 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 301 "parser.y"


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

