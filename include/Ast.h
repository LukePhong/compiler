#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include "Operand.h"

class SymbolEntry;
class Unit;
class Function;
class BasicBlock;
class Instruction;
class IRBuilder;
#include <vector>
#include <iostream>

// class SymbolEntry;
#include <SymbolTable.h>


class Node
{
private:
    static int counter;
    int seq;
protected:
    std::vector<Instruction*> true_list;
    std::vector<Instruction*> false_list;
    static IRBuilder *builder;
    void backPatch(std::vector<Instruction*> &list, BasicBlock*bb, bool setTrueBr = true);
    std::vector<Instruction*> merge(std::vector<Instruction*> &list1, std::vector<Instruction*> &list2);

    Operand *typeConvention(Type* target, Operand * toConvert, BasicBlock*bb);
    void typeConsist(Operand** op1, Operand** op2, BasicBlock*bb, bool wider = false);

public:
    Node();
    int getSeq() const {return seq;};
    static void setIRBuilder(IRBuilder*ib) {builder = ib;};
    virtual void output(int level) = 0;
    virtual void typeCheck() = 0;
    virtual void genCode() = 0;
    std::vector<Instruction*>& trueList() {return true_list;}
    std::vector<Instruction*>& falseList() {return false_list;}
};

class ExprNode : public Node
{
protected:
    SymbolEntry *symbolEntry;
    Operand *dst;   // The result of the subtree is stored into dst.    综合属性

    bool isArrEle = false;
public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){};
    Operand* getOperand() {return dst;};
    SymbolEntry* getSymPtr() {return symbolEntry;};
public:
    //获得符号表项
    SymbolEntry *getSymbolEntry(){return symbolEntry;}
    bool isInArr() {return isArrEle;}
};

//q12函数调用
class FuncCall : public ExprNode{
private:
    IdentifierSymbolEntry *funcDef;
    std::vector<ExprNode*> arg;
public:
    //q5FunctionCall的代码生成
    FuncCall(SymbolEntry *symbolEntry, IdentifierSymbolEntry *def, std::vector<ExprNode*> arg) : ExprNode(symbolEntry), funcDef(def), arg(arg) {dst = new Operand(symbolEntry);};
    void output(int level);
    void typeCheck();
    void genCode();
};

class BinaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    BinaryExpr(SymbolEntry *se, int op, ExprNode*expr1, ExprNode*expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2)
        {SymbolEntry *temp = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel()); dst = new Operand(temp);};
    void output(int level);
    void typeCheck();
    void genCode();
    enum {ADD, SUB, PRODUCT, DIVISION, REMAINDER, LOGIC_AND, LOGIC_OR, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, EQUAL_TO, NOT_EQUAL_TO};
};

class UnaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr;
public:
    enum {SUB, LOGIC_NOT};
    UnaryExpr(SymbolEntry *se, int op, ExprNode*expr) : ExprNode(se), op(op), expr(expr)
        {SymbolEntry *temp = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel()); dst = new Operand(temp);};
    void output(int level);
    void typeCheck();
    void genCode();
};

class Constant : public ExprNode
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){dst = new Operand(se);};
    void output(int level);
    void typeCheck();
    void genCode();
};

class Id : public ExprNode
{
public:
    Id(SymbolEntry *se) : ExprNode(se){SymbolEntry *temp = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel()); dst = new Operand(temp);};
    void output(int level);
    void typeCheck();
    void genCode();
};

class StmtNode : public Node
{};

//q8数组支持
class DimArray : public StmtNode
{
private:
    std::vector<ExprNode*> dimList;
    Operand *dst;
public:
    DimArray(){};
    void addDim(ExprNode* next);
    void output(int level);
    std::vector<ExprNode*>* getDimList() { return &dimList; };
    void typeCheck();
    void genCode();
    void setDst(Operand *d) { dst = d; }
    Operand *getDst() { return dst; }
};

//q9数组定义
class ArrayDef : public StmtNode
{
private:
    std::vector<ArrayDef*> arrDefList;
    //使用叶节点来保存Expr指针
    ExprNode* expr = nullptr;
    std::vector<Operand*> arrDstVec;
public:
    ArrayDef(){};
    void addDef(ArrayDef* def);
    void setLeaf(ExprNode* e){ expr = e;};
    ExprNode* getLeaf() { return expr; };
    std::vector<ArrayDef*>& getDefList() { return arrDefList; }
    void output(int level);
    void typeCheck();
    void genCode();
    bool isAllDefined(int& cnt);
    // void getArrayDefStr(int idx);
    void addArrDst(Operand* d) { arrDstVec.push_back(d); }
    std::vector<Operand*>& getArrDst() { return arrDstVec; }
    void getArrayDefCode(int idx, Operand* defOp, Type* toTrim, bool checkTop = false);
};

//q14数组取值
class ArrayIndex : public ExprNode
{
private:
    SymbolEntry* arrDef;
    DimArray* dim;
public:
    //临时项、原始定义、维度
    ArrayIndex(SymbolEntry *se, SymbolEntry *arr, DimArray* dim) : ExprNode(se) , arrDef(arr), dim(dim)
        {   SymbolEntry *temp = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel()); dst = new Operand(temp);
            isArrEle = true;};
    void output(int level);
    void typeCheck();
    void genCode();
    void genLvalCode();
};


class EmptyStmt : public StmtNode
{
public:
    EmptyStmt(){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ExprStmt : public StmtNode
{
private:
    ExprNode* exp;
public:
    ExprStmt(ExprNode* exp) : exp(exp){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class BreakStmt : public StmtNode
{
public:
    BreakStmt(){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ContinueStmt : public StmtNode
{
public:
    ContinueStmt(){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class CompoundStmt : public StmtNode
{
private:
    StmtNode *stmt;
public:
    CompoundStmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
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
    void typeCheck();
    void genCode();
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
    void typeCheck();
    void genCode();
    auto& getIdList() { return idList; }
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
    void typeCheck();
    void genCode();
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
    void typeCheck();
    void genCode();
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
    void typeCheck();
    void genCode();
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;
    //q4为function加入exit块
    Operand *dst;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
    Operand* getOperand() {return dst;};
    void typeCheck();
    void genCode();
};

class AssignStmt : public StmtNode
{
private:
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
    void typeCheck();
    void genCode();
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
    void typeCheck();
    void genCode();
    auto& getParamList() { return paramList; }
};

class FunctionDef : public StmtNode
{
private:
    SymbolEntry *se;
    //q10添加参数列表
    StmtNode *params = nullptr;
    StmtNode *stmt;

    //q4为function加入exit块
    Operand* retAddr;
public:
    FunctionDef(SymbolEntry *se, StmtNode *params, StmtNode *stmt) : se(se), params(params), stmt(stmt)
        { 
            if(((FunctionType*)se->getType())->getReturnType() != TypeSystem::voidType){
                Type *type;
                type = new PointerType(((FunctionType*)se->getType())->getReturnType());
                SymbolEntry *temp = new TemporarySymbolEntry(type, SymbolTable::getLabel()); 
                retAddr = new Operand(temp);
            }else{
                retAddr = nullptr;
            } 
        };
    auto getRetAddr() { return retAddr; };
    void output(int level);
    void typeCheck();
    void genCode();
    auto& getParamList() { return ((FuncParam*)params)->getParamList(); }
};

class Ast
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
    void typeCheck();
    void genCode(Unit *unit);
};

#endif
