#include "Instruction.h"
#include "BasicBlock.h"
#include <iostream>
#include "Function.h"
#include "Type.h"
extern FILE* yyout;

int cntParam = 0;
MachineFunction* lastFunc = nullptr;

Instruction::Instruction(unsigned instType, BasicBlock *insert_bb)
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr)
    {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
}

Instruction::~Instruction()
{
    parent->remove(this);
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

void Instruction::setParent(BasicBlock *bb)
{
    parent = bb;
}

void Instruction::setNext(Instruction *inst)
{
    next = inst;
}

void Instruction::setPrev(Instruction *inst)
{
    prev = inst;
}

Instruction *Instruction::getNext()
{
    return next;
}

Instruction *Instruction::getPrev()
{
    return prev;
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    //pushback顺序不能改变
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::~BinaryInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void BinaryInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[0]->getType()->toStr();

    if(operands[0]->getType()->isFloat()){
        switch (opcode)
        {
        case ADD:
            op = "fadd";
            break;
        case SUB:
            op = "fsub";
            break;
        case MUL:
            op = "fmul";
            break;
        case DIV:
            op = "fdiv";
            break;
        default:
            break;
        }
    }else{
        switch (opcode)
        {
        case ADD:
            op = "add";
            break;
        case SUB:
            op = "sub";
            break;
        case MUL:
            op = "mul";
            break;
        case DIV:
            op = "sdiv";
            break;
        case MOD:
            op = "srem";
            break;
        default:
            break;
        }
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb): Instruction(CMP, insert_bb){
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::~CmpInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();

    if(operands[1]->getType()->isFloat()){
        switch (opcode)
        {
        case E:
            op = "oeq";
            break;
        case NE:
            op = "one";
            break;
        case L:
            op = "olt";
            break;
        case LE:
            op = "ole";
            break;
        case G:
            op = "ogt";
            break;
        case GE:
            op = "oge";
            break;
        default:
            op = "";
            break;
        }
        fprintf(yyout, "  %s = fcmp %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
    }else{
        switch (opcode)
        {
        case E:
            op = "eq";
            break;
        case NE:
            op = "ne";
            break;
        case L:
            op = "slt";
            break;
        case LE:
            op = "sle";
            break;
        case G:
            op = "sgt";
            break;
        case GE:
            op = "sge";
            break;
        default:
            op = "";
            break;
        }
        fprintf(yyout, "  %s = icmp %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
    }
    
}

UncondBrInstruction::UncondBrInstruction(BasicBlock *to, BasicBlock *insert_bb) : Instruction(UNCOND, insert_bb)
{
    branch = to;
}

void UncondBrInstruction::output() const
{
    fprintf(yyout, "  br label %%B%d\n", branch->getNo());
}

void UncondBrInstruction::setBranch(BasicBlock *bb)
{
    branch = bb;
}

BasicBlock *UncondBrInstruction::getBranch()
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock*true_branch, BasicBlock*false_branch, Operand *cond, BasicBlock *insert_bb) : Instruction(COND, insert_bb){
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
}

CondBrInstruction::~CondBrInstruction()
{
    operands[0]->removeUse(this);
}

void CondBrInstruction::output() const
{
    std::string cond, type;
    cond = operands[0]->toStr();
    type = operands[0]->getType()->toStr();
    int true_label = true_branch->getNo();
    int false_label = false_branch->getNo();
    fprintf(yyout, "  br %s %s, label %%B%d, label %%B%d\n", type.c_str(), cond.c_str(), true_label, false_label);
}

void CondBrInstruction::setFalseBranch(BasicBlock *bb)
{
    false_branch = bb;
}

BasicBlock *CondBrInstruction::getFalseBranch()
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock *bb)
{
    true_branch = bb;
}

BasicBlock *CondBrInstruction::getTrueBranch()
{
    return true_branch;
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    if(src != nullptr)
    {
        operands.push_back(src);
        src->addUse(this);
    }
}

RetInstruction::~RetInstruction()
{
    if(!operands.empty())
        operands[0]->removeUse(this);
}

void RetInstruction::output() const
{
    if(operands.empty())
    {
        fprintf(yyout, "  ret void\n");
    }
    else
    {
        std::string ret, type;
        ret = operands[0]->toStr();
        type = operands[0]->getType()->toStr();
        fprintf(yyout, "  ret %s %s\n", type.c_str(), ret.c_str());
    }
}

AllocaInstruction::AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

AllocaInstruction::~AllocaInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
}

void AllocaInstruction::output() const
{
    std::string dst, type;
    dst = operands[0]->toStr();
    type = se->getType()->toStr();
    if(se->getType()->isNumber())
        fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
    else
        fprintf(yyout, "  %s = alloca %s, align 16\n", dst.c_str(), type.c_str());
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb) : Instruction(LOAD, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

LoadInstruction::~LoadInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void LoadInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = load %s, %s %s, align 4\n", dst.c_str(), dst_type.c_str(), src_type.c_str(), src.c_str());
}

//q13添加数组IR支持
GetElementPtrInstruction::GetElementPtrInstruction(Operand *dst, Operand *src_addr, Operand * dim, BasicBlock *insert_bb, IdentifierSymbolEntry* ident) 
    : LoadInstruction(dst, src_addr, insert_bb), dim(dim), arr(ident)
{
    dim->addUse(this);
}

void GetElementPtrInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    //%7 = getelementptr inbounds [1 x i32], [1 x i32]* @aaa, i64 0, i64 0, align 4
    fprintf(yyout, "  %s = getelementptr inbounds ", dst.c_str());
    // if(dim->getEntry()->isConstant())
    if(((PointerType*)operands[1]->getType())->getValueType()->isArrayType())   // 这才是决定那个0的原因，但如果是多维数组可能有问题？
        fprintf(yyout, "%s, %s %s, i64 0, ", src_type.substr(0, src_type.length() - 1).c_str(), src_type.c_str(), src.c_str());
    else
        fprintf(yyout, "%s, %s %s, ", src_type.substr(0, src_type.length() - 1).c_str(), src_type.c_str(), src.c_str());
    fprintf(yyout, "%s %s\n", dim->getType()->toStr().c_str(), dim->toStr().c_str());
}

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb) : Instruction(STORE, insert_bb)
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}

StoreInstruction::~StoreInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void StoreInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();

    fprintf(yyout, "  store %s %s, %s %s, align 4\n", src_type.c_str(), src.c_str(), dst_type.c_str(), dst.c_str());
}

//q5FunctionCall的代码生成
FunctionCallInstuction::FunctionCallInstuction(Operand *dst, std::vector<Operand*> params, IdentifierSymbolEntry* func, BasicBlock *insert_bb) : Instruction(CALL, insert_bb)
{
    this->func = func;
    operands = params;
    operands.insert(operands.begin(), dst);
    dst->setDef(this);
    for (auto &&i : params)
    {
        i->addUse(this);
    }
}

FunctionCallInstuction::~FunctionCallInstuction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    for (size_t i = 1; i < operands.size(); i++)
    {
        operands[i]->removeUse(this);
    }
}

void FunctionCallInstuction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    if(!operands[0]->getSymbolEntry()->getType()->isVoid())
        fprintf(yyout, "  %s = call %s %s(", dst.c_str(), dst_type.c_str(), func->toStr().c_str());
    else
        fprintf(yyout, "  call %s %s(", dst_type.c_str(), func->toStr().c_str());
    for (size_t i = 1; i < operands.size(); i++)
    {
        fprintf(yyout, "%s %s", operands[i]->getType()->toStr().c_str(), operands[i]->toStr().c_str());
        if(i != operands.size() - 1)
            fprintf(yyout, ",");
    }
    fprintf(yyout, ")\n");
    
}

ZextInstruction::ZextInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(ZEXT, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

ZextInstruction::~ZextInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void ZextInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = zext %s %s to %s\n", dst.c_str(), src_type.c_str(), src.c_str(), dst_type.c_str());
}

void BitCastInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = bitcast %s %s to %s\n", dst.c_str(), src_type.c_str(), src.c_str(), dst_type.c_str());
}

IntFloatCastInstruction::IntFloatCastInstruction(unsigned opcode, Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(CAST, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

IntFloatCastInstruction::~IntFloatCastInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void IntFloatCastInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();
    std::string castType;
    switch(opcode) {
        case I2F:
            castType = "sitofp";
            break;
        case F2I:
            castType = "fptosi";
            break;
        default:
            castType = "";
            break;
    }
    fprintf(yyout, "  %s = %s %s %s to %s\n", dst.c_str(), castType.c_str(), src_type.c_str(), src.c_str(), dst_type.c_str());
}

//==================MachineCode==========================//
MachineOperand* Instruction::genMachineOperand(Operand* ope)
{
    //TODO: 全局常量可能需要调整
    auto se = ope->getEntry();
    MachineOperand* mope = nullptr;
    if(se->isConstant() && se->getType()->isNumber()){
        // 当心精度损失！！！  用fval保存浮点型
        if(((ConstantSymbolEntry*)se)->isInt())
            mope = new MachineOperand(MachineOperand::IMM, dynamic_cast<ConstantSymbolEntry*>(se)->getValueInt());
        else
            mope = new MachineOperand(MachineOperand::IMM, 0, true, dynamic_cast<ConstantSymbolEntry*>(se)->getValueFloat());
    }else if(se->isTemporary() && !(se->getType()->isPtrType() && ((PointerType*)se->getType())->isArray())){
        if(se->getType()->isFloat())
            mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry*>(se)->getLabel(), true);
        else
            mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry*>(se)->getLabel());
    }
    else if(se->isVariable() 
            || (se->isConstant() && !se->getType()->isNumber())
            || (se->isTemporary() && se->getType()->isPtrType()))
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry*>(se);
        // 如果是Temporary进来可能没有id_se
        if((id_se != nullptr && id_se->isGlobal()) || (se->getType()->isPtrType() && ((PointerType*)se->getType())->isGlobal()))    // ||后面的部分是为了数组 应改为isArray&&isAllDefined
            mope = new MachineOperand(id_se->toAsmStr().c_str());   // "addr_"是输出时补上的
        else{
            // assert(0);
            // 生成数组首地址在栈中的偏移量
            mope = new MachineOperand(MachineOperand::IMM, ((TemporarySymbolEntry*)se)->getOffset());
        }
    }
    return mope;
}

MachineOperand* Instruction::genMachineReg(int reg) 
{
    return new MachineOperand(MachineOperand::REG, reg);
}

MachineOperand* Instruction::genMachineVReg(bool isFlt) 
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel(), isFlt);   // VREG的数一直++
}

MachineOperand* Instruction::genMachineImm(int val) 
{
    return new MachineOperand(MachineOperand::IMM, val);
}

MachineOperand* Instruction::genMachineLabel(int block_no)
{
    std::ostringstream buf;
    buf << ".L" << block_no;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder* builder)
{
    /* HINT:
    * Allocate stack space for local variabel
    * Store frame offset in symbol entry */
    auto cur_func = builder->getFunction();
    // int offset = cur_func->AllocSpace(4);
    // 分情况，数组需要计算空间的大小
    int offset;
    
    // if(lastFunc != builder->getFunction()){
    //     cntParam = 0;
    //     lastFunc = builder->getFunction();
    // }
    // if(cntParam < 4 || !(se->isVariable() && ((IdentifierSymbolEntry*)se)->isParam())){
    if(!(se->isVariable() && ((IdentifierSymbolEntry*)se)->getParamNumber() > 3)){
        // 先与上判断条件：防止错误的类型转换
        if(!se->isTemporary() && ((IdentifierSymbolEntry*)se)->isArray()){
            auto i = ((IdentifierSymbolEntry*)se);
            offset = cur_func->AllocSpace(((SizedType*)i->getArrayType()->getElementType())->getSize() * i->getArrayType()->getCntEleNum() / 4);
        }else{
            offset = cur_func->AllocSpace(4);
        }
        dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->setOffset(-offset);
    }else{
        // 参数已经在栈上了，不用再分配空间
        dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->setOffset(8 + 4 * (((IdentifierSymbolEntry*)se)->getParamNumber() - 4));
    }
    // 只有param才能更新cnt
    // if(se->isVariable() && ((IdentifierSymbolEntry*)se)->isParam())
    //     cntParam++;
}

void LoadInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    // Load global operand
    if( (operands[1]->getEntry()->isVariable() || operands[1]->getEntry()->isConstant()) && 
        dynamic_cast<IdentifierSymbolEntry*>(operands[1]->getEntry())->isGlobal())
    {
        if(operands[0]->getType()->isFloat())
        {
            auto dst = genMachineOperand(operands[0]);
            auto internal_reg1 = genMachineVReg();
            auto internal_reg2 = new MachineOperand(*internal_reg1);
            auto src = genMachineOperand(operands[1]);
            // example: load r0, addr_a
            cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
            cur_block->InsertInst(cur_inst);
            // example: load r1, [r0]
            cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2, nullptr, LoadMInstruction::VLDR);
            cur_block->InsertInst(cur_inst);
        }
        else
        {
            auto dst = genMachineOperand(operands[0]);
            auto internal_reg1 = genMachineVReg();
            auto internal_reg2 = new MachineOperand(*internal_reg1);
            auto src = genMachineOperand(operands[1]);
            // example: load r0, addr_a
            cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
            cur_block->InsertInst(cur_inst);
            // example: load r1, [r0]
            cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2);
            cur_block->InsertInst(cur_inst);
        }
    }
    // Load local operand
    else if(operands[1]->getEntry()->isTemporary()
    && operands[1]->getDef()
    && operands[1]->getDef()->isAlloc())
    {
        if(operands[0]->getType()->isFloat()){
            auto dst = genMachineOperand(operands[0]);
            auto src1 = genMachineReg(11);
            auto src2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[1]->getEntry())->getOffset());
            cur_inst = new LoadMInstruction(cur_block, dst, src1, src2, LoadMInstruction::VLDR);
            cur_block->InsertInst(cur_inst);
        }
        else{
            // example: load r1, [r0, #4]
            auto dst = genMachineOperand(operands[0]);
            auto src1 = genMachineReg(11);
            auto src2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[1]->getEntry())->getOffset());
            cur_inst = new LoadMInstruction(cur_block, dst, src1, src2);
            cur_block->InsertInst(cur_inst);
        }

    }
    // Load operand from temporary variable
    else
    {
        if(operands[0]->getType()->isFloat()){
            auto dst = genMachineOperand(operands[0]);
            auto src = genMachineOperand(operands[1]);
            cur_inst = new LoadMInstruction(cur_block, dst, src, nullptr, LoadMInstruction::VLDR);
            cur_block->InsertInst(cur_inst);
        }
        else{
            // example: load r1, [r0]
            auto dst = genMachineOperand(operands[0]);
            auto src = genMachineOperand(operands[1]);
            cur_inst = new LoadMInstruction(cur_block, dst, src);
            cur_block->InsertInst(cur_inst);
        }
    }
}

void StoreInstruction::genMachineCode(AsmBuilder* builder)
{
    //p1补全str指令的输出和生成
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;

    // Store global operand
    if( (operands[0]->getEntry()->isVariable() || operands[0]->getEntry()->isConstant()) && 
        dynamic_cast<IdentifierSymbolEntry*>(operands[0]->getEntry())->isGlobal())
    {
        auto src = genMachineOperand(operands[1]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        auto dst_addr = genMachineOperand(operands[0]);
        // example: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, dst_addr);
        cur_block->InsertInst(cur_inst);
        if(src->isImm()){
            if(operands[1]->getType()->isFloat())
            {
                auto mov_dst = genMachineVReg(true);
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
                cur_block->InsertInst(cur_inst);
                internal_reg = new MachineOperand(*internal_reg);
                cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV, mov_dst, internal_reg);
                cur_block->InsertInst(cur_inst);
                src = new MachineOperand(*mov_dst);
            }
            else
            {
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
                cur_block->InsertInst(cur_inst);
                src = internal_reg;
            }
        }
        if(operands[1]->getType()->isFloat())
        {
            cur_inst = new StoreMInstruction(cur_block, src, internal_reg2, nullptr, StoreMInstruction::VSTR);
            cur_block->InsertInst(cur_inst);
        }
        else{
            // example: str r1, [r0]
            cur_inst = new StoreMInstruction(cur_block, src, internal_reg2);
            cur_block->InsertInst(cur_inst);
        }
        
    }
    // Load function params 该分支必须位于local operand之下，isParam()可能不准确，导致两个分支同时成立
    else if(!operands[1]->getEntry()->isTemporary() && !operands[1]->getEntry()->isConstant()
         && ((IdentifierSymbolEntry*)(operands[1]->getEntry()))->isParam()){
        auto se = ((IdentifierSymbolEntry*)(operands[1]->getEntry()));
        if(se->getParamNumber() <4){
            auto src = genMachineReg(se->getParamNumber());
            auto dst1 = genMachineReg(11);
            auto dst2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->getOffset());
            cur_inst = new StoreMInstruction(cur_block, src, dst1, dst2);
            cur_block->InsertInst(cur_inst);
        }else{
            ;
        }
    }
    // Load local operand
    else if(operands[0]->getEntry()->isTemporary()
    && operands[0]->getDef()
    && operands[0]->getDef()->isAlloc())
    {
        // example: store r1, [r0, #4]
        auto src = genMachineOperand(operands[1]);
        // 避免src是立即数
        if(src->isImm()){
            if(operands[1]->getType()->isFloat())
            {
                auto mov_dst = genMachineVReg(true);
                auto internal_reg1 = genMachineVReg();
                cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
                cur_block->InsertInst(cur_inst);
                internal_reg1 = new MachineOperand(*internal_reg1);
                cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV, mov_dst, internal_reg1);
                cur_block->InsertInst(cur_inst);
                src = new MachineOperand(*mov_dst);
            }
            else
            {
                auto internal_reg1 = genMachineVReg();
                cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
                cur_block->InsertInst(cur_inst);
                src = internal_reg1;
            }
        }
        auto dst1 = genMachineReg(11);
        auto dst2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->getOffset());
        if(operands[1]->getType()->isFloat())
        {
            cur_inst = new StoreMInstruction(cur_block, src, dst1, dst2,StoreMInstruction::VSTR);
            cur_block->InsertInst(cur_inst);
        }
        else
        {
            cur_inst = new StoreMInstruction(cur_block, src, dst1, dst2);
            cur_block->InsertInst(cur_inst);
        }
        
    }
    // store operand from temporary variable
    else
    {
        // example: store r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        if(src->isImm()){
            if(operands[1]->getType()->isFloat())
            {
                auto mov_dst = genMachineVReg(true);
                auto internal_reg1 = genMachineVReg();
                cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
                cur_block->InsertInst(cur_inst);
                internal_reg1 = new MachineOperand(*internal_reg1);
                cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV, mov_dst, internal_reg1);
                cur_block->InsertInst(cur_inst);
                src = new MachineOperand(*mov_dst);
            }
            else
            {
                auto internal_reg1 = genMachineVReg();
                cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
                cur_block->InsertInst(cur_inst);
                src = internal_reg1;
            }
        }
        if(operands[1]->getType()->isFloat())
        {
            cur_inst = new StoreMInstruction(cur_block, src, dst, nullptr, StoreMInstruction::VSTR);
            cur_block->InsertInst(cur_inst);
        }
        else
        {
            cur_inst = new StoreMInstruction(cur_block, src, dst);
            cur_block->InsertInst(cur_inst);
        }
        
    }
}

void BinaryInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO:
    // complete other instructions
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    MachineInstruction* cur_inst = nullptr;
    if(operands[1]->getType()->isFloat()){
        if(src1->isImm())
        {
            auto temp_reg = genMachineVReg(true);
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
            cur_block->InsertInst(cur_inst);
            internal_reg = new MachineOperand(*internal_reg);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,temp_reg, internal_reg);
            cur_block->InsertInst(cur_inst);
            src1 = new MachineOperand(*temp_reg);
        }
        if(src2->isImm())
        {
            auto temp_reg = genMachineVReg(true);
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
            cur_block->InsertInst(cur_inst);
            internal_reg = new MachineOperand(*internal_reg);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,temp_reg, internal_reg);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*temp_reg);
        }
        switch (opcode)
        {
        case ADD:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::VADD, dst, src1, src2);
            break;
        case SUB:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::VSUB, dst, src1, src2);
            break;
        case MUL:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::VMUL, dst, src1, src2);
            break;
        case DIV:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::VDIV, dst, src1, src2);
            break;
        default:
            break;
        }
        cur_block->InsertInst(cur_inst);
    }
    else{
        
        /* HINT:
        * The source operands of ADD instruction in ir code both can be immediate num.
        * However, it's not allowed in assembly code.
        * So you need to insert LOAD/MOV instrucrion to load immediate num into register.
        * As to other instructions, such as MUL, CMP, you need to deal with this situation, too.*/
        if(src1->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
            cur_block->InsertInst(cur_inst);
            src1 = new MachineOperand(*internal_reg);
        }
        if(src2->isOverFlowImm() || ((opcode == MUL || opcode == MOD || opcode == DIV ) && src2->isImm())){
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*internal_reg);
        }
        // auto op1 = genMachineReg(0);
        // auto op2 = genMachineReg(1);
        switch (opcode)
        {
        case ADD:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);
            break;
        case SUB:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, src2);
            break;
        case MUL:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst, src1, src2);
            break;
        case DIV:
            // cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, op1, src1);
            // cur_block->InsertInst(cur_inst);
            // cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, op2, src2);
            // cur_block->InsertInst(cur_inst);
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
            // cur_block->InsertInst(cur_inst);
            // cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, op1);
            break;
        case MOD:
            // cur_inst = new LoadMInstruction(cur_block, op1, src1);
            // cur_block->InsertInst(cur_inst);
            // cur_inst = new LoadMInstruction(cur_block, op2, src2);
            // cur_block->InsertInst(cur_inst);
            // cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MOD, dst, src1, src2);
            // cur_block->InsertInst(cur_inst);
            // cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, op2);
            // a % b = a - a / b * b
            {
                cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
                MachineOperand *dst1 = new MachineOperand(*dst);
                src1 = new MachineOperand(*src1);
                src2 = new MachineOperand(*src2);
                cur_block->InsertInst(cur_inst);
                cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst1, dst, src2);
                cur_block->InsertInst(cur_inst);
                dst = new MachineOperand(*dst1);
                cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, dst1);
            }
            break;
        case AND:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::AND, dst, src1, src2);
            break;
        case OR:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::OR, dst, src1, src2);
            break;
        case LSL:
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::LSL, dst, src1, src2);
            break;
        default:
            break;
        }
        cur_block->InsertInst(cur_inst);
    }
}

void CmpInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    
    auto cur_block = builder->getBlock();
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    //保存当前cmp语句的条件，在条件分支语句中使用
    cur_block->setBranchCond(opcode);
    MachineInstruction* cur_inst = nullptr;

    if(operands[1]->getType()->isFloat()){
        if(src1->isImm())
        {
            MachineOperand* temp_reg = genMachineVReg(true);
            MachineOperand* internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
            cur_block->InsertInst(cur_inst);
            internal_reg = new MachineOperand(*internal_reg);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,temp_reg, internal_reg);
            cur_block->InsertInst(cur_inst);
            src1 = new MachineOperand(*temp_reg);
        }
        if(src2->isImm())
        {
            MachineOperand* temp_reg = genMachineVReg(true);
            MachineOperand* internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
            cur_block->InsertInst(cur_inst);
            internal_reg = new MachineOperand(*internal_reg);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV,temp_reg, internal_reg);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*temp_reg);
        }

        cur_inst = new CmpMInstruction(cur_block, src1, src2, CmpMInstruction::VCMP, opcode);
        cur_block->InsertInst(cur_inst);

        cur_inst = new VmrsMInstruction(cur_block);
        cur_block->InsertInst(cur_inst);

        auto dst = genMachineOperand(operands[0]);
        auto trueResult = genMachineImm(1);
        auto falseResult = genMachineImm(0);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, trueResult, opcode);
        cur_block->InsertInst(cur_inst);
        switch (opcode)
        {
        case CmpInstruction::E:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::NE);
            break;
        case CmpInstruction::NE:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::E);
            break;
        case CmpInstruction::G:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::LE);
            break;
        case CmpInstruction::L:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::GE);
            break;
        case CmpInstruction::GE:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::L);
            break;
        case CmpInstruction::LE:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::G);
            break;
        default:
            break;
        }
        cur_block->InsertInst(cur_inst);
    }
    else{
        if(src1->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
            cur_block->InsertInst(cur_inst);
            src1 = new MachineOperand(*internal_reg);
        }
        if(src2->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*internal_reg);
        }

        cur_inst = new CmpMInstruction(cur_block, src1, src2, opcode);
        cur_block->InsertInst(cur_inst);

        //是否需要？
        // //将比较结果存储到dst中，设置两条条件mov语句，给定条件相反，只有一条执行
        auto dst = genMachineOperand(operands[0]);
        auto trueResult = genMachineImm(1);
        auto falseResult = genMachineImm(0);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, trueResult, opcode);
        cur_block->InsertInst(cur_inst);
        switch (opcode)
        {
        case CmpInstruction::E:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::NE);
            break;
        case CmpInstruction::NE:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::E);
            break;
        case CmpInstruction::G:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::LE);
            break;
        case CmpInstruction::L:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::GE);
            break;
        case CmpInstruction::GE:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::L);
            break;
        case CmpInstruction::LE:
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseResult, CmpInstruction::G);
            break;
        default:
            break;
        }
        cur_block->InsertInst(cur_inst);
    }
}

void ZextInstruction::genMachineCode(AsmBuilder* builder){
    MachineBlock* cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    MachineOperand* src = genMachineOperand(operands[1]);
    if(src->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
        cur_block->InsertInst(cur_inst);
        src = new MachineOperand(*internal_reg);
    }
    MachineOperand* dst = genMachineOperand(operands[0]);
    cur_inst = new ZextMInstruction(cur_block, dst, src);
    cur_block->InsertInst(cur_inst);
}

void IntFloatCastInstruction::genMachineCode(AsmBuilder* builder){
    // auto src = operands[0];
    // auto dst = operands[1];
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    if(opcode == F2I){
        auto op_src = genMachineOperand(operands[1]);
        auto op_dst = genMachineOperand(operands[0]);

        if(op_src->isImm()){
            auto temp = genMachineVReg(true);
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, op_src);
            cur_block->InsertInst(cur_inst);
            internal_reg = new MachineOperand(*internal_reg);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV, temp,internal_reg);
            cur_block->InsertInst(cur_inst);
            op_src = temp;
        }

        auto dst = genMachineVReg(true);
        cur_inst = new VcvrMInstruction(cur_block, VcvrMInstruction::F2I, dst, op_src);
        cur_block->InsertInst(cur_inst);
        auto dst_a = new MachineOperand(*dst);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV, op_dst, dst_a);
        cur_block->InsertInst(cur_inst);
    }else if(opcode == I2F){
        auto op_src = genMachineOperand(operands[1]);
        auto op_dst = genMachineOperand(operands[0]);

        if(op_src->isImm()){
            auto temp = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, temp, op_src);
            cur_block->InsertInst(cur_inst);
            op_src = new MachineOperand(*temp);
        }

        auto dst = genMachineVReg(true);
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::VMOV, dst, op_src);
        cur_block->InsertInst(cur_inst);
        auto dst_a = new MachineOperand(*dst);
        cur_inst = new VcvrMInstruction(cur_block, VcvrMInstruction::I2F, op_dst, dst_a);
        cur_block->InsertInst(cur_inst);
    }else{
        assert(0);
    }
}

void UncondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    //p4实现无条件跳转
    auto cur_block = builder->getBlock();
    auto op = new MachineOperand(".L" + std::to_string(branch->getNo()));
    MachineInstruction* cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, op);
    cur_block->InsertInst(cur_inst);
}

void CondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    auto cur_block = builder->getBlock();
    //根据cmp语句时保存的cond跳转到不同块
    auto true_block = new MachineOperand(".L" + std::to_string(true_branch->getNo()));
    auto false_block = new MachineOperand(".L" + std::to_string(false_branch->getNo()));
    MachineInstruction* cur_inst;
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, true_block, cur_block->getBranchCond());
    cur_block->InsertInst(cur_inst);
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, false_block, MachineInstruction::NONE);
    cur_block->InsertInst(cur_inst);
}

void RetInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    /* HINT:
    * 1. Generate mov instruction to save return value in r0
    * 2. Restore callee saved registers and sp, fp
    * 3. Generate bx instruction */
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    //1. Generate mov instruction to save return value in r0
    if(!operands.empty()){
        if (operands[0]->getType()->isFloat()) {
            auto src = genMachineOperand(operands[0]);
            if(src->isImm()) {
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
                cur_block->InsertInst(cur_inst);
                src = internal_reg;
            }
            auto dst = new MachineOperand(MachineOperand::REG, 16);
        }
        else {
            auto src = genMachineOperand(operands[0]);
            //立即数->寄存器
            if(src->isImm())
            {
                auto internal_reg = genMachineVReg();
                cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
                cur_block->InsertInst(cur_inst);
                src = new MachineOperand(*internal_reg);
            }
            auto dst = new MachineOperand(MachineOperand::REG, 0);//r0
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
            cur_block->InsertInst(cur_inst);
        }
    }
    // 生成一条跳转到结尾函数栈帧处理的无条件跳转语句
    auto dst = new MachineOperand(".L" + this->getParent()->getParent()->getSymPtr()->toStr().erase(0,1) + "_END");
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst);
    cur_block->InsertInst(cur_inst);
}


void FunctionCallInstuction::genMachineCode(AsmBuilder* builder){
    int saved_reg_cnt = 0;
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    std::vector<MachineOperand*> additional_args;
    // 应从右向左进行处理
    for(unsigned int i = operands.size() - 1; i > 0; i--){
        //如果类型是数组，需要考虑局部数组指针的情况
        // if(operands[i]->getEntry()->getType()->isArrayType()){
        // 这里应该改为使用symbol entry里面的isArray
        if(operands[i]->getEntry()->getType()->isPtrType()){
            //需要保证不是值而是数组指针
            // bool isPointer = false;
            // if(((ArrayType*)operands[i]->getEntry()->getType())->getElementType()->isInt()){
            //     isPointer = dynamic_cast<IntArrayType*>(operands[i]->getEntry()->getType())->getPointer();
            //     // dynamic_cast<IntArrayType*>(operands[i]->getEntry()->getType())->setPointer(false);
            //     //如果第一维为-1，表明其为指针，传参时需要注意不加fp
            //     if(dynamic_cast<IntArrayType*>(operands[i]->getEntry()->getType())->getDimensions()[0]==-1){
            //         isPointer = false;
            //     }
            // }else{
            //     ;
            // }
            //必须保证是局部数组，而且不是传进来的参数
            //必须确定这个数组在当前函数的调用栈中能够找到，因为这里拿到的是偏移地址
            //此处如何区分局部和全局区?
            // if(1/*is local*/){
                auto dst_addr = genMachineVReg();
                auto fp = genMachineReg(11);
                auto offset = genMachineOperand(operands[i]);
                if(offset->isImm()) {
                    // auto val = ((ConstantSymbolEntry*)(operands[i]->getEntry()))->isInt() ? ((ConstantSymbolEntry*)(operands[i]->getEntry()))->getValueInt() : ((ConstantSymbolEntry*)(operands[i]->getEntry()))->getValueFloat();
                    // auto val = ((TemporarySymbolEntry*)operands[i]->getEntry())->getOffset();
                    auto val = offset->getVal();
                    // auto offset = new MachineOperand(MachineOperand::IMM, val);
                    if(val > 255 || val < -255) {
                        auto internal_reg = genMachineVReg();
                        cur_inst = new LoadMInstruction(cur_block, internal_reg, offset);
                        cur_block->InsertInst(cur_inst);
                        offset = new MachineOperand(*internal_reg);
                    }
                    cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst_addr, fp, offset);
                }else{
                    cur_inst = new LoadMInstruction(cur_block, dst_addr, offset);
                }
                cur_block->InsertInst(cur_inst);

                //左起前4个参数通过r0-r3传递
                if(i<=4){
                    auto dst = new MachineOperand(MachineOperand::REG, i-1);//r0-r3
                    cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, dst_addr);
                    cur_block->InsertInst(cur_inst);
                }
                else{
                    additional_args.clear();
                    additional_args.push_back(dst_addr);
                    cur_inst = new StackMInstruction(cur_block, StackMInstruction::PUSH, additional_args);
                    cur_block->InsertInst(cur_inst);
                    saved_reg_cnt++;
                }
            // }
            // else{
            //     //左起前4个参数通过r0-r3传递
            //     if(i<=4){
            //         auto dst = new MachineOperand(MachineOperand::REG, i-1);//r0-r3
            //         cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, genMachineOperand(operands[i]));
            //         cur_block->InsertInst(cur_inst);
            //     }
            //     else{
            //         // additional_args.push_back(genMachineOperand(operands[i]));
            //         additional_args.clear();
            //         additional_args.push_back(genMachineOperand(operands[i]));
            //         cur_inst = new StackMInstruction(cur_block, StackMInstruction::PUSH, additional_args);
            //         cur_block->InsertInst(cur_inst);
            //         saved_reg_cnt++;
            //     }
            // }
        }
        else{
            //左起前4个参数通过r0-r3传递
            if(i<=4){
                auto dst = new MachineOperand(MachineOperand::REG, i-1);//r0-r3
                cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, genMachineOperand(operands[i]));
                cur_block->InsertInst(cur_inst);
            }
            else{
                additional_args.clear();
                MachineOperand* operand = genMachineOperand(operands[i]);
                if(operand->isImm()) {
                    MachineOperand* internal_reg = genMachineVReg();
                    cur_inst = new LoadMInstruction(cur_block, internal_reg, operand);
                    cur_block->InsertInst(cur_inst);
                    operand = new MachineOperand(*internal_reg);
                }
                additional_args.push_back(operand);
                cur_inst = new StackMInstruction(cur_block, StackMInstruction::PUSH, additional_args);
                cur_block->InsertInst(cur_inst);
                saved_reg_cnt++;
            }
        }
    }
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, new MachineOperand(func->toAsmStr(), true));
    cur_block->InsertInst(cur_inst);
    // 对于有返回值的函数调用 需要提供一条从mov r0, dst的指令
    if( ((FunctionType*)func->getType())->getRetType() != TypeSystem::voidType) {
        auto dst = genMachineOperand(operands[0]);
        auto src = new MachineOperand(MachineOperand::REG, 0);//r0
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
        cur_block->InsertInst(cur_inst);
    }
    // 恢复栈帧 调整sp
    if(saved_reg_cnt){
        auto src1 = genMachineReg(13);
        auto src2 = genMachineImm(saved_reg_cnt*4);
        if(saved_reg_cnt*4 > 255 || saved_reg_cnt*4 < -255) {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*internal_reg);
        }
        auto dst = genMachineReg(13);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
}

void ZextInstruction::genMachineCode(AsmBuilder* builder){
    MachineBlock* cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    MachineOperand* src = genMachineOperand(operands[1]);
    if(src->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
        cur_block->InsertInst(cur_inst);
        src = new MachineOperand(*internal_reg);
    }
    MachineOperand* dst = genMachineOperand(operands[0]);
    cur_inst = new ZextMInstruction(cur_block, dst, src);
    cur_block->InsertInst(cur_inst);
}

void BitCastInstruction::genMachineCode(AsmBuilder* builder){
    // 强制类型转换因为不会变动数据，不需要生成任何汇编指令
    MachineBlock* cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    //p5汇编数组寻址-起始地址
    if(operands[1]->getEntry()->isVariable())
        operands[0]->setEntry(operands[1]->getEntry());
    else
        ((TemporarySymbolEntry*)operands[0]->getEntry())->setOffset(((TemporarySymbolEntry*)operands[1]->getEntry())->getOffset());

    MachineOperand* dst = genMachineOperand(operands[0]);
    MachineOperand* src = genMachineOperand(operands[1]);
    cur_inst = new BitCastMInstruction(cur_block, dst, src);
    cur_block->InsertInst(cur_inst);
}

void GetElementPtrInstruction::genMachineCode(AsmBuilder* builder){
    MachineBlock* cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;

    auto ptrType = (PointerType*)operands[0]->getType();
    bool isLastLevel = ((PointerType*)operands[0]->getType())->getValueType()->isNumber();
    auto num = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
    Operand* tempOp;
    tempOp = operands[0];
    tempOp->setEntry(num);
    MachineOperand* dst = genMachineOperand(tempOp);
    MachineOperand* src = genMachineOperand(operands[1]);
    // bool isImmSrc = src->isImm();
    bool isLabel = src->isLabel();
    bool isParam = arr->isVariable() && arr->isParam();
    bool isGlb = arr->isGlobal();
    // 对于全局数组，src可能是bridge
    if(src->isImm() || src->isLabel())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src);
        cur_block->InsertInst(cur_inst);
        src = new MachineOperand(*internal_reg);
    }

    MachineOperand* opDim = genMachineOperand(dim);
    if(opDim->isImm()){
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, opDim);
        cur_block->InsertInst(cur_inst);
        opDim = new MachineOperand(*internal_reg);
    }
    // 需要获取余下的维度中的元素总数
    if(!isLastLevel){
        ((ArrayType*)ptrType->getValueType())->countEleNum();
        auto eleMOp = genMachineImm(((ArrayType*)ptrType->getValueType())->getCntEleNum());
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, eleMOp);
        cur_block->InsertInst(cur_inst);
        eleMOp = new MachineOperand(*internal_reg);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, opDim, opDim, eleMOp);
        cur_block->InsertInst(cur_inst);    
    }
    auto cstMOp = genMachineImm(2);
    cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::LSL, opDim, opDim, cstMOp);
    cur_block->InsertInst(cur_inst);
    cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src, opDim);
    cur_block->InsertInst(cur_inst);
    // 到了最后一层了，我们最终的目的是和fp一起算出正确的位置
    // 但是全局的不用，传参进来的也不用
    if(isLastLevel && !isLabel && !isParam && !isGlb){
        auto fp = genMachineReg(11);
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, fp, dst);
        cur_block->InsertInst(cur_inst);
    }
}