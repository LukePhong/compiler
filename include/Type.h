#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>

class ExprNode;

class TypeSystem;

class Type
{
private:
    int kind;
protected:
    enum typeKind{BOOL, INT, FLOAT, VOID, FUNC, ARRAY_INT, ARRAY_FLOAT, PTR};
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
    //q17参数列表模糊匹配
    //判断是否是数字类型
    bool isNumber() { return kind < 3; };
    //p4二元运算类型检查
    bool isArrayType() { return kind > FUNC; }
};

class IntType : public Type
{
private:
    int size;
public:
    IntType(int size) : Type(Type::INT), size(size){};
    std::string toStr();
};

class PointerType : public Type
{
private:
    Type *valueType;
public:
    PointerType(Type* valueType) : Type(Type::PTR) {this->valueType = valueType;};
    std::string toStr();
};

class ArrayType : public Type{
private:
    std::vector<ExprNode*> dimList;
    Type *elementType;
    int cntEleNum;
    std::vector<std::string> dimTypeStrArray;
    Type* trimedType = nullptr;
public:
    //如果这里没有eleType的话，使用arrayIntType定义的数组再访问时如果访问到eleType将会是null
    ArrayType(Type::typeKind typeKind, Type *elementType) : Type(typeKind), elementType(elementType){};
    ArrayType(Type::typeKind typeKind, std::vector<ExprNode*> dimList, Type *elementType) : Type(typeKind), dimList(dimList), elementType(elementType){};
    virtual std::string toStr() = 0;
    size_t getDim() { return dimList.size(); };
    Type* getElementType() { return elementType; };
    auto getDimList() { return dimList; }
    std::string getDimTypeStr(bool isInt);
    void countEleNum();
    int getCntEleNum() { return cntEleNum; }
    void genDimTypeStrings();
    auto getDimTypeStrings() { return dimTypeStrArray; }
    Type* getTrimType();
};

class ArrayIntType : public ArrayType
{
public:
    ArrayIntType(Type *elementType) : ArrayType(Type::ARRAY_INT, elementType) {};
    ArrayIntType(ArrayIntType* a) : ArrayType(Type::ARRAY_INT, a->getDimList(), a->getElementType()) {};
    ArrayIntType(std::vector<ExprNode*> dimList, Type *elementType) : ArrayType(Type::ARRAY_INT, dimList, elementType) {}; 
    std::string toStr();
    // Type* getElementType() { return  };
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
class ArrayFloatType : public ArrayType
{
public:
    ArrayFloatType(ArrayFloatType* a) : ArrayType(Type::ARRAY_FLOAT, a->getDimList(), a->getElementType()) {};
    ArrayFloatType(Type *elementType) : ArrayType(Type::ARRAY_FLOAT, elementType) {};
    ArrayFloatType(std::vector<ExprNode*> dimList, Type *elementType) : ArrayType(Type::ARRAY_FLOAT, dimList, elementType){};
    std::string toStr();
    // Type* getElementType() { return TypeSystem::floatType; };
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
    FunctionType() : Type(Type::FUNC), returnType(nullptr) {};
    FunctionType(Type* returnType, std::vector<Type*> paramsType) : 
        Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    std::string toStr();
    void setParamsType(std::vector<Type*> params) { paramsType = params;};
    std::vector<Type*> getParamsType() { return paramsType; };
    Type *getReturnType() { return returnType; };
    Type* getRetType() {return returnType;};
};

class TypeSystem
{
private:
    static IntType commonInt;
    static BoolType commonBool;
    static FloatType commonFloat;
    static VoidType commonVoid;
    static ArrayIntType commonArrayInt;
    static ArrayFloatType commonArrayFloat;
    static FunctionType commonFunc;
public:
    static Type *intType;
    static Type *floatType;
    static Type *voidType;
    static Type *boolType;
    static Type *arrayIntType;
    static Type *arrayFloatType;
    static Type *funcType;
};


#define ELEMENT_ARRAY_GAP 4

#endif
