#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>

extern FILE* yyout;

Function::Function(Unit *u, SymbolEntry *s, FunctionDef* def) : funcDefNode(def)
{
    u->insertFunc(this);
    entry = new BasicBlock(this);
    //q4为function加入exit块
    exit = new BasicBlock(this);
    sym_ptr = s;
    parent = u;
}

Function::~Function()
{
    //TODO: 使这个析构能正常运行
    // auto delete_list = block_list;
    // for (auto &i : delete_list)
    //     delete i;
    // parent->removeFunc(this);
}

// remove the basicblock bb from its block_list.
void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}

void Function::output() const
{
    FunctionType* funcType = dynamic_cast<FunctionType*>(sym_ptr->getType());
    Type *retType = funcType->getRetType();
    // fprintf(yyout, "define %s %s() {\n", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    fprintf(yyout, "define %s %s(", retType->toStr().c_str(), sym_ptr->toStr().c_str());
    size_t cnt = 0;
    if(!((FunctionType*)sym_ptr->getType())->getParamsType()[0]->isVoid()){
        for (auto &&i : ((FunctionType*)sym_ptr->getType())->getParamsType())
        {
            fprintf(yyout, "%s %%t%d", i->toStr().c_str(), labelParam[cnt]);
            if(cnt != ((FunctionType*)sym_ptr->getType())->getParamsType().size() - 1)
                fprintf(yyout, ",");
            cnt++;
        }
    }
    fprintf(yyout, ") {\n");
    std::set<BasicBlock *> v;
    std::list<BasicBlock *> q;
    q.push_back(entry);
    v.insert(entry);
    while (!q.empty())
    {
        auto bb = q.front();
        q.pop_front();
        bb->output();
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        {
            if (v.find(*succ) == v.end())
            {
                v.insert(*succ);
                q.push_back(*succ);
            }
        }
    }
    fprintf(yyout, "}\n");
}
