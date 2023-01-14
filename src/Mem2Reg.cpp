

#include "Mem2Reg.h"

// #include "MachineCode.h"
// #include "Instruction.h"
#include "Operand.h"

Mem2Reg::Mem2Reg(Unit* u) : unit(u)
{
}

Mem2Reg::~Mem2Reg()
{
}

void Mem2Reg::buildSSA(){
    for (vector<Function *>::iterator f = unit->begin(); f != unit->end(); f++)
    {
        currFunc = *f;
        promoteMemoryToRegister();
        analyzeAlloca();
        genDomTree();
        primarySimplify();
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
            // 如果出现了非零索引，就意味着当前地址需要作为偏移基址，这就不能提升
            if(!gep->isZeroDim())
                return false;
        }else if(BitCastInstruction* btc = dynamic_cast<BitCastInstruction*>(*i)){
            // 这个没事
        }else{
            return false;
        }
    }
    return true;
}

void Mem2Reg::promoteMemoryToRegister()
{
    for (auto &&i : currFunc->getBlockList())
    {
        auto inst = i->begin();
        while(inst != i->end())
        {
            if(AllocaInstruction* alloc = dynamic_cast<AllocaInstruction*>(inst)){
                if(this->isPromotable(alloc))
                    allocas.push_back({alloc, nullptr});
            }
            inst = inst->getNext();
        }
    }
    return;
}

void Mem2Reg::analyzeAlloca()
{
    for (auto &&i : allocas)
    {
        auto info = new AllocaInfo();
        info->AnalyzeAlloca(i.inst);
        i.info = info;
    }
}

bool Mem2Reg::rewriteSingleStoreAlloca(AllocaInstruction* alloc, AllocaInfo* info){
    StoreInstruction *OnlyStore = info->OnlyStore;
    bool StoringGlobalVal = OnlyStore->isStoringGlobalVal();
    BasicBlock *StoreBB = OnlyStore->getParent();
    int StoreIndex = -1;

    // 清空原本的使用(load)块
    info->UsingBlocks.clear();
    Operand* op = alloc->getOperands()[0];

    // for (User *U : make_early_inc_range(AI->users())) {
    for (std::vector<Instruction *>::iterator i = op->use_begin(); i != op->use_end(); i++){

        Instruction *UserInst = *i;
        if (UserInst == OnlyStore)
            continue;
        LoadInstruction *LI;
        if(!dynamic_cast<LoadInstruction*>(UserInst))
            continue;
        LI = dynamic_cast<LoadInstruction*>(UserInst);

        // 首先确保是alloca出的局部变量
        if (!StoringGlobalVal) {
            if (LI->getParent() == StoreBB) {
                // 然后根据索引判断load是否被store所支配
                // if (StoreIndex == -1)
                //     StoreIndex = LBI.getInstructionIndex(OnlyStore);

                // load在store之前，load不做处理，重新放回UsingBlocks中
                if (StoreBB->isInstAfterInst(OnlyStore, LI)) {
                    info->UsingBlocks.push_back(StoreBB);
                    continue;
                }
            } else if (!LI->getParent()->theBlockDomMe(StoreBB)) {   // 等待建立好支配者树之后恢复，我们的支配者树只能看是否是idom、计算domfnt
                // 不在同一个块中，如果store不支配这个load，则也放弃处理
                info->UsingBlocks.push_back(LI->getParent());
                continue;
            }
        }

        // 否则就替换掉load
        Operand *ReplVal = OnlyStore->getOperands()[1];  // 取store的src
        // store支配load但流程上load出现在store之前，说明存在一个不经过函数入口的路径先到达了load再到store，
        // 这时假设是undef，因为函数不可能不从入口进入
        // if (ReplVal == LI)
        //     ReplVal = PoisonValue::get(LI->getType());

        // if (AC && LI->getMetadata(LLVMContext::MD_nonnull) &&
        //     !isKnownNonZero(ReplVal, DL, 0, AC, LI, &DT))
        //     addAssumeNonNull(AC, LI);

        // 替换掉load的所有user
        LI->replaceAllUsesWith(ReplVal);
        LI->removeCurrInst();
        // LBI.deleteValue(LI);
    }

    // 没有load说明完成了
    if (info->UsingBlocks.size())
        return false;

    // 移除本此处理好的store和alloca
    info->OnlyStore->removeCurrInst();
    // LBI.deleteValue(info.OnlyStore);

    alloc->removeCurrInst();
    return true;
}

void Mem2Reg::primarySimplify()
{
    // for (size_t i = 0; i < promotable.size(); i++)
    for(vector<AllocaInst>::iterator i = allocas.begin(); i != allocas.end();)
    {
        auto alloc = (*i).inst;
        auto info = (*i).info;
        Operand* op = alloc->getOperands()[0];
        // 三种优化，都很好理解
        // 优化1：如果alloca出的空间从未被使用，直接删除
        if (!op->usersNum()) {
            alloc->removeCurrInst();
            i = allocas.erase(i);   // 这才是正确的删除方式
            ++NumDeadAlloca;
            continue;
        }

        // 优化2：只有一个store语句(Defining)，且只存在一个基本块中，
        // 那么被这个store基本块所支配的所有load都要被替换为store语句中的右值。
        if (info->DefiningBlocks.size() == 1) {
            if (rewriteSingleStoreAlloca(alloc, info)) {
                i = allocas.erase(i);
                // ++NumSingleStore;
                continue;
            }
        }

        // 优化3：如果某局部变量的读/写(load/store)都只存在一个基本块中，load要被之前离他最近的store的右值替换
        // 没时间了！！！
        // if (Info.OnlyUsedInOneBlock && promoteSingleBlockAlloca(alloc, Info, LBI, SQ.DL, DT, AC)) {
        //     shouldRemove.push_back(i);
        //     continue;
        // }

        i++;    //删除和迭代器自增不能同时发生
    }

    // for (auto &&i : shouldRemove)
    // {
    //     promotable.erase(promotable.begin() + i);   // 决不能这样写！
    // }
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
