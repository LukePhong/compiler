#include "SymbolTable.h"
#include <iostream>
#include <sstream>

SymbolEntry::SymbolEntry(Type *type, int kind) 
{
    this->type = type;
    this->kind = kind;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, int value) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->value_int = value;
}

//q6浮点数支持
ConstantSymbolEntry::ConstantSymbolEntry(Type *type, float value) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->value_float = value;
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
}

//q2const常量支持
//使能更改变量类型
IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope, SymbolEntry::EntryType entryType = SymbolEntry::VARIABLE)
                     : SymbolEntry(type, entryType), name(name)
{
    this->scope = scope;
}

// IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope, int value) : SymbolEntry(type, SymbolEntry::VARIABLE), 
//                                                                                                     name(name), value(value)
// {
//     this->scope = scope;
// }

std::string IdentifierSymbolEntry::toStr()
{
    return name;
}

TemporarySymbolEntry::TemporarySymbolEntry(Type *type, int label) : SymbolEntry(type, SymbolEntry::TEMPORARY)
{
    this->label = label;
}

std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << "t" << label;
    return buffer.str();
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

// install the entry into current symbol table.
void SymbolTable::install(std::string name, SymbolEntry* entry)
{
    symbolTable[name] = entry;
}

int SymbolTable::counter = 0;
static SymbolTable t;
SymbolTable *identifiers = &t;
SymbolTable *globals = &t;
