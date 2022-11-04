%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    extern FILE *yyout;
    extern Ast ast;
    int yylex();
    int yyerror( char const * );
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    int itype;
    float ftype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token <ftype> FLOAT
%token IF ELSE
%token INT VOID
%token LPAREN RPAREN LSQUARE RSQUARE LBRACE RBRACE SEMICOLON
%token ADD SUB OR AND LESS GREATER ASSIGN
%token RETURN
%token BREAK CONTINUE
%token CONST
%token FOR WHILE
%token LOGIC_NOT LOGIC_AND LOGIC_OR
%token PRODUCT DIVISION REMAINDER
%token NOT XOR
%token ADDITION_ASSIGNMENT SUBTRACTION_ASSIGNMENT MULTIPLICATION_ASSIGNMENT DIVISION_ASSIGNMENT MODULO_ASSIGNMENT
%token EQUAL_TO NOT_EQUAL_TO LESS_EQUAL GREATER_EQUAL 
%token COMMA

%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt ReturnStmt DeclStmt FuncDef DefStmt BreakStmt ContinueStmt //FuncParam FuncParamList
%nterm <exprtype> Exp AddExp Cond LOrExp PrimaryExp LVal RelExp LAndExp UnaryExp MulExp
%nterm <type> Type

%precedence THEN
%precedence ELSE
%%
Program
    : Stmts {
        ast.setRoot($1);
    }
    ;

/* Stmts
    : Stmt {$$=$1;}
    | Stmts Stmt{
        $$ = new SeqNode($1, $2);
    }
    ; */
//q1解决seq支持数目过少的问题，此后应只有在出现新的scope之后才有seq
//SeqNode的含义也从每两个（stmt或者sqnode和stmt）就开一个，变成了同一个scope中所有语句的一个集合
Stmts
  : Stmt {
    SeqNode* node = new SeqNode();
    node->addNext((StmtNode*)$1);
    $$ = (StmtNode*) node;
  }
  | Stmts Stmt{
    SeqNode* node = (SeqNode*)$1;
    node->addNext((StmtNode*)$2);
    $$ = (StmtNode*) node;
  }
  ;

Stmt
    : AssignStmt {$$=$1;}
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    | FuncDef {$$=$1;}
    | DefStmt {$$=$1;}
    | SEMICOLON {$$ = new EmptyStmt();}
    | BreakStmt {$$ = $1;}
    | ContinueStmt {$$ = $1;}
    ;
LVal
    : ID {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
    ;
AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
    }
    ;
    
BlockStmt
    :   LBRACE 
        {identifiers = new SymbolTable(identifiers);} 
        Stmts RBRACE 
        {
            $$ = new CompoundStmt($3);
            SymbolTable *top = identifiers;
            identifiers = identifiers->getPrev();
            delete top;
        }
    |   LBRACE RBRACE{
            $$ = new CompoundStmt(nullptr);
        }
    ;

//q4if-else无大括号单声明/定义语句避错
//如果不这样那么刚读入if它不知道该执行哪个大括号，会冲突报错
If
    :
     /*建立新的符号表*/
    IF{
        identifiers = new SymbolTable(identifiers);
    }
    ;
IfStmt
    :
    If LPAREN Cond RPAREN Stmt %prec THEN {
        //一对{}及其内部整体要算一个
        $$ = new IfStmt($3, $5);
        //从符号表退出
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    |If LPAREN Cond RPAREN Stmt ELSE {
        //先退出一次
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        identifiers = new SymbolTable(identifiers);
    } Stmt {
        $$ = new IfElseStmt($3, $5, $8);
        //从符号表退出
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top; 
    }
    ;
/* IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfElseStmt($3, $5, $7);
    }
    ; */


//for循环参照函数形式实现

//break语句
BreakStmt
    :
    BREAK SEMICOLON {
        $$ = new BreakStmt();
    }
    ;
//continue语句
ContinueStmt
    :
    CONTINUE SEMICOLON {
        $$ = new ContinueStmt();
    }
    ;
//return语句
ReturnStmt
    :
    RETURN Exp SEMICOLON {
        $$ = new ReturnStmt($2);
    }
    |
    RETURN SEMICOLON{
        $$ = new ReturnStmt(nullptr);
    }
    ;
/* ReturnStmt
    :
    RETURN Exp SEMICOLON{
        $$ = new ReturnStmt($2);
    }
    ; */


/* Exp
    :
    AddExp {$$ = $1;}
    ;
Cond
    :
    LOrExp {$$ = $1;}
    ;

PrimaryExp
    :
    LVal {
        $$ = $1;
    }
    | INTEGER {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    ;
AddExp
    :
    PrimaryExp {$$ = $1;}
    |
    AddExp ADD PrimaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    |
    AddExp SUB PrimaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    ;


RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    ;
LAndExp
    :
    RelExp {$$ = $1;}
    |
    LAndExp AND RelExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;
LOrExp
    :
    LAndExp {$$ = $1;}
    |
    LOrExp OR LAndExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ; */
Exp
    :
    LOrExp {$$ = $1;}
    ;

//逻辑或运算
LOrExp
    :
    LAndExp {$$ = $1;}
    |
    LOrExp LOGIC_OR LAndExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LOGIC_OR, $1, $3);
    }
    ;
//逻辑与运算
LAndExp
    :
    RelExp {$$ = $1;}
    |
    LAndExp LOGIC_AND RelExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LOGIC_AND, $1, $3);
    }
    ;
//关系运算
RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    |
    RelExp GREATER AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER, $1, $3);
    }
    |
    RelExp LESS_EQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS_EQUAL, $1, $3);
    }
    |
    RelExp GREATER_EQUAL AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER_EQUAL, $1, $3);
    }
    |
    RelExp EQUAL_TO AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL_TO, $1, $3);
    }
    |
    RelExp NOT_EQUAL_TO AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOT_EQUAL_TO, $1, $3);
    }
    ;
//if括号内
Cond
    :
    LOrExp {$$ = $1;}
    ;

//加法表达式
AddExp
    :
    MulExp {$$ = $1;}
    |
    AddExp ADD MulExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    |
    AddExp SUB MulExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    ;
//乘法表达式
MulExp
    :
    UnaryExp {$$ = $1;}
    |
    MulExp PRODUCT UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::PRODUCT, $1, $3);
    }
    |
    MulExp DIVISION UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIVISION, $1, $3);
    }
    |
    MulExp REMAINDER UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::REMAINDER, $1, $3);
    }
    ;
//单目运算表达式
UnaryExp
    :
    PrimaryExp {$$ = $1;}
    |
    ADD UnaryExp {$$ = $2;}
    |
    SUB UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::SUB, $2);
    }
    |
    LOGIC_NOT UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::LOGIC_NOT, $2);
    }
    ;
//原始表达式
PrimaryExp
    :
    LVal {
        $$ = $1;
    }
    | INTEGER {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    | LPAREN Exp RPAREN {
        $$ = $2;
    }
    ;



Type
    //这里使得符号表项中的的Type指针能指Type类型的值
    : INT {
        $$ = TypeSystem::intType;
    }
    | VOID {
        $$ = TypeSystem::voidType;
    }
    ;


DeclStmt
    :
    //这里不能替换成Decl
    Type ID SEMICOLON {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);
        
        // 输出符号表项地址
        // fprintf(yyout, "%s-%x%x\n", $2, ((unsigned int*)&se)[1], ((unsigned int*)&se)[0]);

        $$ = new DeclStmt(new Id(se));
        delete []$2;
    }
    |
    //q2const常量支持
    CONST Type ID SEMICOLON {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($2, $3, identifiers->getLevel(), SymbolEntry::CONSTANT);
        identifiers->install($3, se);
        $$ = new DeclStmt(new Id(se));
        delete []$3;
    }
    ;

//q3添加DefStmt变量常量定义语句
DefStmt
    :
    Type ID ASSIGN Exp SEMICOLON {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);

        $$ = new DefStmt(new Id(se), $4);
        //符号表中保存了，原来开辟的空间就可以释放了
        delete []$2;
    }
    |
    CONST Type ID ASSIGN Exp SEMICOLON {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry($2, $3, identifiers->getLevel(), SymbolEntry::CONSTANT);
        identifiers->install($3, se);
        $$ = new DefStmt(new Id(se), $5);
        delete []$3;
    }
    ;

FuncDef
    :
    Type ID {
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);

        // 输出符号表项地址
        // fprintf(yyout, "%s-%x%x\n", $2, ((unsigned int*)&se)[1], ((unsigned int*)&se)[0]);

        identifiers = new SymbolTable(identifiers);
    }
    LPAREN RPAREN
    BlockStmt
    {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, $6);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ;
%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
