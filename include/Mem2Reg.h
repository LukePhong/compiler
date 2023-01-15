#include <vector>
#include <map>
#include <set>
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

// 定义Renamepass的数据包结构，包括PHI节点所在的块、流入块、流入值等
struct RenamePassData {
    // 按照原意，这里的Value *相当于我们的Operand*
    // using ValVector = std::vector<Value *>;
    using operandMap = map<AllocaInstruction*, Operand*>;
    // using LocationVector = std::vector<DebugLoc>;

    RenamePassData(BasicBlock *B, BasicBlock *P, operandMap V)
            : BB(B), Pred(P), Values(std::move(V)) {}

    BasicBlock *BB;
    BasicBlock *Pred;
    operandMap Values;
    // LocationVector Locations;
};

class Mem2Reg
{
private:
    Unit* unit;
    Function *currFunc;

    vector<AllocaInst> allocas; // 经过简单优化会删除一部分
    int NumDeadAlloca = 0;
    map<Instruction*, AllocaInst> phiAllc;
    // 记录已经访问过的基本块，避免重复访问
    set<BasicBlock *> Visited;
    map<Operand*, AllocaInstruction*> opAlloc;
public:
    Mem2Reg(Unit* u);
    ~Mem2Reg();
public:
    void buildSSA();
private:
    void promoteMemoryToRegister();
    void primarySimplify(), analyzeAlloca();
    void genDomTree(), insertPhiNode(), renamePass(), dePhi(), cleanState();
    bool isPromotable(AllocaInstruction* alloc),
        rewriteSingleStoreAlloca(AllocaInstruction* alloc, AllocaInfo* info);
    void renamePassBody(BasicBlock *BB, BasicBlock *Pred,
                                RenamePassData::operandMap &IncomingVals,
                                /*RenamePassData::LocationVector &IncomingLocs,*/
                                std::vector<RenamePassData> &Worklist);
    // static void updateForIncomingValueLocation(PhiInstruction *PN, DebugLoc DL,
    //                                        bool ApplyMergedLoc);
    AllocaInstruction* findWhoAllocTheOperand(Operand* dst);
};


