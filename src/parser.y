%code top{
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
%token INT VOID FLT
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

%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt ReturnStmt DeclStmt FuncDef BreakStmt ContinueStmt 
                    WhileStmt DimArray ArrayDefBlock ArrayDef FuncParam //ArrayParam//FuncParamList DefStmt
%nterm <exprtype> Exp AddExp Cond LOrExp PrimaryExp LVal RelExp LAndExp UnaryExp MulExp FuncCall ArrayIndex
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
    /* | DefStmt {$$=$1;} */
    // 单分号（空语句）
    | SEMICOLON {$$ = new EmptyStmt();}
    | Exp SEMICOLON { $$; }
    | BreakStmt {$$ = $1;}
    | ContinueStmt {$$ = $1;}
    | WhileStmt {$$ = $1;}
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
    |ArrayIndex {
        $$ = $1;
    }
    ;
AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON {
        // if($3->getSymbolEntry()->getType()->isFloat())
            // std::cout<<((ConstantSymbolEntry*)$3->getSymbolEntry())->getValueFloat()<<std::endl;
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

//q5参照if语句实现while循环
While
    :
     /*建立新的符号表*/
    WHILE{
        identifiers = new SymbolTable(identifiers);
    }
    ;
WhileStmt
    :
    While LPAREN Cond RPAREN Stmt {
        //一对{}及其内部整体要算一个
        $$ = new WhileStmt($3, $5);
        //从符号表退出
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
    }
    ;


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
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
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
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
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
         SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    |
    RelExp GREATER AddExp
    {
         SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER, $1, $3);
    }
    |
    RelExp LESS_EQUAL AddExp
    {
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS_EQUAL, $1, $3);
    }
    |
    RelExp GREATER_EQUAL AddExp
    {
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GREATER_EQUAL, $1, $3);
    }
    |
    RelExp EQUAL_TO AddExp
    {
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQUAL_TO, $1, $3);
    }
    |
    RelExp NOT_EQUAL_TO AddExp
    {
        SymbolEntry *se;
        //if($1->getSymbolEntry()->getType()->isInt())
        //    se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        //else
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
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
        SymbolEntry *se;
        // if($1->getSymbolEntry()->getType()->isInt() && $3->getSymbolEntry()->getType()->isInt())
        //     se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        // else
        //     se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
        int k1 = $1->getSymbolEntry()->getType()->getKind(),k2 = $3->getSymbolEntry()->getType()->getKind();
        se = new TemporarySymbolEntry( k1 >= k2 ? $1->getSymbolEntry()->getType() : $3->getSymbolEntry()->getType() , SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    |
    AddExp SUB MulExp
    {
        SymbolEntry *se;
        int k1 = $1->getSymbolEntry()->getType()->getKind(),k2 = $3->getSymbolEntry()->getType()->getKind();
        se = new TemporarySymbolEntry( k1 >= k2 ? $1->getSymbolEntry()->getType() : $3->getSymbolEntry()->getType() , SymbolTable::getLabel());
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
        SymbolEntry *se;
        int k1 = $1->getSymbolEntry()->getType()->getKind(),k2 = $3->getSymbolEntry()->getType()->getKind();
        se = new TemporarySymbolEntry( k1 >= k2 ? $1->getSymbolEntry()->getType() : $3->getSymbolEntry()->getType() , SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::PRODUCT, $1, $3);
    }
    |
    MulExp DIVISION UnaryExp
    {
        SymbolEntry *se;
        int k1 = $1->getSymbolEntry()->getType()->getKind(),k2 = $3->getSymbolEntry()->getType()->getKind();
        se = new TemporarySymbolEntry( k1 >= k2 ? $1->getSymbolEntry()->getType() : $3->getSymbolEntry()->getType() , SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIVISION, $1, $3);
    }
    |
    MulExp REMAINDER UnaryExp
    {
        SymbolEntry *se;
        int k1 = $1->getSymbolEntry()->getType()->getKind(),k2 = $3->getSymbolEntry()->getType()->getKind();
        se = new TemporarySymbolEntry( k1 >= k2 ? $1->getSymbolEntry()->getType() : $3->getSymbolEntry()->getType() , SymbolTable::getLabel());
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
        //q6浮点数支持
        SymbolEntry *se;
        if($2->getSymbolEntry()->getType()->isInt())
            se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        else
            se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::SUB, $2);
    }
    |
    LOGIC_NOT UnaryExp
    {
        //q6浮点数支持
        // SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        SymbolEntry *se;
        //if($2->getSymbolEntry()->getType()->isInt())
        se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        //else
            //se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
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
    | FLOAT {
        //q6浮点数支持
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::floatType, $1);
        // std::cout<<$1<<std::endl;
        $$ = new Constant(se);
    }
    | LPAREN Exp RPAREN {
        $$ = $2;
    }
    | FuncCall {
        $$ = $1;
    }
    ;

Type
    //这里使得符号表项中的的Type指针能指Type类型的值
    : INT {
        $$ = TypeSystem::intType;
        upperType = TypeSystem::intType;
    }
    | VOID {
        $$ = TypeSystem::voidType;
    }
    | FLT {
        //q6浮点数支持
        $$ = TypeSystem::floatType;
        upperType = TypeSystem::floatType;
    }
    ;


/* DeclStmt
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
    ; */
//q7支持连续定义/声明
DeclStmt
    :
    Type 
    /* {
        upperType = $1;
    }  */
    DeclBody SEMICOLON{
        SymbolEntry *se;
        auto n = new DeclStmt();
        for(auto i:tempDecl){
            if (i.dim){
                if($1->isInt()){
                    // std::cout<<"hello1"<<std::endl;
                    auto t = new ArrayIntType(*(((DimArray*)i.dim)->getDimList()), $1);
                    se = new IdentifierSymbolEntry(t, i.name, i.level);
                }
                else{
                    // std::cout<<"hello2"<<std::endl;
                    auto t = new ArrayFloatType(*(((DimArray*)i.dim)->getDimList()), $1);
                    se = new IdentifierSymbolEntry(t, i.name, i.level);
                }
            }
            else{
                // std::cout<<"hello3"<<std::endl;
                se = new IdentifierSymbolEntry($1, i.name, i.level);
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
        $$ = (StmtNode*)n;
        std::vector<tempDeclArray>().swap(tempDecl);
    }
    //q2const常量支持
    |CONST Type 
    /* {
        upperType = $2;
    }  */
    DeclBody SEMICOLON{
        SymbolEntry *se;
        auto n = new DeclStmt();
        for(auto i:tempDecl){
            if (i.dim){
                if($2->isInt()){
                    auto t = new ArrayIntType(*(((DimArray*)i.dim)->getDimList()), $2);
                    se = new IdentifierSymbolEntry(t, i.name, i.level);
                }
                else{
                    auto t = new ArrayFloatType(*(((DimArray*)i.dim)->getDimList()), $2);
                    se = new IdentifierSymbolEntry(t, i.name, i.level);
                }
            }
            else
                se = new IdentifierSymbolEntry($2, i.name, i.level);
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
        $$ = (StmtNode*)n;
        std::vector<tempDeclArray>().swap(tempDecl);
    }
    ;
DeclBody
    :
    //没有赋值语句
    DeclBody COMMA ID{
        //q18避免定义时出现未声明变量报错
        auto tempEntry = new IdentifierSymbolEntry(upperType, $3, identifiers->getLevel());
        identifiers->install($3, tempEntry); 

        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), $3});
        delete []$3;
    }
    |ID{
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), $1});
        delete []$1;
    }
    |
    //q8数组支持
    DeclBody COMMA ID DimArray{
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), $3, false, nullptr, $4});
        delete []$3;
    }
    |ID DimArray{
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), $1, false, nullptr, $2});
        delete []$1;
    }
    //有赋值语句
    |DeclBody COMMA ID ASSIGN Exp{
        //q18避免定义时出现未声明变量报错
        auto tempEntry = new IdentifierSymbolEntry(upperType, $3, identifiers->getLevel());
        identifiers->install($3, tempEntry); 

        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), $3, true, $5});
        delete []$3;
    }
    |ID ASSIGN Exp{
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), $1, true, $3});
        delete []$1;
    }
    |DeclBody COMMA ID DimArray ASSIGN ArrayDefBlock{
        assert($6 != nullptr);
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), $3, true, nullptr, $4, $6});
        delete []$3;
    }
    |ID DimArray ASSIGN ArrayDefBlock{
        assert($4 != nullptr);
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), $1, true, nullptr, $2, $4});
        delete []$1;
    }
    ;
DimArray
    :DimArray LSQUARE Exp RSQUARE{
        auto dimArr = (DimArray*)$1;
        dimArr->addDim((ExprNode*)$3);
        $$ = (StmtNode*) dimArr;
    }
    |LSQUARE Exp RSQUARE{
        auto dim = new DimArray();
        dim->addDim((ExprNode*)$2);
        $$ = (StmtNode*) dim;
    }
    ;
//q9数组定义
ArrayDef
    : ArrayDef COMMA ArrayDefBlock{
        // 向defArray中push
        auto a = (ArrayDef *)$1;
        a->addDef((ArrayDef *)$3);
        $$ = (StmtNode*)a;
    }
    | ArrayDefBlock{
        auto n = new ArrayDef();
        n->addDef((ArrayDef *)$1);
        $$ = (StmtNode*)n;
    }
    ;
ArrayDefBlock
    :Exp    {
        // 新开辟一个只有leaf的ArrayDef
        auto n = new ArrayDef();
        n->setLeaf($1);
        $$ = (StmtNode*)n;
    }
    |LBRACE ArrayDef RBRACE{
        $$ = $2;
    }
    |LBRACE RBRACE  {
        // 新开辟一个leaf是nullptr的ArrayDef
        auto n = new ArrayDef();
        $$ = (StmtNode*)n;
    }
    ;


//q14数组取值
ArrayIndex
    : ID DimArray {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        // 小于则为数组，大于等于则为单个变量（i[]会导致<右边为0）
        auto n = new TemporarySymbolEntry( ((DimArray*)$2)->getDimList()->size() < ((ArrayType*)(se->getType()))->getDim() ? se->getType() : ((ArrayType*)(se->getType()))->getElementType() , SymbolTable::getLabel());
        // int temp = ((DimArray*)$2)->getDimList()->size() < ((ArrayType*)se)->getDim() ? se->getType() : ((ArrayType*)se)->getElementType();
        // std::cout<<n->getType()->isInt()<<std::endl;
        // std::cout<<((DimArray*)$2)->getDimList()->size()<<" "<<((ArrayType*)(se->getType()))->getDim()<<std::endl;
        // if(!se->getType())
        //     std::cout<<"null!"<<std::endl;
        $$ = new ArrayIndex(n, se, (DimArray*)$2);
        delete []$1;
    }
    ;

/* DeclArray
    : ID LSQUARE Exp RBRACE{
        tempDecl.emplace_back(tempDeclArray{identifiers->getLevel(), $1, true, $3, {$3->}});
        delete []$1;
    }
    ; */

//q3添加DefStmt变量常量定义语句
/* DefStmt
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
    ; */

FuncDef
    :
    Type ID {
        Type *funcType;
        //设定返回类型
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->installFunc($2, se);
        //新建一层符号表
        identifiers = new SymbolTable(identifiers);
    }
    //q10添加参数列表
    LPAREN FuncParam {
        SymbolEntry *se;
        se = identifiers->lookup($2, {});
        assert(se != nullptr);
        // cout<<tempParaType.size()<<endl;
        ((FunctionType*)(se->getType()))->setParamsType(tempParaType);
    } RPAREN
    BlockStmt
    {
        SymbolEntry *se;
        se = identifiers->lookup($2, tempParaType);
        assert(se != nullptr);
        $$ = new FunctionDef(se, $5, $8);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        std::vector<Type*>().swap(tempParaType); 
        delete top;
        delete []$2;
    }
    ;

//q12函数调用
FuncCall
    :
    ID LPAREN FuncArg RPAREN {
        //查看符号表中是否存在
        SymbolEntry *se;
        se = identifiers->lookup($1, vecArgType.back());
        assert(se != nullptr);
        // 新建一个temp符号表项
        auto tempSe = new TemporarySymbolEntry(((FunctionType*)(se->getType()))->getReturnType(), SymbolTable::getLabel());
        // 新建语法树节点，符号表项、expr的向量、临时符号表项
        auto n = new FuncCall(tempSe, (IdentifierSymbolEntry *)se, vecArgList.back());
        $$ = (ExprNode*)n;
        // 清空tempArgList和tempParaType
        // std::vector<Type*>().swap(tempArgType);
        // std::vector<ExprNode*>().swap(tempArgList);
        vecArgList.pop_back();
        vecArgType.pop_back();
    }
    ;

FuncArg
    : FuncArg COMMA Exp {
        // std::cout<<"hello5"<<std::endl;
        //向节点中加入新的参数
        // tempArgList.push_back($3);
        // tempArgType.push_back($3->getSymbolEntry()->getType());
        //q15嵌套函数调用
        vecArgList.back().push_back($3);
        vecArgType.back().push_back($3->getSymbolEntry()->getType());
    }
    | Exp {
        // std::cout<<"hello4"<<std::endl;
        //新建语法树节点
        // tempArgList.push_back($1);
        // tempArgType.push_back($1->getSymbolEntry()->getType());
        //q15嵌套函数调用
        vecArgList.emplace_back(1, $1);
        vecArgType.emplace_back(1, $1->getSymbolEntry()->getType());
    }
    | %empty {
        //q15嵌套函数调用
        vecArgList.emplace_back();
        vecArgType.emplace_back();
    }
    ;

//q10添加参数列表
FuncParam
    :FuncParam COMMA Type ID{
        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = (FuncParam*)$1;
        se = new IdentifierSymbolEntry($3, $4, identifiers->getLevel());
        identifiers->install($4, se);     
        n->addDecl(new Id(se));
        //改写参数类型临时向量
        tempParaType.push_back($3);
        p->addNext(n);
        $$ = (StmtNode*)p;
        delete []$4;
    }
    |Type ID{ 
        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = new FuncParam();
        se = new IdentifierSymbolEntry($1, $2, identifiers->getLevel());
        identifiers->install($2, se);     
        n->addDecl(new Id(se));
        // 改写参数类型临时向量
        tempParaType.push_back($1);
        p->addNext(n);
        $$ = (StmtNode*)p;
        delete []$2;
    }
    //q11参数列表数组支持
    //一维
    |Type ID LSQUARE RSQUARE { 
        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = new FuncParam();
        if ($1->isInt()){
            tempParaType.push_back(TypeSystem::arrayIntType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayIntType, $2, identifiers->getLevel());
        }
        else{
            tempParaType.push_back(TypeSystem::arrayFloatType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayFloatType, $2, identifiers->getLevel());
        }
        identifiers->install($2, se);     
        n->addDecl(new Id(se));
        p->addNext(n);
        $$ = (StmtNode*)p;
        delete []$2;
    }
    //多维
    |Type ID LSQUARE RSQUARE DimArray { 
        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = new FuncParam();
        if ($1->isInt()){
            tempParaType.push_back(TypeSystem::arrayIntType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayIntType, $2, identifiers->getLevel());
        }
        else{
            tempParaType.push_back(TypeSystem::arrayFloatType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayFloatType, $2, identifiers->getLevel());
        }
        identifiers->install($2, se);     
        n->addDecl(new Id(se), nullptr, (DimArray*)$5);
        p->addNext(n);
        $$ = (StmtNode*)p;
        delete []$2;
    }
    |FuncParam COMMA Type ID LSQUARE RSQUARE { 
        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = (FuncParam*)$1;
        if ($3->isInt()){
            tempParaType.push_back(TypeSystem::arrayIntType);
            // if(TypeSystem::arrayIntType != nullptr)
            //     std::cout<<"array param!"<<std::endl;
            se = new IdentifierSymbolEntry(TypeSystem::arrayIntType, $4, identifiers->getLevel());
        }
        else{
            tempParaType.push_back(TypeSystem::arrayFloatType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayFloatType, $4, identifiers->getLevel());
        }
        identifiers->install($4, se);     
        n->addDecl(new Id(se));
        p->addNext(n);
        $$ = (StmtNode*)p;
        delete []$4;
    }
    |FuncParam COMMA Type ID LSQUARE RSQUARE DimArray { 
        //新增符号表项
        SymbolEntry *se;
        auto n = new DeclStmt();
        auto p = (FuncParam*)$1;
        // se = new IdentifierSymbolEntry($3, $4, identifiers->getLevel());
        if ($3->isInt()){
            tempParaType.push_back(TypeSystem::arrayIntType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayIntType, $4, identifiers->getLevel());
        }
        else{
            tempParaType.push_back(TypeSystem::arrayFloatType);
            se = new IdentifierSymbolEntry(TypeSystem::arrayFloatType, $4, identifiers->getLevel());
        }
        identifiers->install($4, se);     
        n->addDecl(new Id(se), nullptr, (DimArray*)$7);
        p->addNext(n);
        $$ = (StmtNode*)p;
        delete []$4;
    }
    |%empty {
        $$ = (StmtNode*)nullptr;
    }
    ;

%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
