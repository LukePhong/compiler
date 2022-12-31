#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include <string>
#include <map>

class Type;
class Operand;
#include <cassert>
#include <iomanip>

#include "Type.h"

// class Type;
// class IntType;
// class FloatType;

class SymbolEntry
{
private:
    int kind;
protected:
    //q2const常量支持
    //把enum放入public使能在类外访问
    // enum {CONSTANT, VARIABLE, TEMPORARY};
    Type *type;
public:
    enum EntryType{CONSTANT, VARIABLE, TEMPORARY};

public:
    SymbolEntry(Type *type, int kind);
    virtual ~SymbolEntry() {};
    bool isConstant() const {return kind == CONSTANT;};
    bool isTemporary() const {return kind == TEMPORARY;};
    bool isVariable() const {return kind == VARIABLE;};
    Type* getType() {return type;};
    void setType(Type *type) {this->type = type;};
    virtual std::string toStr() = 0;
    // You can add any function you need here.
};


/*  
    Symbol entry for literal constant. Example:

    int a = 1;

    Compiler should create constant symbol entry for literal constant '1'.
*/
class ConstantSymbolEntry : public SymbolEntry
{
private:
    int value_int;
    //q6浮点数支持
    float value_float;
    // bool IsInt;

public:
    ConstantSymbolEntry(Type *type, int value);
    ConstantSymbolEntry(Type *type, float value);
    virtual ~ConstantSymbolEntry() {};
    // void* getValue() const {return (void*)(isInt ? value_int : value_float);};
    bool isInt() { return type->isInt(); };
    int getValueInt() const;
    float getValueFloat() const;
    std::string toStr();
    // You can add any function you need here.
};


/* 
    Symbol entry for identifier. Example:

    int a;
    int b;
    void f(int c)
    {
        int d;
        {
            int e;
        }
    }

    Compiler should create identifier symbol entries for variables a, b, c, d and e:

    | variable | scope    |
    | a        | GLOBAL   |
    | b        | GLOBAL   |
    | c        | PARAM    |
    | d        | LOCAL    |
    | e        | LOCAL +1 |
*/
//就算是有DefStmt，符号表里面只要是有Identifier的那也不保存value
class IdentifierSymbolEntry : public SymbolEntry
{
private:
    enum {GLOBAL, PARAM, LOCAL};
    std::string name;
    int scope;
    Operand *addr;  // The address of the identifier.
    // You can add any field you need here.
    //q6在全局区添加系统函数声明和全局变量
    //用于全局变量固定值的获得
    ConstantSymbolEntry* glbConst = nullptr;
    //避免连续声明ID时因为插入临时符号表项导致类型检查时出现 重复定义 错误
    bool isTemp = false;

public:
    bool isGlobal() const {return scope == GLOBAL;};
    bool isParam() const {return scope == PARAM;};
    bool isLocal() const {return scope >= LOCAL;};
    void setAddr(Operand *addr) {this->addr = addr;};
    Operand* getAddr() {return addr;};
    //错了，我们语法分析时根本不知道这里是几维，因为里面可能是表达式等，无法在此时计算
    //默认是0维
    // std::vector<int> dimensions;
    bool getIsTemp() { return isTemp; }
    void setIsTemp(bool b){ isTemp = b; };

public:
    IdentifierSymbolEntry(Type *type, std::string name, int scope);
    IdentifierSymbolEntry(Type *type, std::string name, int scope, SymbolEntry::EntryType entryType);
    // IdentifierSymbolEntry(Type *type, std::string name, int scope, int value);
    virtual ~IdentifierSymbolEntry() {};
    std::string toStr();
    int getScope() const {return scope;};
    // You can add any function you need here.
    bool paramListMarch(std::vector<Type*> typeList);
    //q6在全局区添加系统函数声明和全局变量
    void outputGlbId();
    void outputSysFunc();
    void setGlbConst(SymbolEntry* se) { glbConst = new ConstantSymbolEntry(*(ConstantSymbolEntry*)se); }
};


/* 
    Symbol entry for temporary variable created by compiler. Example:

    int a;
    a = 1 + 2 + 3;

    The compiler would generate intermediate code like:

    t1 = 1 + 2
    t2 = t1 + 3
    a = t2

    So compiler should create temporary symbol entries for t1 and t2:

    | temporary variable | label |
    | t1                 | 1     |
    | t2                 | 2     |
*/
class TemporarySymbolEntry : public SymbolEntry
{
private:
    int stack_offset;
    int label;
public:
    TemporarySymbolEntry(Type *type, int label);
    virtual ~TemporarySymbolEntry() {};
    std::string toStr();
    int getLabel() const {return label;};
    void setOffset(int offset) { this->stack_offset = offset; };
    int getOffset() { return this->stack_offset; };
    // You can add any function you need here.
};

// symbol table managing identifier symbol entries
class SymbolTable
{
private:
    std::map<std::string, SymbolEntry*> symbolTable;
    //q12函数调用
    std::multimap<std::string, SymbolEntry*> funcTable;
    SymbolTable *prev;
    int level;
    static int counter;

public:
    SymbolTable();
    SymbolTable(SymbolTable *prev);
    void install(std::string name, SymbolEntry* entry);
    SymbolEntry* lookup(std::string name);
    //q12函数调用
    SymbolEntry* lookup(std::string name, std::vector<Type*> typeList);
    //p7函数重复重载
    int lookupcount(std::string name, std::vector<Type*> typeList);
    void installFunc(std::string name, SymbolEntry* entry);

    SymbolTable* getPrev() {return prev;};
    int getLevel() {return level;};
    static int getLabel() {return counter++;};
};

extern SymbolTable *identifiers;
extern SymbolTable *globals;

#endif
