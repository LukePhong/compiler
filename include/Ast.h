#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include <vector>

// class SymbolEntry;
#include <SymbolTable.h>

class Node
{
private:
    static int counter;
    int seq;
public:
    Node();
    int getSeq() const {return seq;};
    virtual void output(int level) = 0;
};

class ExprNode : public Node
{
protected:
    SymbolEntry *symbolEntry;
public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){};
    //获得符号表项
    SymbolEntry *getSymbolEntry(){return symbolEntry;}
};
//q12函数调用
class FuncCall : public ExprNode{
private:
    IdentifierSymbolEntry *funcDef;
    std::vector<ExprNode*> arg;
public:
    FuncCall(SymbolEntry *symbolEntry, IdentifierSymbolEntry *def, std::vector<ExprNode*> arg) : ExprNode(symbolEntry), funcDef(def), arg(arg) {};
    void output(int level);
};

class BinaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {ADD, SUB, LOGIC_AND, LOGIC_OR, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL_TO, NOT_EQUAL_TO, PRODUCT, DIVISION, REMAINDER};
    BinaryExpr(SymbolEntry *se, int op, ExprNode*expr1, ExprNode*expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2){};
    void output(int level);
};

class UnaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr;
public:
    enum {SUB, LOGIC_NOT};
    UnaryExpr(SymbolEntry *se, int op, ExprNode*expr) : ExprNode(se), op(op), expr(expr){};
    void output(int level);
};



class Constant : public ExprNode
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};

class Id : public ExprNode
{
public:
    Id(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};


class StmtNode : public Node
{};

//q8数组支持
class DimArray : public StmtNode
{
private:
    std::vector<ExprNode*> dimList;
public:
    DimArray(){};
    void addDim(ExprNode* next);
    void output(int level);
    std::vector<ExprNode*>* getDimList() { return &dimList; };
};

//q9数组定义
class ArrayDef : public StmtNode
{
private:
    std::vector<ArrayDef*> arrDefList;
    //使用叶节点来保存Expr指针
    ExprNode* expr = nullptr;
public:
    ArrayDef(){};
    void addDef(ArrayDef* def);
    void setLeaf(ExprNode* e){ expr = e;};
    ExprNode* getLeaf() { return expr; };
    void output(int level);
};
//q14数组取值
class ArrayIndex : public ExprNode
{
private:
    SymbolEntry* arrDef;
    DimArray* dim;
public:
    //临时项、原始定义、维度
    ArrayIndex(SymbolEntry *se, SymbolEntry *arr, DimArray* dim) : ExprNode(se) , arrDef(arr), dim(dim){};
    void output(int level);
};


class EmptyStmt : public StmtNode
{
public:
    EmptyStmt(){};
    void output(int level);
};
class ExprStmt : public StmtNode
{
private:
    ExprNode* exp;
public:
    ExprStmt(ExprNode* exp) : exp(exp){};
    void output(int level);
};

class BreakStmt : public StmtNode
{
public:
    BreakStmt(){};
    void output(int level);
};

class ContinueStmt : public StmtNode
{
public:
    ContinueStmt(){};
    void output(int level);
};

class CompoundStmt : public StmtNode
{
private:
    StmtNode *stmt;
public:
    CompoundStmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
};

// class SeqNode : public StmtNode
// {
// private:
//     StmtNode *stmt1, *stmt2;
// public:
//     SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
//     void output(int level);
// };
//q1解决seq支持数目过少的问题
class SeqNode : public StmtNode
{
private:
    std::vector<StmtNode*> stmtList;
public:
    SeqNode(){};
    void addNext(StmtNode* next);
    void output(int level);
};

//q7支持连续定义/声明
class DeclStmt : public StmtNode
{
private:
    // Id *id;
    std::vector<Id *> idList;
    std::vector<ExprNode *> exprList;
    std::vector<DimArray *> dimArrayList;
    //q9数组定义
    std::vector<ArrayDef *> defArrList;
public:
    // DeclStmt(Id *id) : id(id){};
    DeclStmt(){};
    void addDecl(Id* next, ExprNode *exp = nullptr, DimArray *dim = nullptr, ArrayDef *defList = nullptr);
    void output(int level);
};
//q7支持连续定义/声明
// class DefStmt : public StmtNode
// {
// private:
//     Id *id;
//     ExprNode *expr;
// public:
//     DefStmt(Id *id, ExprNode *expr) : id(id), expr(expr){};
//     void output(int level);
// };

class IfStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
};

class IfElseStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
    StmtNode *elseStmt;
public:
    IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt) : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {};
    void output(int level);
};

//q5参照if语句实现while循环
class WhileStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *whileBodyStmt;
public:
    WhileStmt(ExprNode *cond, StmtNode *whileBodyStmt) : cond(cond), whileBodyStmt(whileBodyStmt){};
    void output(int level);
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
};

class AssignStmt : public StmtNode
{
private:
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
};

class FunctionDef : public StmtNode
{
private:
    SymbolEntry *se;
    //q10添加参数列表
    StmtNode *params = nullptr;
    StmtNode *stmt;
public:
    FunctionDef(SymbolEntry *se, StmtNode *params, StmtNode *stmt) : se(se), params(params), stmt(stmt){};
    void output(int level);
};
//q10添加参数列表
class FuncParam : public StmtNode
{
private:
    std::vector<StmtNode*> paramList;
public:
    FuncParam(){};
    void addNext(StmtNode* next);
    void output(int level);
};

class Ast
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
};

#endif
