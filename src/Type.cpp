#include "Type.h"
#include <sstream>

#include "Ast.h"

IntType TypeSystem::commonInt = IntType(32);
IntType TypeSystem::shortInt = IntType(8);
IntType TypeSystem::longInt = IntType(64);
FloatType TypeSystem::commonFloat = FloatType(32);
VoidType TypeSystem::commonVoid = VoidType();
BoolType TypeSystem::commonBool = BoolType(1);
ArrayIntType TypeSystem::commonArrayInt = ArrayIntType(&commonInt);
ArrayFloatType TypeSystem::commonArrayFloat = ArrayFloatType(&commonFloat);
FunctionType TypeSystem::commonFunc = FunctionType();

Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::shortIntType = &shortInt;
Type* TypeSystem::longIntType = &longInt;
Type* TypeSystem::floatType = &commonFloat;
Type* TypeSystem::voidType = &commonVoid;
Type* TypeSystem::boolType = &commonBool;
Type* TypeSystem::arrayIntType = &commonArrayInt;
Type* TypeSystem::arrayFloatType = &commonArrayFloat;
Type* TypeSystem::funcType = &commonFunc;

std::string IntType::toStr()
{
    std::stringstream ss;
    ss<<"i"<<size;
    return ss.str();
}

std::string ArrayIntType::toStr()
{    
    return getDimTypeStr(true);
}

//q6浮点数支持
std::string FloatType::toStr()
{
    return "float";
}
std::string ArrayFloatType::toStr()
{
    return getDimTypeStr(false);
}

std::string ArrayType::getDimTypeStr(bool isInt){

    std::stringstream ss;
    ss<<"[";
    size_t cnt = 0;
    for (auto &&i : getDimList())
    {
        int val = ((ConstantSymbolEntry*)i->getSymbolEntry())->getValueInt();
        ss<<val<<" x ";
        cnt++;
        if(cnt < getDim()){
            ss<<"[";
        }
    }
    if(isInt)
        ss<<"i32";
    else
        ss<<"float";
    for (size_t i = 0; i < getDim(); i++)
    {
        ss<<"]";
    }
    return ss.str();
}

void ArrayType::countEleNum(){ 
        cntEleNum = 1;
        for (auto &&i : dimList)
        {
            cntEleNum *= ((ConstantSymbolEntry*)i->getSymbolEntry())->getValueInt();
        }
}

void ArrayType::genDimTypeStrings(){
    
    auto tempDimList = getDimList();
    for (size_t i = 1; i < getDim(); i++)
    {
        std::vector<ExprNode*> subDimArr(tempDimList.begin()+i, tempDimList.end());
        // subDimArr.assign(getDimList().begin()+i, getDimList().end());
        std::stringstream ss;
        ss<<"[";
        size_t cnt = 0;
        for (auto &&i : subDimArr)
        {
            int val = ((ConstantSymbolEntry*)i->getSymbolEntry())->getValueInt();
            ss<<val<<" x ";
            cnt++;
            if(cnt < subDimArr.size()){
                ss<<"[";
            }
        }
        if(elementType->isInt())
            ss<<"i32";
        else
            ss<<"float";
        for (size_t i = 0; i < subDimArr.size(); i++)
        {
            ss<<"]";
        }
        dimTypeStrArray.push_back(ss.str());
    }
    dimTypeStrArray.push_back(elementType->toStr());
    
}

Type* ArrayType::getTrimType(){
    if(trimedType)
        return trimedType;

    if(dimList.size()>1){
        ArrayType* t;
        if(elementType->isInt()){
            t = new ArrayIntType((ArrayIntType*)this);
        }else{
            t = new ArrayFloatType((ArrayFloatType*)this);
        }
        t->cntEleNum /= ((ConstantSymbolEntry*)dimList[0]->getSymbolEntry())->getValueInt();
        t->dimList.erase(t->dimList.begin());
        t->dimTypeStrArray = this->dimTypeStrArray;
        assert(!t->dimTypeStrArray.empty());
        t->dimTypeStrArray.erase(t->dimTypeStrArray.begin());
        trimedType = t;
        return t;
    }else{
        Type* t;
        if(elementType->isInt()){
            t = new IntType(32);
        }else{
            t = new FloatType(32);
        }
        trimedType = t;
        return t;
    }
    
}

std::string VoidType::toStr()
{
    return "void";
}

std::string BoolType::toStr()
{
    return "i1";
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer;
    buffer << returnType->toStr() << "()";
    return buffer.str();
}

std::string PointerType::toStr()
{
    std::ostringstream buffer;
    buffer << valueType->toStr() << "*";
    return buffer.str();
}
