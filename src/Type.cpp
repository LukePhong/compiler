#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(32);
FloatType TypeSystem::commonFloat = FloatType(4);
VoidType TypeSystem::commonVoid = VoidType();
BoolType TypeSystem::commonBool = BoolType(1);
ArrayIntType TypeSystem::commonArrayInt = ArrayIntType(&commonInt);
ArrayFloatType TypeSystem::commonArrayFloat = ArrayFloatType(&commonFloat);

Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::floatType = &commonFloat;
Type* TypeSystem::voidType = &commonVoid;
Type* TypeSystem::boolType = &commonBool;
Type* TypeSystem::arrayIntType = &commonArrayInt;
Type* TypeSystem::arrayFloatType = &commonArrayFloat;

std::string IntType::toStr()
{
    return "i32";
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
