/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
#line 1 "src/parser.y"

    #include <iostream>
    #include <assert.h>
    #include <string>
    using namespace std;
    #include "parser.h"
    extern FILE *yyout;
    extern Ast ast;
    int yylex();
    int yyerror( char const * );

    struct tempDeclArray{
        int level;
        string name;
        bool isDef = false;
        void* exp = nullptr;
        void* dim = nullptr;
        void* defArr = nullptr;
    };
    std::vector<tempDeclArray> tempDecl;
    std::vector<Type*> tempParaType;
    std::vector<Type*> tempArgType;
    std::vector<ExprNode*> tempArgList;

    //q15嵌套函数调用
    std::vector<std::vector<Type*>> vecArgType;
    std::vector<std::vector<ExprNode*>> vecArgList;
    //q18避免定义时出现未声明变量报错
    //上层type保留栈，只有一个元素
    Type* upperType = nullptr;

#line 99 "src/parser.cpp"




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

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_INCLUDE_PARSER_H_INCLUDED
# define YY_YY_INCLUDE_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 33 "src/parser.y"

    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"

#line 151 "src/parser.cpp"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID = 258,
    INTEGER = 259,
    FLOAT = 260,
    IF = 261,
    ELSE = 262,
    INT = 263,
    VOID = 264,
    FLT = 265,
    LPAREN = 266,
    RPAREN = 267,
    LSQUARE = 268,
    RSQUARE = 269,
    LBRACE = 270,
    RBRACE = 271,
    SEMICOLON = 272,
    ADD = 273,
    SUB = 274,
    OR = 275,
    AND = 276,
    LESS = 277,
    GREATER = 278,
    ASSIGN = 279,
    RETURN = 280,
    BREAK = 281,
    CONTINUE = 282,
    CONST = 283,
    FOR = 284,
    WHILE = 285,
    LOGIC_NOT = 286,
    LOGIC_AND = 287,
    LOGIC_OR = 288,
    PRODUCT = 289,
    DIVISION = 290,
    REMAINDER = 291,
    NOT = 292,
    XOR = 293,
    ADDITION_ASSIGNMENT = 294,
    SUBTRACTION_ASSIGNMENT = 295,
    MULTIPLICATION_ASSIGNMENT = 296,
    DIVISION_ASSIGNMENT = 297,
    MODULO_ASSIGNMENT = 298,
    EQUAL_TO = 299,
    NOT_EQUAL_TO = 300,
    LESS_EQUAL = 301,
    GREATER_EQUAL = 302,
    COMMA = 303,
    THEN = 304
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 39 "src/parser.y"

    int itype;
    float ftype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;

#line 221 "src/parser.cpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_INCLUDE_PARSER_H_INCLUDED  */



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
typedef yytype_uint8 yy_state_t;

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
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
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

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


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
#define YYFINAL  80
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   207

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  50
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  40
/* YYNRULES -- Number of rules.  */
#define YYNRULES  97
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  163

#define YYUNDEFTOK  2
#define YYMAXUTOK   304


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    76,    76,    90,    95,   103,   104,   105,   106,   107,
     108,   111,   114,   115,   116,   117,   120,   132,   138,   147,
     146,   155,   166,   172,   180,   180,   207,   213,   229,   236,
     243,   247,   254,   261,   274,   276,   289,   291,   304,   306,
     316,   329,   331,   341,   351,   361,   374,   386,   388,   409,
     433,   435,   455,   476,   497,   499,   501,   526,   551,   557,
     561,   567,   570,   577,   582,   585,   619,   675,   736,   746,
     752,   756,   761,   770,   774,   779,   786,   791,   799,   805,
     812,   818,   821,   831,   882,   896,   882,   938,   965,   974,
     983,   992,  1006,  1023,  1044,  1064,  1085,  1105
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "INTEGER", "FLOAT", "IF", "ELSE",
  "INT", "VOID", "FLT", "LPAREN", "RPAREN", "LSQUARE", "RSQUARE", "LBRACE",
  "RBRACE", "SEMICOLON", "ADD", "SUB", "OR", "AND", "LESS", "GREATER",
  "ASSIGN", "RETURN", "BREAK", "CONTINUE", "CONST", "FOR", "WHILE",
  "LOGIC_NOT", "LOGIC_AND", "LOGIC_OR", "PRODUCT", "DIVISION", "REMAINDER",
  "NOT", "XOR", "ADDITION_ASSIGNMENT", "SUBTRACTION_ASSIGNMENT",
  "MULTIPLICATION_ASSIGNMENT", "DIVISION_ASSIGNMENT", "MODULO_ASSIGNMENT",
  "EQUAL_TO", "NOT_EQUAL_TO", "LESS_EQUAL", "GREATER_EQUAL", "COMMA",
  "THEN", "$accept", "Stmts", "Stmt", "AssignStmt", "BlockStmt", "IfStmt",
  "ReturnStmt", "DeclStmt", "FuncDef", "BreakStmt", "ContinueStmt",
  "WhileStmt", "DimArray", "ArrayDefBlock", "ArrayDef", "FuncParam",
  "ExprStmt", "Exp", "AddExp", "Cond", "LOrExp", "PrimaryExp", "LVal",
  "RelExp", "LAndExp", "UnaryExp", "MulExp", "FuncCall", "ArrayIndex",
  "EqlExp", "Type", "Program", "$@1", "If", "$@2", "While", "DeclBody",
  "$@3", "$@4", "FuncArg", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304
};
# endif

#define YYPACT_NINF (-121)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-85)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     158,    66,  -121,  -121,  -121,  -121,  -121,  -121,   176,    -9,
    -121,   176,   176,    47,    22,    28,    51,  -121,   176,   158,
    -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,  -121,
    -121,    36,    29,   -16,  -121,    43,     9,    31,  -121,    34,
    -121,  -121,    42,    73,    72,    77,    79,   176,   176,    67,
      83,  -121,  -121,   158,  -121,  -121,  -121,    85,  -121,  -121,
      97,  -121,  -121,  -121,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,     3,    -7,
    -121,   176,   176,  -121,    -8,   108,   176,  -121,    88,  -121,
       2,    -4,    34,    34,    31,    94,    29,    29,    29,    29,
      42,  -121,  -121,  -121,     9,     9,   176,     6,   114,  -121,
     125,   123,   -16,   129,  -121,   176,  -121,   128,  -121,  -121,
    -121,  -121,   134,    51,    11,   158,   158,  -121,  -121,    18,
    -121,  -121,    96,   143,   176,    33,   140,  -121,  -121,  -121,
     -10,    51,   136,   137,  -121,   134,  -121,  -121,   134,   148,
     142,   141,  -121,   158,  -121,   145,  -121,   146,  -121,   156,
      67,   146,    67
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    16,    59,    60,    22,    63,    64,    65,     0,    19,
      11,     0,     0,     0,     0,     0,     0,    26,     0,     2,
       3,     5,     6,     7,     8,     9,    10,    13,    14,    15,
      12,     0,    41,    33,    54,    58,    38,    34,    50,    47,
      62,    17,    36,     0,     0,     0,     0,    90,     0,    83,
       0,    58,    21,     0,    55,    56,    31,     0,    28,    29,
       0,    57,     4,    32,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    69,     0,
       1,     0,     0,    89,     0,     0,     0,    61,     0,    30,
      69,     0,    48,    49,    35,     0,    42,    43,    44,    45,
      37,    51,    52,    53,    39,    40,     0,    71,     0,    66,
       0,     0,    46,     0,    87,     0,    77,     0,    20,    67,
      18,    73,     0,    97,    68,     0,     0,    88,    76,     0,
      75,    80,    85,     0,     0,    70,    23,    27,    82,    79,
       0,     0,     0,    92,    72,     0,    24,    81,     0,     0,
       0,     0,    74,     0,    78,    91,    86,    93,    25,     0,
      94,    95,    96
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -121,   118,   -17,  -121,    24,  -121,  -121,  -121,  -121,  -121,
    -121,  -121,    -1,  -120,  -121,  -121,  -121,    -5,    14,    90,
      39,  -121,     1,    55,   112,     0,    69,  -121,  -121,   110,
     -11,  -121,  -121,  -121,  -121,  -121,   130,  -121,  -121,  -121
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,   107,   130,   140,   132,    30,    31,    32,   111,
      33,    34,    51,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    53,    45,   153,    46,    79,   108,   142,    84
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      49,    35,    62,    50,   114,    60,   147,    52,    57,   139,
     109,    54,    55,   119,   -84,    48,    48,    66,    61,    86,
      35,     1,     2,     3,    48,   152,   106,   106,   154,     8,
     122,    68,    69,   129,   138,   134,    11,    12,   148,    58,
     115,   110,    83,    85,   110,    59,    86,    64,    65,    18,
       1,     2,     3,    63,    35,    70,    71,   145,     8,     5,
       6,     7,    95,    72,    56,    11,    12,    67,    73,    74,
      75,    62,    80,   101,   102,   103,    78,    47,    18,    48,
      86,   117,    96,    97,    98,    99,    76,    77,    81,    35,
      82,     1,     2,     3,     4,    87,     5,     6,     7,     8,
      90,   121,    89,     9,   118,    10,    11,    12,   136,   137,
     127,   120,   133,    13,    14,    15,    16,   131,    17,    18,
     112,   112,   116,   135,   131,   123,    35,    35,   124,   144,
     149,   104,   105,    92,    93,   125,   158,     1,     2,     3,
     131,   126,   128,   131,   141,     8,   143,   146,   150,   129,
     151,   155,    11,    12,    35,   157,   160,     9,   159,    48,
     162,     1,     2,     3,     4,    18,     5,     6,     7,     8,
     161,    88,   113,     9,   156,    10,    11,    12,    94,     1,
       2,     3,   100,    13,    14,    15,    16,     8,    17,    18,
      91,     0,     0,     0,    11,    12,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    18
};

static const yytype_int16 yycheck[] =
{
       1,     0,    19,     8,    12,    16,    16,    16,    13,   129,
      17,    11,    12,    17,    11,    13,    13,    33,    18,    13,
      19,     3,     4,     5,    13,   145,    24,    24,   148,    11,
      24,    22,    23,    15,    16,    24,    18,    19,    48,    17,
      48,    48,    47,    48,    48,    17,    13,    18,    19,    31,
       3,     4,     5,    17,    53,    46,    47,    24,    11,     8,
       9,    10,    67,    32,    17,    18,    19,    24,    34,    35,
      36,    88,     0,    73,    74,    75,     3,    11,    31,    13,
      13,    86,    68,    69,    70,    71,    44,    45,    11,    88,
      11,     3,     4,     5,     6,    12,     8,     9,    10,    11,
       3,   106,    17,    15,    16,    17,    18,    19,   125,   126,
     115,    17,   123,    25,    26,    27,    28,   122,    30,    31,
      81,    82,    14,   124,   129,    11,   125,   126,     3,   134,
     141,    76,    77,    64,    65,    12,   153,     3,     4,     5,
     145,    12,    14,   148,    48,    11,     3,     7,    12,    15,
      13,     3,    18,    19,   153,    14,   157,    15,    13,    13,
     161,     3,     4,     5,     6,    31,     8,     9,    10,    11,
      14,    53,    82,    15,   150,    17,    18,    19,    66,     3,
       4,     5,    72,    25,    26,    27,    28,    11,    30,    31,
      60,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    31
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     8,     9,    10,    11,    15,
      17,    18,    19,    25,    26,    27,    28,    30,    31,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      66,    67,    68,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    83,    85,    11,    13,    62,
      67,    72,    16,    82,    75,    75,    17,    67,    17,    17,
      80,    75,    52,    17,    18,    19,    33,    24,    22,    23,
      46,    47,    32,    34,    35,    36,    44,    45,     3,    86,
       0,    11,    11,    67,    89,    67,    13,    12,    51,    17,
       3,    86,    76,    76,    74,    67,    68,    68,    68,    68,
      79,    75,    75,    75,    73,    73,    24,    62,    87,    17,
      48,    69,    70,    69,    12,    48,    14,    67,    16,    17,
      17,    67,    24,    11,     3,    12,    12,    67,    14,    15,
      63,    67,    65,    80,    24,    62,    52,    52,    16,    63,
      64,    48,    88,     3,    67,    24,     7,    16,    48,    80,
      12,    13,    63,    84,    63,     3,    54,    14,    52,    13,
      62,    14,    62
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    50,    81,    51,    51,    52,    52,    52,    52,    52,
      52,    52,    52,    52,    52,    52,    72,    72,    53,    82,
      54,    54,    83,    55,    84,    55,    85,    61,    59,    60,
      56,    56,    66,    67,    70,    70,    74,    74,    79,    79,
      79,    73,    73,    73,    73,    73,    69,    68,    68,    68,
      76,    76,    76,    76,    75,    75,    75,    75,    71,    71,
      71,    71,    71,    80,    80,    80,    57,    57,    86,    86,
      86,    86,    86,    86,    86,    86,    62,    62,    64,    64,
      63,    63,    63,    78,    87,    88,    58,    77,    89,    89,
      89,    65,    65,    65,    65,    65,    65,    65
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     0,
       4,     2,     1,     5,     0,     8,     1,     5,     2,     2,
       3,     2,     2,     1,     1,     3,     1,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     1,     3,     3,
       1,     3,     3,     3,     1,     2,     2,     2,     1,     1,
       1,     3,     1,     1,     1,     1,     3,     4,     3,     1,
       4,     2,     5,     3,     6,     4,     4,     3,     3,     1,
       1,     3,     2,     2,     0,     0,     8,     4,     3,     1,
       0,     4,     2,     4,     5,     6,     7,     0
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



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

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
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
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
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
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
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
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
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

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
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
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
  case 2:
#line 76 "src/parser.y"
            {
        ast.setRoot((yyvsp[0].stmttype));
    }
#line 1528 "src/parser.cpp"
    break;

  case 3:
#line 90 "src/parser.y"
         {
    SeqNode* node = new SeqNode();
    node->addNext((StmtNode*)(yyvsp[0].stmttype));
    (yyval.stmttype) = (StmtNode*) node;
  }
#line 1538 "src/parser.cpp"
    break;

  case 4:
#line 95 "src/parser.y"
              {
    SeqNode* node = (SeqNode*)(yyvsp[-1].stmttype);
    node->addNext((StmtNode*)(yyvsp[0].stmttype));
    (yyval.stmttype) = (StmtNode*) node;
  }
#line 1548 "src/parser.cpp"
    break;

  case 5:
#line 103 "src/parser.y"
                 {(yyval.stmttype)=(yyvsp[0].stmttype);}
#line 1554 "src/parser.cpp"
    break;

  case 6:
#line 104 "src/parser.y"
                {(yyval.stmttype)=(yyvsp[0].stmttype);}
#line 1560 "src/parser.cpp"
    break;

  case 7:
#line 105 "src/parser.y"
             {(yyval.stmttype)=(yyvsp[0].stmttype);}
#line 1566 "src/parser.cpp"
    break;

  case 8:
#line 106 "src/parser.y"
                 {(yyval.stmttype)=(yyvsp[0].stmttype);}
#line 1572 "src/parser.cpp"
    break;

  case 9:
#line 107 "src/parser.y"
               {(yyval.stmttype)=(yyvsp[0].stmttype);}
#line 1578 "src/parser.cpp"
    break;

  case 10:
#line 108 "src/parser.y"
              {(yyval.stmttype)=(yyvsp[0].stmttype);}
#line 1584 "src/parser.cpp"
    break;

  case 11:
#line 111 "src/parser.y"
                {(yyval.stmttype) = new EmptyStmt();}
#line 1590 "src/parser.cpp"
    break;

  case 12:
#line 114 "src/parser.y"
               { (yyval.stmttype) = (yyvsp[0].stmttype); }
#line 1596 "src/parser.cpp"
    break;

  case 13:
#line 115 "src/parser.y"
                {(yyval.stmttype) = (yyvsp[0].stmttype);}
#line 1602 "src/parser.cpp"
    break;

  case 14:
#line 116 "src/parser.y"
                   {(yyval.stmttype) = (yyvsp[0].stmttype);}
#line 1608 "src/parser.cpp"
    break;

  case 15:
#line 117 "src/parser.y"
                {(yyval.stmttype) = (yyvsp[0].stmttype);}
#line 1614 "src/parser.cpp"
    break;

  case 16:
#line 120 "src/parser.y"
         {
        SymbolEntry *se;
        se = identifiers->lookup((yyvsp[0].strtype));
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)(yyvsp[0].strtype));
            delete [](char*)(yyvsp[0].strtype);
            assert(se != nullptr);
        }
        (yyval.exprtype) = new Id(se);
        delete [](yyvsp[0].strtype);
    }
#line 1631 "src/parser.cpp"
    break;

  case 17:
#line 132 "src/parser.y"
                {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 1639 "src/parser.cpp"
    break;

  case 18:
#line 138 "src/parser.y"
                              {
        // if($3->getSymbolEntry()->getType()->isFloat())
            // std::cout<<((ConstantSymbolEntry*)$3->getSymbolEntry())->getValueFloat()<<std::endl;
        (yyval.stmttype) = new AssignStmt((yyvsp[-3].exprtype), (yyvsp[-1].exprtype));
    }
#line 1649 "src/parser.cpp"
    break;

  case 19:
#line 147 "src/parser.y"
        {identifiers = new SymbolTable(identifiers);}
#line 1655 "src/parser.cpp"
    break;

  case 20:
#line 149 "src/parser.y"
        {
            (yyval.stmttype) = new CompoundStmt((yyvsp[-1].stmttype));
            SymbolTable *top = identifiers;
            identifiers = identifiers->getPrev();
            delete top;
        }
#line 1666 "src/parser.cpp"
    break;

  case 21:
#line 155 "src/parser.y"
                     {
            (yyval.stmttype) = new CompoundStmt(nullptr);
        }
#line 1674 "src/parser.cpp"
    break;

  case 22:
#line 166 "src/parser.y"
      {
        identifiers = new SymbolTable(identifiers);
    }
#line 1682 "src/parser.cpp"
    break;

  case 23:
#line 172 "src/parser.y"
                                          {
        //一对{}及其内部整体要算一个
        (yyval.stmttype) = new IfStmt((yyvsp[-2].exprtype), (yyvsp[0].stmttype));
        //从符号表退出
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
#line 1695 "src/parser.cpp"
    break;

  case 24:
#line 180 "src/parser.y"
                                     {
        //先退出一次
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        identifiers = new SymbolTable(identifiers);
    }
#line 1707 "src/parser.cpp"
    break;

  case 25:
#line 186 "src/parser.y"
           {
        (yyval.stmttype) = new IfElseStmt((yyvsp[-5].exprtype), (yyvsp[-3].stmttype), (yyvsp[0].stmttype));
        //从符号表退出
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top; 
    }
#line 1719 "src/parser.cpp"
    break;

  case 26:
#line 207 "src/parser.y"
         {
        identifiers = new SymbolTable(identifiers);
    }
#line 1727 "src/parser.cpp"
    break;

  case 27:
#line 213 "src/parser.y"
                                  {
        //一对{}及其内部整体要算一个
        (yyval.stmttype) = new WhileStmt((yyvsp[-2].exprtype), (yyvsp[0].stmttype));
        //从符号表退出
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
#line 1740 "src/parser.cpp"
    break;

  case 28:
#line 229 "src/parser.y"
                    {
        (yyval.stmttype) = new BreakStmt();
    }
#line 1748 "src/parser.cpp"
    break;

  case 29:
#line 236 "src/parser.y"
                       {
        (yyval.stmttype) = new ContinueStmt();
    }
#line 1756 "src/parser.cpp"
    break;

  case 30:
#line 243 "src/parser.y"
                         {
        (yyval.stmttype) = new ReturnStmt((yyvsp[-1].exprtype));
    }
#line 1764 "src/parser.cpp"
    break;

  case 31:
#line 247 "src/parser.y"
                    {
        (yyval.stmttype) = new ReturnStmt(nullptr);
    }
#line 1772 "src/parser.cpp"
    break;

  case 32:
#line 254 "src/parser.y"
                 {
        (yyval.stmttype) = new ExprStmt((yyvsp[-1].exprtype));
    }
#line 1780 "src/parser.cpp"
    break;

  case 33:
#line 261 "src/parser.y"
           {
        //p3表达式结果不能为void
        // if($1->getSymbolEntry()->getType()->getKind() == TypeSystem::voidType->getKind()
        //     || $1->getSymbolEntry()->getType()->getKind() == TypeSystem::funcType->getKind()){
        //     std::cout<<"错误！表达式结果为VOID类型或FUNC类型！"<<std::endl;
        // }
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 1793 "src/parser.cpp"
    break;

  case 34:
#line 274 "src/parser.y"
            {(yyval.exprtype) = (yyvsp[0].exprtype);}
#line 1799 "src/parser.cpp"
    break;

  case 35:
#line 277 "src/parser.y"
    {
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::LOGIC_OR, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 1812 "src/parser.cpp"
    break;

  case 36:
#line 289 "src/parser.y"
           {(yyval.exprtype) = (yyvsp[0].exprtype);}
#line 1818 "src/parser.cpp"
    break;

  case 37:
#line 292 "src/parser.y"
    {
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::LOGIC_AND, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 1831 "src/parser.cpp"
    break;

  case 38:
#line 304 "src/parser.y"
           {(yyval.exprtype) = (yyvsp[0].exprtype);}
#line 1837 "src/parser.cpp"
    break;

  case 39:
#line 307 "src/parser.y"
    {
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::EQUAL_TO, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 1850 "src/parser.cpp"
    break;

  case 40:
#line 317 "src/parser.y"
    {
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::NOT_EQUAL_TO, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 1863 "src/parser.cpp"
    break;

  case 41:
#line 329 "src/parser.y"
           {(yyval.exprtype) = (yyvsp[0].exprtype);}
#line 1869 "src/parser.cpp"
    break;

  case 42:
#line 332 "src/parser.y"
    {
         SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::LESS, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 1882 "src/parser.cpp"
    break;

  case 43:
#line 342 "src/parser.y"
    {
         SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::GREATER, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 1895 "src/parser.cpp"
    break;

  case 44:
#line 352 "src/parser.y"
    {
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::LESS_EQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 1908 "src/parser.cpp"
    break;

  case 45:
#line 362 "src/parser.y"
    {
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::GREATER_EQUAL, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 1921 "src/parser.cpp"
    break;

  case 46:
#line 374 "src/parser.y"
           {
        //p2条件表达式结果类型
        if((yyvsp[0].exprtype)->getSymbolEntry()->getType()->getKind() > TypeSystem::floatType->getKind()){
            std::cout<<"错误！条件判断结果不合法！"<<std::endl;
        }
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 1933 "src/parser.cpp"
    break;

  case 47:
#line 386 "src/parser.y"
           {(yyval.exprtype) = (yyvsp[0].exprtype);}
#line 1939 "src/parser.cpp"
    break;

  case 48:
#line 389 "src/parser.y"
    {
        SymbolEntry *se;
        int k1 = (yyvsp[-2].exprtype)->getSymbolEntry()->getType()->getKind(),k2 = (yyvsp[0].exprtype)->getSymbolEntry()->getType()->getKind();
        if((yyvsp[-2].exprtype)->getSymbolEntry()->isConstant() && (yyvsp[0].exprtype)->getSymbolEntry()->isConstant()){
            ConstantSymbolEntry *temp1 = (ConstantSymbolEntry*)((yyvsp[-2].exprtype)->getSymbolEntry()), *temp2 = (ConstantSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry());
            // se = new ConstantSymbolEntry( k1 >= k2 ? temp1->getType() : temp2->getType(),
            // (temp1->isInt() ? temp1->getValueInt() : temp1->getValueFloat()) + (temp2->isInt() ? temp2->getValueInt() : temp2->getValueFloat()));
            if(temp1->isInt() && temp2->isInt()){
                se = new ConstantSymbolEntry(TypeSystem::intType, temp1->getValueInt() + temp2->getValueInt());
                // std::cout<<temp1->getValueInt()<<" "<<temp2->getValueInt()<<std::endl;
            }else{
                se = new ConstantSymbolEntry(TypeSystem::floatType, (temp1->isInt() ? temp1->getValueInt() : temp1->getValueFloat()) + (temp2->isInt() ? temp2->getValueInt() : temp2->getValueFloat()));
            }
        } else{
            se = new TemporarySymbolEntry( k1 >= k2 ? (yyvsp[-2].exprtype)->getSymbolEntry()->getType() : (yyvsp[0].exprtype)->getSymbolEntry()->getType() , SymbolTable::getLabel());
        }
        // se = new TemporarySymbolEntry( k1 >= k2 ? $1->getSymbolEntry()->getType() : $3->getSymbolEntry()->getType() , SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::ADD, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 1963 "src/parser.cpp"
    break;

  case 49:
#line 410 "src/parser.y"
    {
        SymbolEntry *se;
        int k1 = (yyvsp[-2].exprtype)->getSymbolEntry()->getType()->getKind(),k2 = (yyvsp[0].exprtype)->getSymbolEntry()->getType()->getKind();
        if((yyvsp[-2].exprtype)->getSymbolEntry()->isConstant() && (yyvsp[0].exprtype)->getSymbolEntry()->isConstant()){
            ConstantSymbolEntry *temp1 = (ConstantSymbolEntry*)((yyvsp[-2].exprtype)->getSymbolEntry()), *temp2 = (ConstantSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry());
            // se = new ConstantSymbolEntry( k1 >= k2 ? temp1->getType() : temp2->getType(),
            // (temp1->isInt() ? temp1->getValueInt() : temp1->getValueFloat()) - (temp2->isInt() ? temp2->getValueInt() : temp2->getValueFloat()));
            if(temp1->isInt() && temp2->isInt()){
                se = new ConstantSymbolEntry(TypeSystem::intType, temp1->getValueInt() - temp2->getValueInt());
                // std::cout<<temp1->getValueInt()<<" "<<temp2->getValueInt()<<std::endl;
            }else{
                se = new ConstantSymbolEntry(TypeSystem::floatType, (temp1->isInt() ? temp1->getValueInt() : temp1->getValueFloat()) - (temp2->isInt() ? temp2->getValueInt() : temp2->getValueFloat()));
            }
        } else{
            se = new TemporarySymbolEntry( k1 >= k2 ? (yyvsp[-2].exprtype)->getSymbolEntry()->getType() : (yyvsp[0].exprtype)->getSymbolEntry()->getType() , SymbolTable::getLabel());
        }
        // se = new TemporarySymbolEntry( k1 >= k2 ? $1->getSymbolEntry()->getType() : $3->getSymbolEntry()->getType() , SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::SUB, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 1987 "src/parser.cpp"
    break;

  case 50:
#line 433 "src/parser.y"
             {(yyval.exprtype) = (yyvsp[0].exprtype);}
#line 1993 "src/parser.cpp"
    break;

  case 51:
#line 436 "src/parser.y"
    {
        SymbolEntry *se;
        int k1 = (yyvsp[-2].exprtype)->getSymbolEntry()->getType()->getKind(),k2 = (yyvsp[0].exprtype)->getSymbolEntry()->getType()->getKind();
        if((yyvsp[-2].exprtype)->getSymbolEntry()->isConstant() && (yyvsp[0].exprtype)->getSymbolEntry()->isConstant()){
            ConstantSymbolEntry *temp1 = (ConstantSymbolEntry*)((yyvsp[-2].exprtype)->getSymbolEntry()), *temp2 = (ConstantSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry());
            // se = new ConstantSymbolEntry( k1 >= k2 ? temp1->getType() : temp2->getType(),
            // (temp1->isInt() ? temp1->getValueInt() : temp1->getValueFloat()) * (temp2->isInt() ? temp2->getValueInt() : temp2->getValueFloat()));
            if(temp1->isInt() && temp2->isInt()){
                se = new ConstantSymbolEntry(TypeSystem::intType, temp1->getValueInt() * temp2->getValueInt());
                // std::cout<<temp1->getValueInt() * temp2->getValueInt()<<std::endl;
            }else{
                se = new ConstantSymbolEntry(TypeSystem::floatType, (temp1->isInt() ? temp1->getValueInt() : temp1->getValueFloat()) * (temp2->isInt() ? temp2->getValueInt() : temp2->getValueFloat()));
            }
        } else{
            se = new TemporarySymbolEntry( k1 >= k2 ? (yyvsp[-2].exprtype)->getSymbolEntry()->getType() : (yyvsp[0].exprtype)->getSymbolEntry()->getType() , SymbolTable::getLabel());
        }
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::PRODUCT, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 2016 "src/parser.cpp"
    break;

  case 52:
#line 456 "src/parser.y"
    {
        SymbolEntry *se;
        int k1 = (yyvsp[-2].exprtype)->getSymbolEntry()->getType()->getKind(),k2 = (yyvsp[0].exprtype)->getSymbolEntry()->getType()->getKind();
        if((yyvsp[-2].exprtype)->getSymbolEntry()->isConstant() && (yyvsp[0].exprtype)->getSymbolEntry()->isConstant()){
            ConstantSymbolEntry *temp1 = (ConstantSymbolEntry*)((yyvsp[-2].exprtype)->getSymbolEntry()), *temp2 = (ConstantSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry());
            // se = new ConstantSymbolEntry( k1 >= k2 ? temp1->getType() : temp2->getType(),
            // (temp1->isInt() ? temp1->getValueInt() : temp1->getValueFloat()) / (temp2->isInt() ? temp2->getValueInt() : temp2->getValueFloat()));
            if(temp1->isInt() && temp2->isInt()){
                se = new ConstantSymbolEntry(TypeSystem::intType, temp1->getValueInt() / temp2->getValueInt());
                // std::cout<<temp1->getValueInt() / temp2->getValueInt()<<std::endl;
            }else{
                se = new ConstantSymbolEntry(TypeSystem::floatType, (temp1->isInt() ? temp1->getValueInt() : temp1->getValueFloat()) / (temp2->isInt() ? temp2->getValueInt() : temp2->getValueFloat()));
            }
        } else{
            se = new TemporarySymbolEntry( k1 >= k2 ? (yyvsp[-2].exprtype)->getSymbolEntry()->getType() : (yyvsp[0].exprtype)->getSymbolEntry()->getType() , SymbolTable::getLabel());
        }
        // se = new TemporarySymbolEntry( k1 >= k2 ? $1->getSymbolEntry()->getType() : $3->getSymbolEntry()->getType() , SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::DIVISION, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 2040 "src/parser.cpp"
    break;

  case 53:
#line 477 "src/parser.y"
    {
        SymbolEntry *se;
        int k1 = (yyvsp[-2].exprtype)->getSymbolEntry()->getType()->getKind(),k2 = (yyvsp[0].exprtype)->getSymbolEntry()->getType()->getKind();
        if((yyvsp[-2].exprtype)->getSymbolEntry()->getType()->isFloat() || (yyvsp[0].exprtype)->getSymbolEntry()->getType()->isFloat()){
            std::cout<<"错误！取余两个运算符必须均为整数！"<<std::endl;
        }
        if((yyvsp[-2].exprtype)->getSymbolEntry()->isConstant() && (yyvsp[0].exprtype)->getSymbolEntry()->isConstant()){
            ConstantSymbolEntry *temp1 = (ConstantSymbolEntry*)((yyvsp[-2].exprtype)->getSymbolEntry()), *temp2 = (ConstantSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry());
            se = new ConstantSymbolEntry( k1 >= k2 ? temp1->getType() : temp2->getType(),
            (temp1->isInt() ? temp1->getValueInt() : (int)temp1->getValueFloat()) % (temp2->isInt() ? temp2->getValueInt() : (int)temp2->getValueFloat()));
        } else{
            se = new TemporarySymbolEntry( k1 >= k2 ? (yyvsp[-2].exprtype)->getSymbolEntry()->getType() : (yyvsp[0].exprtype)->getSymbolEntry()->getType() , SymbolTable::getLabel());
        }
        // se = new TemporarySymbolEntry( k1 >= k2 ? $1->getSymbolEntry()->getType() : $3->getSymbolEntry()->getType() , SymbolTable::getLabel());
        (yyval.exprtype) = new BinaryExpr(se, BinaryExpr::REMAINDER, (yyvsp[-2].exprtype), (yyvsp[0].exprtype));
    }
#line 2061 "src/parser.cpp"
    break;

  case 54:
#line 497 "src/parser.y"
               {(yyval.exprtype) = (yyvsp[0].exprtype);}
#line 2067 "src/parser.cpp"
    break;

  case 55:
#line 499 "src/parser.y"
                 {(yyval.exprtype) = (yyvsp[0].exprtype);}
#line 2073 "src/parser.cpp"
    break;

  case 56:
#line 502 "src/parser.y"
    {
        //q6浮点数支持
        SymbolEntry *se;
        //p11计算常量数值
        if((yyvsp[0].exprtype)->getSymbolEntry()->isConstant()){
            // 因为所有的？：运算符都会导致类型的错误，所以要全部进行更改
            if(((ConstantSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry()))->isInt()){
                se = new ConstantSymbolEntry((yyvsp[0].exprtype)->getSymbolEntry()->getType(), -(((ConstantSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry()))->getValueInt()));
                // std::cout<<-(((ConstantSymbolEntry*)($2->getSymbolEntry()))->getValueInt())<<std::endl;
            }else{
                se = new ConstantSymbolEntry((yyvsp[0].exprtype)->getSymbolEntry()->getType(), -(((ConstantSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry()))->getValueFloat()));
            }
            // se = new ConstantSymbolEntry($2->getSymbolEntry()->getType(), 
            //     -(((ConstantSymbolEntry*)($2->getSymbolEntry()))->isInt() ? ((ConstantSymbolEntry*)($2->getSymbolEntry()))->getValueInt() : ((ConstantSymbolEntry*)($2->getSymbolEntry()))->getValueFloat()));
        }else{
            // if($2->getSymbolEntry()->getType()->isInt())
            //     se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            // else
            //     se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
            se = new TemporarySymbolEntry((yyvsp[0].exprtype)->getSymbolEntry()->getType(), SymbolTable::getLabel());
        }
        (yyval.exprtype) = new UnaryExpr(se, UnaryExpr::SUB, (yyvsp[0].exprtype));
    }
#line 2101 "src/parser.cpp"
    break;

  case 57:
#line 527 "src/parser.y"
    {
        //p4二元运算类型检查
        if(!(yyvsp[0].exprtype)->getSymbolEntry()->getType()->isNumber()){
            std::cout<<"错误！一元运算出现非法类型！"<<std::endl;
        }
        //q6浮点数支持
        SymbolEntry *se;
        if((yyvsp[0].exprtype)->getSymbolEntry()->isConstant()){
            // se = new ConstantSymbolEntry($2->getSymbolEntry()->getType(), 
            //     !(((ConstantSymbolEntry*)($2->getSymbolEntry()))->isInt()? ((ConstantSymbolEntry*)($2->getSymbolEntry()))->getValueInt() : ((ConstantSymbolEntry*)($2->getSymbolEntry()))->getValueFloat()));
            if(((ConstantSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry()))->isInt()){
                se = new ConstantSymbolEntry((yyvsp[0].exprtype)->getSymbolEntry()->getType(), !((ConstantSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry()))->getValueInt());
            }else{
                se = new ConstantSymbolEntry((yyvsp[0].exprtype)->getSymbolEntry()->getType(), !((ConstantSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry()))->getValueFloat());
            }
        }else{
            se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        }
        (yyval.exprtype) = new UnaryExpr(se, UnaryExpr::LOGIC_NOT, (yyvsp[0].exprtype));
    }
#line 2126 "src/parser.cpp"
    break;

  case 58:
#line 551 "src/parser.y"
         {
        if((yyvsp[0].exprtype)->getSymbolEntry()->isConstant())
            (yyval.exprtype) = new Constant(((IdentifierSymbolEntry*)((yyvsp[0].exprtype)->getSymbolEntry()))->getGlbValue());
        else
            (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 2137 "src/parser.cpp"
    break;

  case 59:
#line 557 "src/parser.y"
              {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, (yyvsp[0].itype));
        (yyval.exprtype) = new Constant(se);
    }
#line 2146 "src/parser.cpp"
    break;

  case 60:
#line 561 "src/parser.y"
            {
        //q6浮点数支持
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::floatType, (yyvsp[0].ftype));
        // std::cout<<$1<<std::endl;
        (yyval.exprtype) = new Constant(se);
    }
#line 2157 "src/parser.cpp"
    break;

  case 61:
#line 567 "src/parser.y"
                        {
        (yyval.exprtype) = (yyvsp[-1].exprtype);
    }
#line 2165 "src/parser.cpp"
    break;

  case 62:
#line 570 "src/parser.y"
               {
        (yyval.exprtype) = (yyvsp[0].exprtype);
    }
#line 2173 "src/parser.cpp"
    break;

  case 63:
#line 577 "src/parser.y"
        {
        //这里使得符号表项中的的Type指针能指Type类型的值
        (yyval.type) = TypeSystem::intType;
        upperType = TypeSystem::intType;
    }
#line 2183 "src/parser.cpp"
    break;

  case 64:
#line 582 "src/parser.y"
           {
        (yyval.type) = TypeSystem::voidType;
    }
#line 2191 "src/parser.cpp"
    break;

  case 65:
#line 585 "src/parser.y"
          {
        //q6浮点数支持
        (yyval.type) = TypeSystem::floatType;
        upperType = TypeSystem::floatType;
    }
#line 2201 "src/parser.cpp"
    break;

  case 66:
#line 623 "src/parser.y"
                      {
        SymbolEntry *se;
        auto n = new DeclStmt();
        for(auto i:tempDecl){
            //p12数组定义检查下标可计算
            if(i.dim){
                for(auto& idx:*(((DimArray*)(i.dim))->getDimList())){
                    if(!idx->getSymbolEntry()->isConstant()){
                        std::cout<<"错误！数组定义下标为变量！"<<std::endl;
                        break;
                    }
                    if(!idx->getSymbolEntry()->getType()->isInt()){
                        std::cout<<"错误！数组定义下标不是整数！"<<std::endl;
                        break;
                    }
                }
            }
            if (i.dim){
                if((yyvsp[-2].type)->isInt()){
                    auto t = new ArrayIntType(*(((DimArray*)i.dim)->getDimList()), (yyvsp[-2].type));
                    se = new IdentifierSymbolEntry(t, i.name, i.level);
                }
                else{
                    auto t = new ArrayFloatType(*(((DimArray*)i.dim)->getDimList()), (yyvsp[-2].type));
                    se = new IdentifierSymbolEntry(t, i.name, i.level);
                }
            }
            else{
                se = new IdentifierSymbolEntry((yyvsp[-2].type), i.name, i.level);
            }
            identifiers->install(i.name, se);            
            //q3添加DefStmt变量常量定义语句
            if(i.isDef)
                //q8数组支持
                if(i.dim)
                    if(i.defArr){
                        n->addDecl(new Id(se), (ExprNode*)i.exp, (DimArray*)i.dim, (ArrayDef*)i.defArr);
                    }
                    else
                        n->addDecl(new Id(se), (ExprNode*)i.exp, (DimArray*)i.dim);
                else
                    n->addDecl(new Id(se), (ExprNode*)i.exp);
            else
                if(i.dim)
                    n->addDecl(new Id(se), nullptr, (DimArray*)i.dim);
                else
                    n->addDecl(new Id(se));
        }
        (yyval.stmttype) = (StmtNode*)n;
        std::vector<tempDeclArray>().swap(tempDecl);
    }
#line 2257 "src/parser.cpp"
    break;

  case 67:
#line 679 "src/parser.y"
                      {
        SymbolEntry *se;
        auto n = new DeclStmt();
        for(auto i:tempDecl){
            //p12数组定义检查下标可计算
            if(i.dim){
                for(auto& idx:*(((DimArray*)(i.dim))->getDimList())){
                    if(!idx->getSymbolEntry()->isConstant()){
                        std::cout<<"错误！数组定义下标为变量！"<<std::endl;
                        break;
                    }
                    if(!idx->getSymbolEntry()->getType()->isInt()){
                        std::cout<<"错误！数组定义下标不是整数！"<<std::endl;
                        break;
                    }
                }
            }

            if (i.dim){
                if((yyvsp[-2].type)->isInt()){
                    auto t = new ArrayIntType(*(((DimArray*)i.dim)->getDimList()), (yyvsp[-2].type));
                    se = new IdentifierSymbolEntry(t, i.name, i.level, SymbolEntry::CONSTANT);
                }
                else{
                    auto t = new ArrayFloatType(*(((DimArray*)i.dim)->getDimList()), (yyvsp[-2].type));
                    se = new IdentifierSymbolEntry(t, i.name, i.level, SymbolEntry::CONSTANT);
                }
            }
            else{
                se = new IdentifierSymbolEntry((yyvsp[-2].type), i.name, i.level, SymbolEntry::CONSTANT);
                if((((ExprNode*)i.exp)->getSymbolEntry())->isConstant())
                    ((IdentifierSymbolEntry*)se)->setGlbConst(((ExprNode*)i.exp)->getSymbolEntry());
            }
            identifiers->install(i.name, se);
            //q3添加DefStmt变量常量定义语句
            if(i.isDef)
                //q8数组支持
                if(i.dim)
                    if(i.defArr)
                        n->addDecl(new Id(se), (ExprNode*)i.exp, (DimArray*)i.dim, (ArrayDef*)i.defArr);
                    else
                        n->addDecl(new Id(se), (ExprNode*)i.exp, (DimArray*)i.dim);
                else
                    n->addDecl(new Id(se), (ExprNode*)i.exp);
            else
                if(i.dim)
                    n->addDecl(new Id(se), nullptr, (DimArray*)i.dim);
                else
                    n->addDecl(new Id(se));
        }
        (yyval.stmttype) = (StmtNode*)n;
        std::vector<tempDeclArray>().swap(tempDecl);
    }
#line 2315 "src/parser.cpp"
    break;

  case 68:
#line 736 "src/parser.y"
                     {
        //q18避免定义时出现未声明变量报错
        // int a =10, b = a+1;
        auto tempEntry = new IdentifierSymbolEntry(upperType, (yyvsp[0].strtype), identifiers->getLevel());
        tempEntry->setIsTemp(true);
        identifiers->install((yyvsp[0].strtype), tempEntry); 

        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), (yyvsp[0].strtype)});
        delete [](yyvsp[0].strtype);
    }
#line 2330 "src/parser.cpp"
    break;

  case 69:
#line 746 "src/parser.y"
       {
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), (yyvsp[0].strtype)});
        delete [](yyvsp[0].strtype);
    }
#line 2339 "src/parser.cpp"
    break;

  case 70:
#line 752 "src/parser.y"
                              {
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), (yyvsp[-1].strtype), false, nullptr, (yyvsp[0].stmttype)});
        delete [](yyvsp[-1].strtype);
    }
#line 2348 "src/parser.cpp"
    break;

  case 71:
#line 756 "src/parser.y"
                {
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), (yyvsp[-1].strtype), false, nullptr, (yyvsp[0].stmttype)});
        delete [](yyvsp[-1].strtype);
    }
#line 2357 "src/parser.cpp"
    break;

  case 72:
#line 761 "src/parser.y"
                                 {
        //q18避免定义时出现未声明变量报错
        auto tempEntry = new IdentifierSymbolEntry(upperType, (yyvsp[-2].strtype), identifiers->getLevel());
        tempEntry->setIsTemp(true);
        identifiers->install((yyvsp[-2].strtype), tempEntry); 

        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), (yyvsp[-2].strtype), true, (yyvsp[0].exprtype)});
        delete [](yyvsp[-2].strtype);
    }
#line 2371 "src/parser.cpp"
    break;

  case 73:
#line 770 "src/parser.y"
                  {
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), (yyvsp[-2].strtype), true, (yyvsp[0].exprtype)});
        delete [](yyvsp[-2].strtype);
    }
#line 2380 "src/parser.cpp"
    break;

  case 74:
#line 774 "src/parser.y"
                                                    {
        assert((yyvsp[0].stmttype) != nullptr);
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), (yyvsp[-3].strtype), true, nullptr, (yyvsp[-2].stmttype), (yyvsp[0].stmttype)});
        delete [](yyvsp[-3].strtype);
    }
#line 2390 "src/parser.cpp"
    break;

  case 75:
#line 779 "src/parser.y"
                                     {
        assert((yyvsp[0].stmttype) != nullptr);
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), (yyvsp[-3].strtype), true, nullptr, (yyvsp[-2].stmttype), (yyvsp[0].stmttype)});
        delete [](yyvsp[-3].strtype);
    }
#line 2400 "src/parser.cpp"
    break;

  case 76:
#line 786 "src/parser.y"
                                 {
        auto dimArr = (DimArray*)(yyvsp[-3].stmttype);
        dimArr->addDim((ExprNode*)(yyvsp[-1].exprtype));
        (yyval.stmttype) = (StmtNode*) dimArr;
    }
#line 2410 "src/parser.cpp"
    break;

  case 77:
#line 791 "src/parser.y"
                        {
        auto dim = new DimArray();
        dim->addDim((ExprNode*)(yyvsp[-1].exprtype));
        (yyval.stmttype) = (StmtNode*) dim;
    }
#line 2420 "src/parser.cpp"
    break;

  case 78:
#line 799 "src/parser.y"
                                  {
        // 向defArray中push
        auto a = (ArrayDef *)(yyvsp[-2].stmttype);
        a->addDef((ArrayDef *)(yyvsp[0].stmttype));
        (yyval.stmttype) = (StmtNode*)a;
    }
#line 2431 "src/parser.cpp"
    break;

  case 79:
#line 805 "src/parser.y"
                   {
        auto n = new ArrayDef();
        n->addDef((ArrayDef *)(yyvsp[0].stmttype));
        (yyval.stmttype) = (StmtNode*)n;
    }
#line 2441 "src/parser.cpp"
    break;

  case 80:
#line 812 "src/parser.y"
            {
        // 新开辟一个只有leaf的ArrayDef
        auto n = new ArrayDef();
        n->setLeaf((yyvsp[0].exprtype));
        (yyval.stmttype) = (StmtNode*)n;
    }
#line 2452 "src/parser.cpp"
    break;

  case 81:
#line 818 "src/parser.y"
                           {
        (yyval.stmttype) = (yyvsp[-1].stmttype);
    }
#line 2460 "src/parser.cpp"
    break;

  case 82:
#line 821 "src/parser.y"
                    {
        // 新开辟一个leaf是nullptr的ArrayDef
        auto n = new ArrayDef();
        (yyval.stmttype) = (StmtNode*)n;
    }
#line 2470 "src/parser.cpp"
    break;

  case 83:
#line 831 "src/parser.y"
                  {
        SymbolEntry *se;
        se = identifiers->lookup((yyvsp[-1].strtype));
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)(yyvsp[-1].strtype));
            delete [](char*)(yyvsp[-1].strtype);
            assert(se != nullptr);
        }
        // 小于则为数组，大于等于则为单个变量（i[]会导致<右边为0）
        auto n = new TemporarySymbolEntry( ((DimArray*)(yyvsp[0].stmttype))->getDimList()->size() < ((ArrayType*)(se->getType()))->getDim() ? se->getType() : ((ArrayType*)(se->getType()))->getElementType() , SymbolTable::getLabel());
        // int temp = ((DimArray*)$2)->getDimList()->size() < ((ArrayType*)se)->getDim() ? se->getType() : ((ArrayType*)se)->getElementType();
        // std::cout<<n->getType()->isInt()<<std::endl;
        // std::cout<<((DimArray*)$2)->getDimList()->size()<<" "<<((ArrayType*)(se->getType()))->getDim()<<std::endl;
        // if(!se->getType())
        //     std::cout<<"null!"<<std::endl;
        (yyval.exprtype) = new ArrayIndex(n, se, (DimArray*)(yyvsp[0].stmttype));
        delete [](yyvsp[-1].strtype);
    }
#line 2494 "src/parser.cpp"
    break;

  case 84:
#line 882 "src/parser.y"
            {
        //p9函数返回类型检查
        if(!(yyvsp[-1].type)->isNumber() && (yyvsp[-1].type) != TypeSystem::voidType){
            std::cout<<"错误！函数返回类型非法！"<<std::endl;
        }
        Type *funcType;
        //设定返回类型
        funcType = new FunctionType((yyvsp[-1].type),{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, (yyvsp[0].strtype), identifiers->getLevel());
        identifiers->installFunc((yyvsp[0].strtype), se);
        //新建一层符号表
        identifiers = new SymbolTable(identifiers);
    }
#line 2512 "src/parser.cpp"
    break;

  case 85:
#line 896 "src/parser.y"
                     {
        SymbolEntry *se;
        if(tempParaType.empty()){
            tempParaType.push_back(TypeSystem::voidType);
        }
        //p7函数重复重载
        se = identifiers->lookup((yyvsp[-3].strtype), tempParaType);
        if(se){
            fprintf(stderr, "function identifier \"%s\" is already defined\n", (char*)(yyvsp[-3].strtype));
            // delete [](char*)$2;
        }
        se = identifiers->lookup((yyvsp[-3].strtype), {});
        assert(se != nullptr);
        ((FunctionType*)(se->getType()))->setParamsType(tempParaType);
    }
#line 2532 "src/parser.cpp"
    break;

  case 86:
#line 912 "src/parser.y"
    {
        SymbolEntry *se;

        //p7函数重复重载
        // if(identifiers->lookupcount($2, tempParaType) > 1){
        //     std::cout<<"错误！同一函数多次定义！"<<std::endl;
        //     // assert(identifiers->lookupcount($2, tempParaType) == 1);
        //     // panic();
        // }

        se = identifiers->lookup((yyvsp[-6].strtype), tempParaType);
        assert(se != nullptr);
        (yyval.stmttype) = new FunctionDef(se, (yyvsp[-3].stmttype), (yyvsp[0].stmttype));
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        // identifiers = identifiers->getPrev();

        std::vector<Type*>().swap(tempParaType); 
        delete top;
        delete [](yyvsp[-6].strtype);
    }
#line 2558 "src/parser.cpp"
    break;

  case 87:
#line 938 "src/parser.y"
                             {
        //查看符号表中是否存在
        SymbolEntry *se;
         if(vecArgType.back().empty()){
            vecArgType.back().push_back(TypeSystem::voidType);
        }
        se = identifiers->lookup((yyvsp[-3].strtype), vecArgType.back());
        if(se == nullptr)
        {
            fprintf(stderr, "function identifier \"%s\" is undefined\n", (char*)(yyvsp[-3].strtype));
            delete [](char*)(yyvsp[-3].strtype);
            assert(se != nullptr);
        }
        // 新建一个temp符号表项
        auto tempSe = new TemporarySymbolEntry(((FunctionType*)(se->getType()))->getReturnType(), SymbolTable::getLabel());
        // 新建语法树节点，符号表项、expr的向量、临时符号表项
        auto n = new FuncCall(tempSe, (IdentifierSymbolEntry *)se, vecArgList.back());
        (yyval.exprtype) = (ExprNode*)n;
        // 清空tempArgList和tempParaType
        // std::vector<Type*>().swap(tempArgType);
        // std::vector<ExprNode*>().swap(tempArgList);
        vecArgList.pop_back();
        vecArgType.pop_back();
    }
#line 2587 "src/parser.cpp"
    break;

  case 88:
#line 965 "src/parser.y"
                        {
        // std::cout<<"hello5"<<std::endl;
        //向节点中加入新的参数
        // tempArgList.push_back($3);
        // tempArgType.push_back($3->getSymbolEntry()->getType());
        //q15嵌套函数调用
        vecArgList.back().push_back((yyvsp[0].exprtype));
        vecArgType.back().push_back((yyvsp[0].exprtype)->getSymbolEntry()->getType());
    }
#line 2601 "src/parser.cpp"
    break;

  case 89:
#line 974 "src/parser.y"
          {
        // std::cout<<"hello4"<<std::endl;
        //新建语法树节点
        // tempArgList.push_back($1);
        // tempArgType.push_back($1->getSymbolEntry()->getType());
        //q15嵌套函数调用
        vecArgList.emplace_back(1, (yyvsp[0].exprtype));
        vecArgType.emplace_back(1, (yyvsp[0].exprtype)->getSymbolEntry()->getType());
    }
#line 2615 "src/parser.cpp"
    break;

  case 90:
#line 983 "src/parser.y"
             {
        //q15嵌套函数调用
        vecArgList.emplace_back();
        vecArgType.emplace_back();
    }
#line 2625 "src/parser.cpp"
    break;

  case 91:
#line 992 "src/parser.y"
                            {
        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = (FuncParam*)(yyvsp[-3].stmttype);
        se = new IdentifierSymbolEntry((yyvsp[-1].type), (yyvsp[0].strtype), identifiers->getLevel());
        identifiers->install((yyvsp[0].strtype), se);     
        n->addDecl(new Id(se));
        //改写参数类型临时向量
        tempParaType.push_back((yyvsp[-1].type));
        p->addNext(n);
        (yyval.stmttype) = (StmtNode*)p;
        delete [](yyvsp[0].strtype);
    }
#line 2644 "src/parser.cpp"
    break;

  case 92:
#line 1006 "src/parser.y"
            { 

        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = new FuncParam();
        se = new IdentifierSymbolEntry((yyvsp[-1].type), (yyvsp[0].strtype), identifiers->getLevel());
        identifiers->install((yyvsp[0].strtype), se);     
        n->addDecl(new Id(se));
        // 改写参数类型临时向量
        tempParaType.push_back((yyvsp[-1].type));
        p->addNext(n);
        (yyval.stmttype) = (StmtNode*)p;
        delete [](yyvsp[0].strtype);
    }
#line 2664 "src/parser.cpp"
    break;

  case 93:
#line 1023 "src/parser.y"
                             { 

        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = new FuncParam();
        if ((yyvsp[-3].type)->isInt()){
            tempParaType.push_back(TypeSystem::arrayIntType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayIntType, (yyvsp[-2].strtype), identifiers->getLevel());
        }
        else{
            tempParaType.push_back(TypeSystem::arrayFloatType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayFloatType, (yyvsp[-2].strtype), identifiers->getLevel());
        }
        identifiers->install((yyvsp[-2].strtype), se);     
        n->addDecl(new Id(se));
        p->addNext(n);
        (yyval.stmttype) = (StmtNode*)p;
        delete [](yyvsp[-2].strtype);
    }
#line 2689 "src/parser.cpp"
    break;

  case 94:
#line 1044 "src/parser.y"
                                      { 

        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = new FuncParam();
        if ((yyvsp[-4].type)->isInt()){
            tempParaType.push_back(TypeSystem::arrayIntType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayIntType, (yyvsp[-3].strtype), identifiers->getLevel());
        }
        else{
            tempParaType.push_back(TypeSystem::arrayFloatType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayFloatType, (yyvsp[-3].strtype), identifiers->getLevel());
        }
        identifiers->install((yyvsp[-3].strtype), se);     
        n->addDecl(new Id(se), nullptr, (DimArray*)(yyvsp[0].stmttype));
        p->addNext(n);
        (yyval.stmttype) = (StmtNode*)p;
        delete [](yyvsp[-3].strtype);
    }
#line 2714 "src/parser.cpp"
    break;

  case 95:
#line 1064 "src/parser.y"
                                             { 
        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = (FuncParam*)(yyvsp[-5].stmttype);
        if ((yyvsp[-3].type)->isInt()){
            tempParaType.push_back(TypeSystem::arrayIntType);
            // if(TypeSystem::arrayIntType != nullptr)
            //     std::cout<<"array param!"<<std::endl;
            se = new IdentifierSymbolEntry(TypeSystem::arrayIntType, (yyvsp[-2].strtype), identifiers->getLevel());
        }
        else{
            tempParaType.push_back(TypeSystem::arrayFloatType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayFloatType, (yyvsp[-2].strtype), identifiers->getLevel());
        }
        identifiers->install((yyvsp[-2].strtype), se);     
        n->addDecl(new Id(se));
        p->addNext(n);
        (yyval.stmttype) = (StmtNode*)p;
        delete [](yyvsp[-2].strtype);
    }
#line 2740 "src/parser.cpp"
    break;

  case 96:
#line 1085 "src/parser.y"
                                                      { 
        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = (FuncParam*)(yyvsp[-6].stmttype);
        // se = new IdentifierSymbolEntry($3, $4, identifiers->getLevel());
        if ((yyvsp[-4].type)->isInt()){
            tempParaType.push_back(TypeSystem::arrayIntType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayIntType, (yyvsp[-3].strtype), identifiers->getLevel());
        }
        else{
            tempParaType.push_back(TypeSystem::arrayFloatType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayFloatType, (yyvsp[-3].strtype), identifiers->getLevel());
        }
        identifiers->install((yyvsp[-3].strtype), se);     
        n->addDecl(new Id(se), nullptr, (DimArray*)(yyvsp[0].stmttype));
        p->addNext(n);
        (yyval.stmttype) = (StmtNode*)p;
        delete [](yyvsp[-3].strtype);
    }
#line 2765 "src/parser.cpp"
    break;

  case 97:
#line 1105 "src/parser.y"
            {
        (yyval.stmttype) = (StmtNode*)nullptr;
    }
#line 2773 "src/parser.cpp"
    break;


#line 2777 "src/parser.cpp"

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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

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
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
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
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1110 "src/parser.y"


int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
