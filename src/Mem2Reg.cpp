

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
        dePhi();
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
        // 首先补充分配出的地址op和alloc指令的对应关系
        Operand* op = i.inst->getOperands()[0];
        opAlloc[op] = i.inst;
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
            // 相应的Alloc也要在opAlloc中删除
            opAlloc.erase(op);
            ++NumDeadAlloca;
            continue;
        }

        // 优化2：只有一个store语句(Defining)，且只存在一个基本块中，
        // 那么被这个store基本块所支配的所有load都要被替换为store语句中的右值。
        if (info->DefiningBlocks.size() == 1) {
            if (rewriteSingleStoreAlloca(alloc, info)) {
                i = allocas.erase(i);
                // ++NumSingleStore;
                // 相应的Alloc也要在opAlloc中删除
                opAlloc.erase(op);
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

// 更新PHI节点流入位置
// static void Mem2Reg::updateForIncomingValueLocation(PhiInstruction *PN, DebugLoc DL,
//                                            bool ApplyMergedLoc) {
//     if (ApplyMergedLoc)
//         PN->applyMergedLocation(PN->getDebugLoc(), DL);
//     else
//         PN->setDebugLoc(DL);
// }

// AllocaInstruction* Mem2Reg::findWhoAllocTheOperand(Operand* dst){
    
// }

// 在空PHI节点中加入流入数据并处理替换掉load和store指令
// 本函数是消除递归的DFS
void Mem2Reg::renamePassBody(BasicBlock *BB, BasicBlock *Pred,
                                RenamePassData::operandMap &IncomingVals,
                                /*RenamePassData::LocationVector &IncomingLocs,*/
                                std::vector<RenamePassData> &Worklist) {
    NextIteration:

    // 如果当前块插入了PHI节点
    if (PhiInstruction *phiInst = dynamic_cast<PhiInstruction *>(BB->begin())) {
        if (phiAllc.count(phiInst)) {
            size_t NewPHINumOperands = phiInst->getNumOperands();

            // size_t NumEdges = llvm::count(successors(Pred), BB);
            // assert(NumEdges && "Must be at least one edge from Pred to BB!");
            assert(std::find(Pred->succ_begin(), Pred->succ_end(), BB) != Pred->succ_end());

            Instruction* PNI = BB->begin();
            do {
                AllocaInstruction* AllocaInst = phiAllc[phiInst].inst;

                // 更新PHI节点流入位置
                // updateForIncomingValueLocation(phiInst, IncomingLocs[AllocaInst],
                //                                phiInst->getNumIncomingValues() > 0);

                // 为PHI节点添加操作数，流入数据来自于Pred块
                // 应该只有一条边
                // for (size_t i = 0; i != NumEdges; ++i)
                    phiInst->addIncoming(IncomingVals[AllocaInst], Pred);

                // 记录在IncomingVals数组中，让我们传播起来！
                // 遇到phi节点了，我现在把IncomingVals对应的op改成phi的dst
                IncomingVals[AllocaInst] = phiInst->getOperands()[0];

                // 继续处理下一个，可能一个块有多个phi
                PNI = PNI->getNext();
                phiInst = dynamic_cast<PhiInstruction *>(PNI);
                if (!phiInst)
                    break;

                // PHI节点可能源程序就有，为了区别源程序中的PHI节点和本pass添加的PHI节点，我们获取PHI节点的操作数
                // 手动添加的PHI节点的操作数一定相等，且初始值为0，即没有操作数。
            } while (phiInst->getNumOperands() == NewPHINumOperands);
        }
    }

    // 有环路的情况下可能重复处理，此时直接退出
    if (!Visited.insert(BB).second) //insert返回pair类型
        return;

    // 遍历所有指令做替换。如果是store指令，更新原alloca值；如果是load指令，直接替换为alloca当前更新到的值。替换后删除原指令
    // for (BasicBlock::iterator II = BB->begin(); !II->isTerminator();) {
    for(Instruction* II = BB->begin(); II != BB->end();){
        // Instruction *I = &*II++;
        Instruction *I = II;
        II = II->getNext();

        if (LoadInstruction *LI = dynamic_cast<LoadInstruction*>(I)) {
            // AllocaInst *Src = dynamic_cast<AllocaInst>(LI->getPointerOperand());
            // if (!Src)
            //     continue;

            // DenseMap<AllocaInst *, size_t>::iterator AI = AllocaLookup.find(Src);
            if (opAlloc.find(LI->getOperands()[1]) == opAlloc.end())
                continue;

            auto allocInst = opAlloc[LI->getOperands()[1]];
            //找到load对应的alloca后，获取alloca在当前block的流入值
            //从IncomingVals获取一个最新的op，替代load的内存地址中的值作为load的结果
            Operand *V = IncomingVals[allocInst];

            // if (AC && LI->getMetadata(LLVMContext::MD_nonnull) &&
            //     !isKnownNonZero(V, SQ.DL, 0, AC, LI, &DT))
            //     addAssumeNonNull(AC, LI);

            // 用流入值代替所有user，移除load指令
            LI->replaceAllUsesWith(V);
            LI->removeCurrInst();
        } else if (StoreInstruction *SI = dynamic_cast<StoreInstruction *>(I)) {
            // AllocaInst *Dest = dynamic_cast<AllocaInst>(SI->getPointerOperand());
            // if (!Dest)
            //     continue;

            // DenseMap<AllocaInst *, size_t>::iterator ai = AllocaLookup.find(Dest);
            // if (ai == AllocaLookup.end())
            //     continue;
            if (opAlloc.find(SI->getOperands()[0]) == opAlloc.end())
                continue;
            
            auto allocInst = opAlloc[SI->getOperands()[0]];
            // 获取store对应的alloca后，用store的参数来更新alloca的流出值
            // store意味着要更新IncomingVals中的值了
            IncomingVals[allocInst] = SI->getOperands()[1];

            // IncomingLocs[AllocaNo] = SI->getDebugLoc();
            // 移除store指令
            SI->removeCurrInst();
        }
    }

    // 如果没有后继可处理了，则退出
    std::vector<BasicBlock *>::iterator I = BB->succ_begin(), E = BB->succ_end();
    if (I == E)
        return;

    set<BasicBlock *> VisitedSuccs;

    // 第一个后继不放到队列里，而是直接在下一轮迭代中处理，对应DFS顺序
    VisitedSuccs.insert(*I);
    Pred = BB;
    BB = *I;
    ++I;

    // 其余后继则跟随当前block的流出值接入到worklist中。目的是实现IncomingVals的复用
    for (; I != E; ++I)
        if (VisitedSuccs.insert(*I).second)
            Worklist.emplace_back(*I, Pred, IncomingVals);

    goto NextIteration;
}

void Mem2Reg::renamePass()
{
    // 为所有value设置一个隐式的初始定义，避免出现使用未定义value的情况。所谓name就是def定义的意思
    // 我们不这样做，直接上operand
    RenamePassData::operandMap values;
    for (unsigned i = 0, e = allocas.size(); i != e; ++i)
        // values[i] = UndefValue::get(Allocas[i]->getAllocatedType());
        values[allocas[i].inst] = allocas[i].inst->getOperands()[0];

    // 众所周知PHI节点中要有流入数据的信息，此处用rename进行添加
    // RenamePass做两件事：1.在空PHI节点中加入流入数据 2.处理替换掉load和store指令
    std::vector<RenamePassData> RenamePassWorkList;
    RenamePassWorkList.emplace_back(currFunc->getEntry(), nullptr, values);
                                    // std::move(Locations));
    do {
        RenamePassData RPD = std::move(RenamePassWorkList.back());
        RenamePassWorkList.pop_back();
        renamePassBody(RPD.BB, RPD.Pred, RPD.Values, RenamePassWorkList);
    } while (RenamePassWorkList.size());

    //TODO: 删掉没用的alloc
}

void Mem2Reg::genDomTree()
{
    DomTreeGen dom;
    dom.pass(currFunc);
}

void Mem2Reg::insertPhiNode()
{
    vector<BasicBlock*> workList;
    for(vector<AllocaInst>::iterator i = allocas.begin(); i != allocas.end(); i++){
        for (auto &&blk : currFunc->getBlockList())
        {
            auto defBlk = (*i).info->DefiningBlocks;
            if(std::find(defBlk.begin(), defBlk.end(), blk) == defBlk.end())
                continue;
            blk->setInWorkListFor((*i).inst);
            workList.push_back(blk);
        }
        while (!workList.empty())
        {
            auto n = workList.back();
            workList.pop_back();
            for (auto &&df : n->getDomFrontier())
            {
                //TODO: 是否需要移除只有一步跳的基本块中的phi？
                if(df->getInserted() != (*i).inst){
                    auto temp = new TemporarySymbolEntry((*i).inst->getType(), SymbolTable::getLabel());
                    auto tOp = new Operand(temp);
                    auto phi = new PhiInstruction(tOp);      //注意！！new Inst指定基本块和insertFront不能同时使用，指定基本块自带insertBack效果，输出时会造成无限循环
                    phiAllc[phi] = *i;
                    df->insertFront(phi);
                    df->setInserted((*i).inst);
                    if(df->getInWorkListFor() != (*i).inst){
                        df->setInWorkListFor((*i).inst);
                        workList.push_back(df);
                    }
                }
            }
        }
    }
    // cout<<phiAllc.size()<<endl;
}

void Mem2Reg::dePhi()
{
    // go over each BB
    for (auto &&currBlk : currFunc->getBlockList()){
        if (PhiInstruction *phiInst = dynamic_cast<PhiInstruction *>(currBlk->begin())) {
            do{
                int cnt = 0;
                for (auto &&blk : phiInst->getBlkVec())
                {
                    cnt++;
                    auto inst = new CopyInstruction(phiInst->getOperands()[0], phiInst->getOperands()[cnt]);
                    blk->insertBackBeforeCompAndBr(inst);
                }
                //如果下一条不是phi节点了，phiInst会变成null
                phiInst = dynamic_cast<PhiInstruction *>(phiInst->getNext());
            }while(phiInst);
        }
    }
}

void Mem2Reg::cleanState()
{
    allocas.clear();
    phiAllc.clear();
    Visited.clear();
    opAlloc.clear();
    NumDeadAlloca = 0;
}
