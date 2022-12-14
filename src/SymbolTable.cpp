#include "SymbolTable.h"
#include <iostream>
#include <sstream>

extern FILE* yyout;

SymbolEntry::SymbolEntry(Type *type, int kind) 
{
    this->type = type;
    this->kind = kind;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, int value) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->value_int = value;
    // IsInt = true;
}

//q6浮点数支持
ConstantSymbolEntry::ConstantSymbolEntry(Type *type, float value) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->value_float = value;
    // IsInt = false;
}

//q6浮点数支持
std::string ConstantSymbolEntry::toStr()
{
    std::ostringstream buffer;
    // buffer << value_int;
    if(type->isInt())
        buffer << getValueInt();
    else
        buffer << getValueFloat();
    return buffer.str();
}

//q6浮点数支持
int ConstantSymbolEntry::getValueInt() const
{
    assert(type->isInt());
    return value_int;
}
float ConstantSymbolEntry::getValueFloat() const
{
    assert(type->isFloat());
    return value_float;
}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope)
                     : SymbolEntry(type, SymbolEntry::VARIABLE), name(name)
{
    this->scope = scope;
    addr = nullptr;
}

//q2const常量支持
//使能更改变量类型
IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope, SymbolEntry::EntryType entryType = SymbolEntry::VARIABLE)
                     : SymbolEntry(type, entryType), name(name)
{
    this->scope = scope;
    addr = nullptr;
}

bool IdentifierSymbolEntry::paramListMarch(std::vector<Type*> typeList){
    //TODO: 没有对返回类型匹配的判断（参考C/C++的判断方式）

    std::vector<Type*> targetList = ((FunctionType*)type)->getParamsType();
    if (typeList.size() != targetList.size()){
        // std::cout<<"参数列表长度不匹配 "<<typeList.size()<<" "<<targetList.size()<<std::endl;
        return false;
    }
    for (size_t i = 0; i < typeList.size(); i++)
    {
        //q17参数列表模糊匹配
        if(typeList[i]->getKind() != targetList[i]->getKind() && !(typeList[i]->isNumber() && targetList[i]->isNumber()) ){
            // std::cout<<"参数类型不匹配 "<<typeList[i]->getKind()<<" "<<targetList[i]->getKind()<<std::endl;
            return false;
        }
    }
    
    return true;
}

std::string IdentifierSymbolEntry::toStr()
{
    return "@" + name;
}

void IdentifierSymbolEntry::outputGlbId()
{
    assert(isGlobal() && (type->isNumber() || type->isArrayType()));

    if(type->isInt()) {
        if(glbConst){
            fprintf(yyout, "@%s = global %s %d, align 4 \n", this->name.c_str(), this->type->toStr().c_str(), glbConst->getValueInt());
        }else{
            fprintf(yyout, "@%s = global %s 0, align 4 \n", this->name.c_str(), this->type->toStr().c_str());
        }
    }
    else if(type->isFloat()) {
        if(glbConst){
            fprintf(yyout, "@%s = global %s %.6e\n",this->name.c_str(), this->type->toStr().c_str(), glbConst->getValueFloat());
        }else{
            fprintf(yyout, "@%s = global %s 0.000000e+00 \n",this->name.c_str(), this->type->toStr().c_str());
        }
    }
}

void IdentifierSymbolEntry::outputSysFunc(){

    assert(this->type->isFunc());
    
    fprintf(yyout, "declare %s @%s(", 
        dynamic_cast<FunctionType*>(type)->getRetType()->toStr().c_str(), (const char*)name.c_str());
    bool first = true;
    for(auto type : dynamic_cast<FunctionType*>(type)->getParamsType()){
        if(!type->isVoid()){
            if(!first){
                first = false;
                fprintf(yyout, ", ");
            }
            fprintf(yyout,"%s", type->toStr().c_str());
        }
    }
    fprintf(yyout, ")\n");

}


std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << "%t" << label;
    return buffer.str();
} 

TemporarySymbolEntry::TemporarySymbolEntry(Type *type, int label) : SymbolEntry(type, SymbolEntry::TEMPORARY)
{
    this->label = label;
}



SymbolTable::SymbolTable()
{
    prev = nullptr;
    level = 0;
}

SymbolTable::SymbolTable(SymbolTable *prev)
{
    this->prev = prev;
    this->level = prev->level + 1;
}

/*
    Description: lookup the symbol entry of an identifier in the symbol table
    Parameters: 
        name: identifier name
    Return: pointer to the symbol entry of the identifier

    hint:
    1. The symbol table is a stack. The top of the stack contains symbol entries in the current scope.
    2. Search the entry in the current symbol table at first.
    3. If it's not in the current table, search it in previous ones(along the 'prev' link).
    4. If you find the entry, return it.
    5. If you can't find it in all symbol tables, return nullptr.
*/
SymbolEntry* SymbolTable::lookup(std::string name)
{
    // return symbolTable.begin()->second;

    if(symbolTable.find(name)!=symbolTable.end()){
        return symbolTable[name];
    }
    else{
        if(prev != nullptr){
            return prev->lookup(name);
        }
        else{
            return nullptr;
        }
    }
}
//q12函数调用
SymbolEntry* SymbolTable::lookup(std::string name, std::vector<Type*> typeList){
    // std::cout<<"hello4"<<std::endl;
    // 同时满足名字和参数列表相同才返回
    if(funcTable.find(name)!=funcTable.end()){
        // std::cout<<"hello3"<<std::endl;
        auto pr = funcTable.equal_range(name);
        for (auto iter = pr.first ; iter != pr.second; ++iter){
            if(((IdentifierSymbolEntry*)(iter->second))->paramListMarch(typeList)){
                // for (auto inner = iter++; inner != pr.second; inner++)
                // {
                //     if(((IdentifierSymbolEntry*)(inner->second))->paramListMarch(typeList)){
                //         std::cout<<"错误！同一函数多次定义！"<<std::endl;
                //     }
                // }
                return iter->second;
            }
        }
        return nullptr;
    }
    else{
        if(prev != nullptr){
            return prev->lookup(name, typeList);
        }
        else{
            return nullptr;
        }
    }
}
//p7函数重复重载
int SymbolTable::lookupcount(std::string name, std::vector<Type*> typeList){
    int count = 0;
    // 同时满足名字和参数列表相同才返回
    if(funcTable.find(name)!=funcTable.end()){
        auto pr = funcTable.equal_range(name);
        for (auto iter = pr.first ; iter != pr.second; ++iter){
            if(((IdentifierSymbolEntry*)(iter->second))->paramListMarch(typeList)){
                count++;
            }
        }
        // return nullptr;
    }
    // else{
    if(prev != nullptr){
        return count + prev->lookupcount(name, typeList);
    }
    else{
        return count;
    }
    // }
}
//q12函数调用
void SymbolTable::installFunc(std::string name, SymbolEntry* entry){
    funcTable.insert({name, entry});
}

// install the entry into current symbol table.
void SymbolTable::install(std::string name, SymbolEntry* entry)
{
    //p1同一作用域下重复声明
    if(symbolTable.find(name) != symbolTable.end()){
        std::cout<<"错误！相同作用域下变量重复声明！"<<std::endl;
    }

    symbolTable[name] = entry;
}

int SymbolTable::counter = 0;
static SymbolTable t;
SymbolTable *identifiers = &t;
SymbolTable *globals = &t;
