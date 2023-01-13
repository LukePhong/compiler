#include <vector>
using namespace std;

#include "DomTreeGen.h"
#include "Instruction.h"
// #include "MachineCode.h"
#include "Unit.h"

struct AllocaInfo {

    vector<BasicBlock *> DefiningBlocks;
    vector<BasicBlock *> UsingBlocks;

    StoreInstruction *OnlyStore;
    BasicBlock *OnlyBlock;
    bool OnlyUsedInOneBlock;

    void clear() {
        DefiningBlocks.clear();
        UsingBlocks.clear();
        OnlyStore = nullptr;
        OnlyBlock = nullptr;
        OnlyUsedInOneBlock = true;
    }

    void AnalyzeAlloca(AllocaInstruction *inst) {
        clear();

        Operand* op = inst->getOperands()[0];
        // 获得store指令和load指令所在的基本块，并判断它们是否在同一块中
        for (std::vector<Instruction *>::iterator i = op->use_begin(); i != op->use_end(); i++){
            if (StoreInstruction *SI = dynamic_cast<StoreInstruction*>(*i)) {
                DefiningBlocks.push_back(SI->getParent());
                OnlyStore = SI;
            } else {
                // LoadInstruction *LI = dynamic_cast<LoadInstruction*>(*i);
                // UsingBlocks.push_back(LI->getParent());
                UsingBlocks.push_back((*i)->getParent());
            }

            if (OnlyUsedInOneBlock) {
                if (!OnlyBlock)
                    OnlyBlock = (*i)->getParent();
                else if (OnlyBlock != (*i)->getParent())
                    OnlyUsedInOneBlock = false;
            }
        }
    }
};

struct AllocaInst
{
    AllocaInstruction* inst = nullptr;
    AllocaInfo* info = nullptr;
};


class Mem2Reg
{
private:
    Unit* unit;
    Function *currFunc;

    // vector<AllocaInstruction*> promotable;
    // vector<AllocaInfo*> allocInfo;
    vector<AllocaInst> allocas;
    int NumDeadAlloca = 0;
public:
    Mem2Reg(Unit* u);
    ~Mem2Reg();
public:
    void buildSSA();
private:
    void promoteMemoryToRegister();
    void primarySimplify(), analyzeAlloca();
    void genDomTree(), insertPhiNode(), renamePass(), cleanState();
    bool isPromotable(AllocaInstruction* alloc),
        rewriteSingleStoreAlloca(AllocaInstruction* alloc, AllocaInfo* info);
};


