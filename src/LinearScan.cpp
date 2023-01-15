#include <algorithm>
#include "LinearScan.h"
#include "MachineCode.h"
#include "LiveVariableAnalysis.h"

LinearScan::LinearScan(MachineUnit *unit)
{
    this->unit = unit;
    for (int i = 4; i < 11; i++)
        regs.push_back(i);
    for (int i = 21; i < 48; i++)
        fregs.push_back(i);
}

void LinearScan::allocateRegisters()
{
    for (auto &f : unit->getFuncs())
    {
        func = f;
        bool success;
        success = false;
        while (!success)        // repeat until all vregs can be mapped
        {
            computeLiveIntervals();
            success = linearScanRegisterAllocation();
            if (success)        // all vregs can be mapped to real regs
                modifyCode();
            else                // spill vregs that can't be mapped to real regs
                genSpillCode();
        }
    }
}

void LinearScan::makeDuChains()
{
    LiveVariableAnalysis lva;
    lva.pass(func);
    du_chains.clear();
    int i = 0;
    std::map<MachineOperand, std::set<MachineOperand *>> liveVar;
    for (auto &bb : func->getBlocks())
    {
        liveVar.clear();
        for (auto &t : bb->getLiveOut())
            liveVar[*t].insert(t);
        int no;
        no = i = bb->getInsts().size() + i;
        for (auto inst = bb->getInsts().rbegin(); inst != bb->getInsts().rend(); inst++)
        {
            (*inst)->setNo(no--);
            for (auto &def : (*inst)->getDef())
            {
                if (def->isVReg())
                {
                    auto &uses = liveVar[*def];
                    du_chains[def].insert(uses.begin(), uses.end());
                    auto &kill = lva.getAllUses()[*def];
                    std::set<MachineOperand *> res;
                    set_difference(uses.begin(), uses.end(), kill.begin(), kill.end(), inserter(res, res.end()));
                    liveVar[*def] = res;
                }
            }
            for (auto &use : (*inst)->getUse())
            {
                if (use->isVReg())
                    liveVar[*use].insert(use);
            }
        }
    }
}

void LinearScan::computeLiveIntervals()
{
    makeDuChains();
    intervals.clear();
    for (auto &du_chain : du_chains)
    {
        int t = -1;
        for (auto &use : du_chain.second)
            t = std::max(t, use->getParent()->getNo());
        Interval *interval = new Interval({du_chain.first->getParent()->getNo(), t, false, 0, 0, du_chain.first->isFloat(), {du_chain.first}, du_chain.second});
        intervals.push_back(interval);
    }
    for (auto& interval : intervals) {
        auto uses = interval->uses;
        auto begin = interval->start;
        auto end = interval->end;
        for (auto block : func->getBlocks()) {
            auto liveIn = block->getLiveIn();
            auto liveOut = block->getLiveOut();
            bool in = false;
            bool out = false;
            for (auto use : uses)
                if (liveIn.count(use)) {
                    in = true;
                    break;
                }
            for (auto use : uses)
                if (liveOut.count(use)) {
                    out = true;
                    break;
                }
            if (in && out) {
                begin = std::min(begin, (*(block->begin()))->getNo());
                end = std::max(end, (*(block->rbegin()))->getNo());
            } else if (!in && out) {
                for (auto i : block->getInsts())
                    if (i->getDef().size() > 0 &&
                        i->getDef()[0] == *(uses.begin())) {
                        begin = std::min(begin, i->getNo());
                        break;
                    }
                end = std::max(end, (*(block->rbegin()))->getNo());
            } else if (in && !out) {
                begin = std::min(begin, (*(block->begin()))->getNo());
                int temp = 0;
                for (auto use : uses)
                    if (use->getParent()->getParent() == block)
                        temp = std::max(temp, use->getParent()->getNo());
                end = std::max(temp, end);
            }
        }
        interval->start = begin;
        interval->end = end;
    }
    bool change;
    change = true;
    while (change)
    {
        change = false;
        std::vector<Interval *> t(intervals.begin(), intervals.end());
        for (size_t i = 0; i < t.size(); i++)
            for (size_t j = i + 1; j < t.size(); j++)
            {
                Interval *w1 = t[i];
                Interval *w2 = t[j];
                if (**w1->defs.begin() == **w2->defs.begin())
                {
                    std::set<MachineOperand *> temp;
                    set_intersection(w1->uses.begin(), w1->uses.end(), w2->uses.begin(), w2->uses.end(), inserter(temp, temp.end()));
                    if (!temp.empty())
                    {
                        change = true;
                        w1->defs.insert(w2->defs.begin(), w2->defs.end());
                        w1->uses.insert(w2->uses.begin(), w2->uses.end());
                        // w1->start = std::min(w1->start, w2->start);
                        // w1->end = std::max(w1->end, w2->end);
                        auto w1Min = std::min(w1->start, w1->end);
                        auto w1Max = std::max(w1->start, w1->end);
                        auto w2Min = std::min(w2->start, w2->end);
                        auto w2Max = std::max(w2->start, w2->end);
                        w1->start = std::min(w1Min, w2Min);
                        w1->end = std::max(w1Max, w2Max);
                        auto it = std::find(intervals.begin(), intervals.end(), w2);
                        if (it != intervals.end())
                            intervals.erase(it);
                    }
                }
            }
    }
    sort(intervals.begin(), intervals.end(), compareStart);
}

// 需要补充
bool LinearScan::linearScanRegisterAllocation()
{
    bool retValue = true;
    active.clear();
    regs.clear();
    fregs.clear();
    // 初始化所有的寄存器都是可分配的
    for (int i = 4; i < 11; i++)
        regs.push_back(i);
    for (int i = 21; i < 48; i++)
        fregs.push_back(i);
    // 尝试为全部的活跃区间分配寄存器
    for(auto &interval : intervals){
        expireOldIntervals(interval);
        //判断 active 列表中 interval 的数目和可用的物理寄存器数目是否相等
        if(interval->freg){ // 需要的是浮点寄存器
            // 剩余寄存器数目为0，说明溢出了
            if(fregs.size() == 0){
                spillAtInterval(interval);
                retValue = false;
            }
            else{//当前有可用于分配的物理寄存器
                interval->rreg = fregs[fregs.size()-1];//为 unhandled interval 分配物理寄存器
                fregs.pop_back();
                active.push_back(interval); //该interval变为active
                sort(active.begin(), active.end(), insertComp); //保持结束位置递增排列
            }
        }
        else{   // 需要的是普通寄存器
            if(regs.size() == 0){
                spillAtInterval(interval);
                retValue = false;
            }
            else{//当前有可用于分配的物理寄存器
                interval->rreg = regs[regs.size()-1];//为 unhandled interval 分配物理寄存器
                regs.pop_back();
                //再按照活跃区间结束位置，将其插入到 active 列表中
                active.push_back(interval);
                sort(active.begin(), active.end(), insertComp);
            }
        }
    }
    return retValue;
}

void LinearScan::modifyCode()
{   
    //对于生存区间内的每一个def、use进行更改
    //无需更改
    for (auto &interval : intervals)
    {
        func->addSavedRegs(interval->rreg);
        for (auto def : interval->defs)
            def->setReg(interval->rreg);
        for (auto use : interval->uses)
            use->setReg(interval->rreg);
    }
}

void LinearScan::genSpillCode()
{
    for(auto &interval:intervals)
    {
        if(!interval->spill)
            continue;
        // TODO
        /* HINT:
         * The vreg should be spilled to memory.
         * 1. insert ldr inst before the use of vreg
         * 2. insert str inst after the def of vreg
         */ 
        // The vreg should be spilled to memory.
        interval->disp = func->AllocSpace(4);
        // 1. insert ldr inst before the use of vreg
        for (auto use : interval->uses){
            MachineBlock* block = use->getParent()->getParent();
            MachineOperand* offset = new MachineOperand(MachineOperand::IMM, -interval->disp);
            if(!use->isFloat()){
                block->insertBefore(use->getParent(), new LoadMInstruction(block, new MachineOperand(*use), new MachineOperand(MachineOperand::REG, 11), offset, LoadMInstruction::LDR));
            }
            else{
                block->insertBefore(use->getParent(), new LoadMInstruction(block, new MachineOperand(*use), new MachineOperand(MachineOperand::REG, 11), offset, LoadMInstruction::VLDR));
            }
        }
        // 2. insert str inst after the def of vreg
        for (auto def : interval->defs){
            MachineBlock* block = def->getParent()->getParent();
            MachineOperand* offset = new MachineOperand(MachineOperand::IMM, -interval->disp);
            if(!def->isFloat()){
                block->insertAfter(def->getParent(), new StoreMInstruction(block, new MachineOperand(*def), new MachineOperand(MachineOperand::REG, 11), offset, StoreMInstruction::STR));
            }
            else{
                block->insertAfter(def->getParent(), new StoreMInstruction(block, new MachineOperand(*def), new MachineOperand(MachineOperand::REG, 11), offset, StoreMInstruction::VSTR));
            }
        }
    }
}

void LinearScan::expireOldIntervals(Interval *interval)
{

    for(std::vector<Interval*>::iterator it = active.begin(); it != active.end(); ){
        // 考虑排列顺序，如果最早结束的都要比目标区间的开始时间晚，那么就返回
        if(!((*it)->end < interval->start)){
            return;
        }
        //rreg
        if ((*it)->rreg < 11) {
            //释放寄存器
            regs.push_back((*it)->rreg);
            //将不影响后续interval的active抛弃掉，一定和寄存器同步修改
            it = active.erase(find(active.begin(), active.end(), *it));
            sort(regs.begin(), regs.end());
        }
        //freg
        else{
            fregs.push_back((*it)->rreg);
            it = active.erase(find(active.begin(), active.end(), *it));
            sort(fregs.begin(), fregs.end()); 
        }
    }
}

void LinearScan::spillAtInterval(Interval *interval)
{
    if(active[active.size()-1]->end <= interval->end){  //unhandled interval 的结束时间更晚
        interval->spill = true; //只需要置位其 spill 标志位
    }
    else{   // active 列表中的 interval 结束时间更晚
        active[active.size()-1]->spill = true;  //置位其spill标志位
        interval->rreg = active[active.size()-1]->rreg; //将其占用的寄存器分配给 unhandled interval
        //按照unhandled interval活跃区间结束位置，将其插入到 active 列表中
        active.push_back(interval);
        sort(active.begin(), active.end(), insertComp);
    }
}

bool LinearScan::compareStart(Interval *a, Interval *b)
{
    return a->start < b->start;
}