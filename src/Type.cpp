#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(4);
FloatType TypeSystem::commonFloat = FloatType(4);
VoidType TypeSystem::commonVoid = VoidType();
BoolType TypeSystem::commonBool = BoolType(1);
ArrayIntType TypeSystem::commonArrayInt = ArrayIntType();
ArrayFloatType TypeSystem::commonArrayFloat = ArrayFloatType();

Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::floatType = &commonFloat;
Type* TypeSystem::voidType = &commonVoid;
Type* TypeSystem::boolType = &commonBool;
Type* TypeSystem::arrayIntType = &commonArrayInt;
Type* TypeSystem::arrayFloatType = &commonArrayFloat;

std::string IntType::toStr()
{
    return "int";
}
std::string ArrayIntType::toStr()
{
    return "array_int";
}

//q6浮点数支持
std::string FloatType::toStr()
{
    return "float";
}
std::string ArrayFloatType::toStr()
{
    return "array_float";
}

std::string VoidType::toStr()
{
    return "void";
}

std::string BoolType::toStr()
{
    return "bool";
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer;
    buffer << returnType->toStr() << "()";
    return buffer.str();
}
