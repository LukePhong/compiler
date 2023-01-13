

#include "Mem2Reg.h"

#include "MachineCode.h"
#include "Instruction.h"
#include "Operand.h"

Mem2Reg::Mem2Reg(MachineUnit* u) : unit(u)
{
}

Mem2Reg::~Mem2Reg()
{
}

void Mem2Reg::buildSSA(){
    for (auto &f : unit->getFuncs())
    {
        currFunc = f;
        promoteMemoryToRegister();
        primarySimplify();
        genDomTree();
        insertPhiNode();
        renamePass();
        cleanState();
    }
}

bool Mem2Reg::isPromotable(AllocaInstruction* alloc){
    Operand* op = alloc->getOperands()[0];
    for (std::vector<Instruction *>::iterator i = op->use_begin(); i != op->use_end(); i++)
    {
        // op也就是dst，它是一个地址，如果他被str ldr getelementptr(GEP)指令以外的指令使用了，那就是对地址运算，就不是Promotable
        if(LoadInstruction* ld = dynamic_cast<LoadInstruction*>(*i)){
            // load到的op类型和alloc指令的类型不符，无法提升
            if(ld->getType()->getKind() != alloc->getType()->getKind())
                return false;
        }else if(StoreInstruction* st = dynamic_cast<StoreInstruction*>(*i)){
            //不允许alloc的结果作为store的左操作数，仅能做右操作数，即被存储的对象；str指令要存的op的类型和alloc指令的类型不符，无法提升
            if(st->getOperands()[1] == op   //获取st的src
                || st->getType()->getKind() != alloc->getType()->getKind())
                return false;
        }else if(GetElementPtrInstruction* gep = dynamic_cast<GetElementPtrInstruction*>(*i)){
            
        }else{
            return false;
        }
    }
    return true;
}

void Mem2Reg::promoteMemoryToRegister()
{
    vector<AllocaInstruction*> promotable;
    for (auto &&i : currFunc->getBlocks())
    {
        for (auto &&inst : i->getInsts())
        {
            if(AllocaInstruction* alloc = dynamic_cast<AllocaInstruction*>(inst)){
                if(this->isPromotable(alloc))
                    promotable.push_back(alloc);
            }
        }
    }
    
}

void Mem2Reg::primarySimplify()
{
}

void Mem2Reg::renamePass()
{
}

void Mem2Reg::genDomTree()
{
    DomTreeGen dom;
    dom.pass(currFunc);
}

void Mem2Reg::insertPhiNode()
{
}

void Mem2Reg::cleanState()
{
}
