#include "Unit.h"

extern FILE* yyout;

void Unit::insertFunc(Function *f)
{
    func_list.push_back(f);
}

void Unit::removeFunc(Function *func)
{
    func_list.erase(std::find(func_list.begin(), func_list.end(), func));
}

void Unit::output() const
{
    fprintf(yyout, "target triple = \"x86_64-pc-linux-gnu\"\n");

    //q6在全局区添加系统函数声明和全局变量
    //先打印全局变量
    for (auto &&i : glbIds)
    {
        i->outputGlbId();
    }
    
    for (auto &func : func_list)
        func->output();

    //打印系统函数
    for (auto &&i : sysFuncs)
    {
        i->outputSysFunc();
    }
    
}

void Unit::genMachineCode(MachineUnit* munit) 
{
    AsmBuilder* builder = new AsmBuilder();
    builder->setUnit(munit);
    for (auto &func : func_list)
        func->genMachineCode(builder);
}

Unit::~Unit()
{
    for(auto &func:func_list)
        delete func;
}
