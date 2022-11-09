%option noyywrap
%{
    /*
    * You will need to comment this line in lab5.
    */
    //#define ONLY_FOR_LEX
    
    #ifdef ONLY_FOR_LEX
     #else
    //#define YYSTYPE void *
    #include "parser.h"
    extern YYSTYPE yylval;
    #endif

    #define YY_NO_UNPUT
    #define YY_NO_INPUT
    #include <string>
    #include <stdlib.h>

    #ifdef ONLY_FOR_LEX
    #include <ostream>
    #include <fstream>
    using namespace std;
    extern FILE *yyin; 
    extern FILE *yyout;

    int offset;
    #define YY_USER_ACTION         \
    offset += yyleng;            \

    void DEBUG_FOR_LAB4(std::string s){
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }

    void DEBUG_FOR_LAB4(std::string s, int row, int col){
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\tline: " + std::to_string(row) + "\tcol: " + std::to_string(col) + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }
    #endif
%}


FLOAT ({DECIMIAL}\.[0-9]*|{DECIMIAL}\.[0-9]*[Ee](\+|-)?[0-9]+|{DECIMIAL}[Ee](\+|-)?[0-9]+)
HEXADECIMAL (0[xX][0-9a-fA-F]+)
OCTAL (0[0-7]+)
DECIMIAL ([1-9][0-9]*|0)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
COMMENT "//".*{EOL}
BLOCKCOMMENTBEGIN "/*"
BLOCKCOMMENTELEMENT .
BLOCKLINE {EOL}
BLOCKCOMMENTEND "*/"
%x BLOCKCOMMENT

%%

    //more terminal words
"break" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("BREAK\tbreak", yylineno, offset - yyleng);
     #else
        return BREAK;
    #endif 
}
"const" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("CONST\tconst", yylineno, offset - yyleng);
     #else
        return CONST;
    #endif 
}
"continue" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("CONTINUE\tcontinue", yylineno, offset - yyleng);
     #else
        return CONTINUE;
    #endif 
}
"for" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("FOR\tfor", yylineno, offset - yyleng);
     #else
        return FOR;
    #endif 
}
"while" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("WHILE\twhile", yylineno, offset - yyleng);
     #else
        return WHILE;
    #endif 
}

"int" {
    /*
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   Q2: What is "INT" actually?
    */
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("INT\tint", yylineno, offset - yyleng);
     #else
        return INT;
    #endif
}
"float" {
    //q6浮点数支持
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("FLOAT\tfloat", yylineno, offset - yyleng);
     #else
        return FLT;
    #endif
}
"void" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("VOID\tvoid", yylineno, offset - yyleng);
     #else
        return VOID;
    #endif 
}
"if" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("IF\tif", yylineno, offset - yyleng);
     #else
        return IF;
    #endif
};
"else" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ELSE\telse", yylineno, offset - yyleng);
     #else
        return ELSE;
    #endif
};
"return" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RETURN\treturn", yylineno, offset - yyleng);
     #else
        return RETURN;
    #endif
}

"!" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LOGIC_NOT\t!", yylineno, offset - yyleng);
     #else
        return LOGIC_NOT;
    #endif
}
"&&" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LOGIC_AND\t!", yylineno, offset - yyleng);
     #else
        return LOGIC_AND;
    #endif
}
"||" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LOGIC_OR\t!", yylineno, offset - yyleng);
     #else
        return LOGIC_OR;
    #endif
}

"*" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("PRODUCT\t*", yylineno, offset - yyleng);
     #else
        return PRODUCT;
    #endif
}
"/" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("DIVISION\t/", yylineno, offset - yyleng);
     #else
        return DIVISION;
    #endif
}
"%" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("REMAINDER\t%", yylineno, offset - yyleng);
     #else
        return REMAINDER;
    #endif
}
"~" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("NOT\t~", yylineno, offset - yyleng);
     #else
        return NOT;
    #endif
}
"&" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("AND\t&", yylineno, offset - yyleng);
     #else
        return AND;
    #endif
}
"|" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("OR\t|", yylineno, offset - yyleng);
     #else
        return OR;
    #endif
}
"^" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("XOR\t^", yylineno, offset - yyleng);
     #else
        return XOR;
    #endif
}

"+=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ADDITION ASSIGNMENT\t+=", yylineno, offset - yyleng);
     #else
        return ADDITION_ASSIGNMENT;
    #endif
}
"-=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("SUBTRACTION ASSIGNMENT\t-=", yylineno, offset - yyleng);
     #else
        return SUBTRACTION_ASSIGNMENT;
    #endif
}
"*=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("MULTIPLICATION ASSIGNMENT\t*=", yylineno, offset - yyleng);
     #else
        return MULTIPLICATION_ASSIGNMENT;
    #endif
}
"/=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("DIVISION ASSIGNMENT\t/=", yylineno, offset - yyleng);
     #else
        return DIVISION_ASSIGNMENT;
    #endif
}
"%=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("MODULO ASSIGNMENT\t%=", yylineno, offset - yyleng);
     #else
        return MODULO_ASSIGNMENT;
    #endif
}

"==" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("EQUAL TO\t==", yylineno, offset - yyleng);
     #else
        return EQUAL_TO;
    #endif
}
"!=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("NOT EQUAL TO\t!=", yylineno, offset - yyleng);
     #else
        return NOT_EQUAL_TO;
    #endif
}
"<=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LESS EQUAL\t<=", yylineno, offset - yyleng);
     #else
        return LESS_EQUAL;
    #endif
}
">=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("GREATER EQUAL\t>=", yylineno, offset - yyleng);
     #else
        return GREATER_EQUAL;
    #endif
}

"=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ASSIGN\t=", yylineno, offset - yyleng);
     #else
        return ASSIGN;
    #endif
}
"<" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LESS\t<", yylineno, offset - yyleng);
     #else
        return LESS;
    #endif
}
">" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("GREATER\t>", yylineno, offset - yyleng);
     #else
        return GREATER;
    #endif
}
"+" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ADD\t+", yylineno, offset - yyleng);
     #else
        return ADD;
    #endif
}
"-" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("SUB\t-", yylineno, offset - yyleng);
     #else
        return SUB;
    #endif
}
";" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("SEMICOLON\t;", yylineno, offset - yyleng);
     #else
        return SEMICOLON;
    #endif
}
"(" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LPAREN\t(", yylineno, offset - yyleng);
     #else
        return LPAREN;
    #endif
}
")" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RPAREN\t)", yylineno, offset - yyleng);
     #else
    return RPAREN;
    #endif
}
"[" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LSQUARE\t[", yylineno, offset - yyleng);
     #else
    return LSQUARE;
    #endif
}
"]" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RSQUARE\t]", yylineno, offset - yyleng);
     #else
    return RSQUARE;
    #endif
}
"{" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LBRACE\t{", yylineno, offset - yyleng);
     #else
        return LBRACE;
    #endif
}
"}" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RBRACE\t}", yylineno, offset - yyleng);
     #else
        return RBRACE;
    #endif
}

"," {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("COMMA\t,", yylineno, offset - yyleng);
     #else
        return COMMA;
    #endif
}

{ID}    {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ID\t"+std::string(yytext), yylineno, offset - yyleng);
     #else
        yylval.strtype = new char[2501];
        strcpy(yylval.strtype, yytext);
        return ID;
    #endif
}
{FLOAT} {
        #ifdef ONLY_FOR_LEX
            DEBUG_FOR_LAB4("FLOAT\t"+std::string(yytext), yylineno, offset - yyleng);
         #else
            sscanf(yytext, "%f", &yylval.ftype);
            //printf("%f\n", yylval.ftype);
            return FLOAT;
        #endif
    }
{DECIMIAL} {
        #ifdef ONLY_FOR_LEX
            DEBUG_FOR_LAB4("INTEGER\t"+std::string(yytext), yylineno, offset - yyleng);
         #else
            sscanf(yytext, "%d", &yylval.itype);
            return INTEGER;
        #endif
    }
{HEXADECIMAL} {
        #ifdef ONLY_FOR_LEX
            DEBUG_FOR_LAB4("INTEGER\t"+std::to_string(strtol(yytext,NULL,16)), yylineno, offset - yyleng);
         #else
            sscanf(std::to_string(strtol(yytext,NULL,16)).c_str(), "%d", &yylval.itype);
            return INTEGER;
        #endif
    }
{OCTAL} {
        #ifdef ONLY_FOR_LEX
            DEBUG_FOR_LAB4("INTEGER\t"+std::to_string(strtol(yytext,NULL,8)), yylineno, offset - yyleng);
         #else
            sscanf(std::to_string(strtol(yytext,NULL,8)).c_str(), "%d", &yylval.itype);
            return INTEGER;
        #endif
    }

{COMMENT} {yylineno++;}

{BLOCKCOMMENTBEGIN} {BEGIN BLOCKCOMMENT;}
<BLOCKCOMMENT>{BLOCKCOMMENTELEMENT} {}
<BLOCKCOMMENT>{BLOCKLINE}   {yylineno++;}
<BLOCKCOMMENT>{BLOCKCOMMENTEND}  {BEGIN INITIAL;}

{EOL} {
    yylineno++;
    #ifdef ONLY_FOR_LEX
        offset = 0;
    #endif
}
{WHITE}
%%

#ifdef ONLY_FOR_LEX
/* int main(int argc, char **argv){
    if(argc != 5){
        fprintf(stderr, "Argument Not Enough");
        exit(EXIT_FAILURE);
    }

    if(!(yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "No such file or directory: %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    if(!(yyout = fopen(argv[3], "w"))){
        fprintf(stderr, "No such file or directory: %s", argv[3]);
        exit(EXIT_FAILURE);
    }

    yylex();
    return 0;
} */
#endif
