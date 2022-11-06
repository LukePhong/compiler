#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>

class Type
{
private:
    int kind;
protected:
    enum {BOOL, INT, FLOAT, VOID, FUNC, ARRAY_INT, ARRAY_FLOAT};
public:
    Type(int kind) : kind(kind) {};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    bool isInt() const {return kind == INT;};
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
    bool isBool() const {return kind == BOOL;};
    //q6浮点数支持
    bool isFloat() const {return kind == FLOAT;};
    //输出kind
    int getKind() { return (int)kind; };
    //q11参数列表数组支持
    // bool isFloat() const {return kind == FLOAT;};
};

class IntType : public Type
{
private:
    int size;
public:
    IntType(int size) : Type(Type::INT), size(size){};
    std::string toStr();
};
class ArrayIntType : public Type
{
public:
    ArrayIntType() : Type(Type::ARRAY_INT){};
    std::string toStr();
};

//q6浮点数支持
class FloatType : public Type
{
private:
    int size;
public:
    FloatType(int size) : Type(Type::FLOAT), size(size){};
    std::string toStr();
};
class ArrayFloatType : public Type
{
public:
    ArrayFloatType() : Type(Type::ARRAY_FLOAT){};
    std::string toStr();
};

class BoolType : public Type
{
private:
    int size;
public:
    BoolType(int size) : Type(Type::BOOL), size(size){};
    std::string toStr();
};

class VoidType : public Type
{
public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
};

class FunctionType : public Type
{
private:
    Type *returnType;
    std::vector<Type*> paramsType;
public:
    FunctionType(Type* returnType, std::vector<Type*> paramsType) : 
        Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    std::string toStr();
    void setParamsType(std::vector<Type*> params) {paramsType = params;};
};

class TypeSystem
{
private:
    static IntType commonInt;
    static FloatType commonFloat;
    static VoidType commonVoid;
    static BoolType commonBool;
    static ArrayIntType commonArrayInt;
    static ArrayFloatType commonArrayFloat;
public:
    static Type *intType;
    static Type *floatType;
    static Type *voidType;
    static Type *boolType;
    static Type *arrayIntType;
    static Type *arrayFloatType;
};

#endif
