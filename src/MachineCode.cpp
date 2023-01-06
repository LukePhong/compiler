#include "MachineCode.h"
#include "Unit.h"
extern FILE* yyout;

MachineOperand::MachineOperand(int tp, int val)
{
    this->type = tp;
    if(tp == MachineOperand::IMM){
        this->val = val;
    }
    else 
        this->reg_no = val;
}

MachineOperand::MachineOperand(std::string label, bool isFuncLabel) : isFuncLabel(isFuncLabel)
{
    this->type = MachineOperand::LABEL;
    this->label = label;
}

bool MachineOperand::operator==(const MachineOperand&a) const
{
    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

bool MachineOperand::operator<(const MachineOperand&a) const
{
    if(this->type == a.type)
    {
        if(this->type == IMM)
            return this->val < a.val;
        return this->reg_no < a.reg_no;
    }
    return this->type < a.type;

    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

void MachineOperand::PrintReg()
{
    // 特殊寄存器
    switch (reg_no)
    {
    case 11:
        fprintf(yyout, "fp");
        break;
    case 13:
        fprintf(yyout, "sp");
        break;
    case 14:
        fprintf(yyout, "lr");
        break;
    case 15:
        fprintf(yyout, "pc");
        break;
    default:
        fprintf(yyout, "r%d", reg_no);
        break;
    }
}

void MachineOperand::output() 
{
    /* HINT：print operand
    * Example:
    * immediate num 1 -> print #1;
    * register 1 -> print r1;
    * lable addr_a -> print addr_a; */
    switch (this->type)
    {
    case IMM:
        fprintf(yyout, "#%d", this->val);
        break;
    case VREG:
        fprintf(yyout, "v%d", this->reg_no);
        break;
    case REG:
        PrintReg();
        break;
    case LABEL:
        if (this->label.substr(0, 2) == ".L" || isFuncLabel)
            fprintf(yyout, "%s", this->label.c_str());
        else
            fprintf(yyout, "addr_%s", this->label.c_str());
    default:
        break;
    }
}

void MachineInstruction::PrintCond()
{
    // TODO
    switch (cond)
    {
    case LT:
        fprintf(yyout, "lt");
        break;
    default:
        break;
    }
}

BinaryMInstruction::BinaryMInstruction(
    MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::BINARY;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
}

void BinaryMInstruction::output() 
{
    // TODO: 
    // Complete other instructions
    switch (this->op)
    {
    case BinaryMInstruction::ADD:
        fprintf(yyout, "\tadd ");
        break;
    case BinaryMInstruction::SUB:
        fprintf(yyout, "\tsub ");
        break;
    case BinaryMInstruction::MUL:
        fprintf(yyout, "\tmul ");
        break;
    case BinaryMInstruction::DIV:
        // fprintf(yyout, "\tbl\t__aeabi_idiv\n");
        fprintf(yyout, "\tsdiv ");
        break;
    // case BinaryMInstruction::MOD:
    //     fprintf(yyout, "\tbl\t__aeabi_idivmod\n");
    //     break;
    case BinaryMInstruction::AND:
        fprintf(yyout, "\tand ");
        break;
    case BinaryMInstruction::OR:
        fprintf(yyout, "\torr ");
        break;
    default:
        break;
    }
    // if(op != DIV && op != MOD){
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
    // }
}

LoadMInstruction::LoadMInstruction(MachineBlock* p,
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2,
    int op, int cond)
{
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    if (src2)
        this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    if (src2)
        src2->setParent(this);
}

void LoadMInstruction::output()
{
    fprintf(yyout, "\tldr ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");

    // Load immediate num, eg: ldr r1, =8
    if(this->use_list[0]->isImm())
    {
        fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
        return;
    }

    // Load address
    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "[");

    this->use_list[0]->output();
    if( this->use_list.size() > 1 )
    {
        fprintf(yyout, ", ");
        this->use_list[1]->output();
    }

    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

StoreMInstruction::StoreMInstruction(MachineBlock* p,
    MachineOperand* src1, MachineOperand* src2, MachineOperand* src3, 
    int op, int cond)
{
    //p1补全str指令的输出和生成
    this->parent = p;
    this->type = MachineInstruction::STORE;
    this->op = op;
    this->cond = cond;
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    if (src3)
        this->use_list.push_back(src3);
    src1->setParent(this);
    src2->setParent(this);
    if (src3)
        src3->setParent(this);
}

void StoreMInstruction::output()
{
    //p1补全str指令的输出和生成
    fprintf(yyout, "\tstr ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");

    // store address
    if(this->use_list[1]->isReg()||this->use_list[1]->isVReg())
        fprintf(yyout, "[");

    this->use_list[1]->output();
    if( this->use_list.size() > 2 )
    {
        fprintf(yyout, ", ");
        this->use_list[2]->output();
    }

    if(this->use_list[1]->isReg()||this->use_list[1]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

MovMInstruction::MovMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src,
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::MOV;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}

void MovMInstruction::output() 
{
    //如何表示cond？
    switch (cond)
    {
    case EQ:
        fprintf(yyout, "\tmoveq\t");
        break;
    case NE:
        fprintf(yyout, "\tmovne\t");
        break;
    case LT:
        fprintf(yyout, "\tmovlt\t");
        break;
    case GT:
        fprintf(yyout, "\tmovgt\t");
        break;
    case LE:
        fprintf(yyout, "\tmovle\t");
        break;
    case GE:
        fprintf(yyout, "\tmovge\t");
        break;
    default:
        fprintf(yyout, "\tmov\t");
        break;
    }
    // fprintf(yyout, "\tmov ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

BranchMInstruction::BranchMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, 
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::BRANCH;
    this->op = op;
    this->cond = cond;
    this->use_list.push_back(dst);
    dst->setParent(this);
}

void BranchMInstruction::output()
{
    switch (op)
    {
    case B:
        switch (cond)
        {
        case EQ:
            fprintf(yyout, "\tbeq\t");
            break;
        case NE:
            fprintf(yyout, "\tbne\t");
            break;
        case LT:
            fprintf(yyout, "\tblt\t");
            break;
        case GT:
            fprintf(yyout, "\tbgt\t");
            break;
        case LE:
            fprintf(yyout, "\tble\t");
            break;
        case GE:
            fprintf(yyout, "\tbge\t");
            break;
        default:
            fprintf(yyout, "\tb\t");
            break;
        }
        break;
    case BL:
        fprintf(yyout, "\tbl\t");
        break;
    case BX:
        fprintf(yyout, "\tbx\t");
        break;
    default:
        break;
    }
    use_list[0]->output();
    fprintf(yyout, "\n");
}

CmpMInstruction::CmpMInstruction(MachineBlock* p, 
    MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::CMP;
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);
    this->cond = cond;
}

void CmpMInstruction::output()
{
    // TODO
    // Jsut for reg alloca test
    // delete it after test
    fprintf(yyout, "\tcmp ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, "\n");
}

StackMInstruction::StackMInstruction(MachineBlock* p, int op, 
    std::vector<MachineOperand*> src,
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::STACK;
    this->op = op;
    this->cond = cond;
    this->use_list = src;
    for(auto reg : use_list){
        reg->setParent(this);
    }
}

void StackMInstruction::output()
{
    switch(op){
    case PUSH:
        fprintf(yyout, "\tpush {");
        break;
    case POP:
        fprintf(yyout, "\tpop {");
        break;
    case VPUSH:
        fprintf(yyout, "\tvpush {");
        break;
    case VPOP:
        fprintf(yyout, "\tvpop {");
        break;
    }
    if(use_list.size() <= 16) {
        this->use_list[0]->output();
        for (long unsigned int i = 1; i < use_list.size(); i++) {
            fprintf(yyout, ", ");
            this->use_list[i]->output();
        }
    }
    // 浮点寄存器可能会很多 每次只能push/pop16个
    else {
        this->use_list[0]->output();
        for (long unsigned int i = 1; i < 16; i++) {
            fprintf(yyout, ", ");
            this->use_list[i]->output();
        }
        fprintf(yyout, "}\n");
        if(op == VPUSH) {
            fprintf(yyout, "\tvpush ");
        }
        else if(op == VPOP){
            fprintf(yyout, "\tvpop ");
        }
        fprintf(yyout, "{");
        this->use_list[16]->output();
        for (long unsigned int i = 17; i < use_list.size(); i++) {
            fprintf(yyout, ", ");
            this->use_list[i]->output();
        }
    }
    fprintf(yyout, "}\n");
}

ZextMInstruction::ZextMInstruction(MachineBlock *p, MachineOperand *dst, MachineOperand *src, int cond) {
    this->parent = p;
    this->type = MachineInstruction::ZEXT;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}

void ZextMInstruction::output() {
    fprintf(yyout, "\tuxtb ");
    def_list[0]->output();
    fprintf(yyout, ", ");
    use_list[0]->output();
    fprintf(yyout, "\n");
}

/*============================================================================*/
MachineFunction::MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr) 
{ 
    this->parent = p; 
    this->sym_ptr = sym_ptr; 
    this->stack_size = 0;
};

void MachineBlock::insertBefore(MachineInstruction* at, MachineInstruction* src)
{
    std::vector<MachineInstruction*>::iterator pos = find(inst_list.begin(), inst_list.end(), at);
    inst_list.insert(pos, src);
}

void MachineBlock::insertAfter(MachineInstruction* at, MachineInstruction* src)
{
    std::vector<MachineInstruction*>::iterator pos = find(inst_list.begin(), inst_list.end(), at);
    // 如果是最后一条
    if(pos == inst_list.end())
        inst_list.push_back(src);
    else
        inst_list.insert(pos+1, src);
}


void MachineBlock::output()
{
    fprintf(yyout, ".L%d:\n", this->no);
    for(auto iter : inst_list){
        if(iter)
            iter->output();
    }
}

void MachineFunction::output()
{
    const char *func_name = this->sym_ptr->toStr().c_str() + 1;
    fprintf(yyout, "\t.global %s\n", func_name);
    fprintf(yyout, "\t.type %s , %%function\n", func_name);
    fprintf(yyout, "%s:\n", func_name);
    // TODO
    /* Hint:
    *  1. Save fp
    *  2. fp = sp
    *  3. Save callee saved register
    *  4. Allocate stack space for local variable */
    fprintf(yyout, "\tpush {fp, lr}\n");
    fprintf(yyout, "\tmov fp, sp\n");
    size_t cnt = 0;
    if(!saved_regs.empty()){
        fprintf(yyout, "\tpush {");
        for (auto &&i : saved_regs)
        {
            fprintf(yyout, "r%d", i);
            if(cnt != saved_regs.size() - 1)
                fprintf(yyout, ", ");
            cnt++;
        }
        fprintf(yyout, "}\n");
    }
    if(stack_size!=0){
        if(stack_size > 255) {
            fprintf(yyout, "\tldr r4,=%d\n", stack_size);
            fprintf(yyout, "\tsub sp, sp, r4\n");
        }
        else {
            fprintf(yyout, "\tsub sp, sp, #%d\n", stack_size);
        }
    }
    // Traverse all the block in block_list to print assembly code.
    for(auto iter : block_list)
        iter->output();

    fprintf(yyout, ".L%s_END:\n", func_name);
    if(stack_size!=0){
        if(stack_size > 255) {
            fprintf(yyout, "\tldr r4,=%d\n", stack_size);
            fprintf(yyout, "\tadd sp, sp, r4\n");
        }
        else {
            fprintf(yyout, "\tadd sp, sp, #%d\n", stack_size);
        }
    }
    //恢复saved registers和fp
    fprintf(yyout, "\tpop {");
    // cnt = 0;
    if(!saved_regs.empty()){
        for (auto &&i : saved_regs)
        {
            fprintf(yyout, "r%d", i);
            // if(cnt != saved_regs.size() - 1)
                fprintf(yyout, ", ");
            // cnt++;
        }
    }
    fprintf(yyout, "fp, lr}\n");
    // 3. Generate bx instruction
    fprintf(yyout, "\tbx lr\n\n");
}

void MachineUnit::PrintGlobalDecl()
{
    // TODO:
    // You need to print global variable/const declarition code;
    if(!unit->getGlbIds().empty())
        fprintf(yyout, "\t.data\n");
    for(IdentifierSymbolEntry* var : unit->getGlbIds()) {
        if(var->getType()->isArrayType()) {
            if(((ArrayType*)var->getType())->getCntEleNum() == 0) {
                fprintf(yyout, "\t.comm\t%s, 4, 4\n", var->toAsmStr().c_str());
            }
            else {
                fprintf(yyout, "\t.global %s\n", var->toAsmStr().c_str());
                fprintf(yyout, "\t.align 4\n");
                fprintf(yyout,"\t.size %s, 4\n", var->toAsmStr().c_str());
                fprintf(yyout,"%s:\n", var->toAsmStr().c_str());
                // TODO: 待将所有的数组元素初始值保存下来后填充
                // if(((ArrayType*)var->getType())->getElementType()->isInt()) {
                //     for (auto value: var->arrayValues) {
                //         fprintf(yyout, "\t.word %d\n", int(value));
                //     }
                // }
                // else {
                //     ;
                // }
            }
        }
        else {
            if(!var->getGlbValue()){
                fprintf(yyout, "\t.comm\t%s, 4, 4\n", var->toAsmStr().c_str());
            }else{
                fprintf(yyout, "\t.global %s\n", var->toAsmStr().c_str());
                fprintf(yyout, "\t.align 4\n");
                fprintf(yyout,"\t.size %s, 4\n", var->toAsmStr().c_str());
                fprintf(yyout,"%s:\n", var->toAsmStr().c_str());
                if(var->getGlbValue()){
                    if(var->getType()->isInt()) {
                        fprintf(yyout, "\t.word %s\n", var->getGlbValue()->toStr().c_str());
                    } else {
                        ;
                    }
                }
            }
        }
    }
}

void MachineUnit::output()
{
    // TODO
    /* Hint:
    * 1. You need to print global variable/const declarition code;
    * 2. Traverse all the function in func_list to print assembly code;
    * 3. Don't forget print bridge label at the end of assembly code!! */
    fprintf(yyout, "\t.cpu cortex-a72\n");
    fprintf(yyout, "\t.arch armv8-a\n");
    fprintf(yyout, "\t.fpu vfpv3-d16\n");
    fprintf(yyout, "\t.arch_extension crc\n");
    PrintGlobalDecl();
    fprintf(yyout, "\t.text\n");
    for(auto iter : func_list)
        iter->output();
    PrintBridges();
}

void MachineUnit::PrintBridges(){
    for (auto sym_ptr: unit->getGlbIds()) {
        fprintf(yyout, "addr_%s:\n", sym_ptr->toAsmStr().c_str());
        fprintf(yyout, "\t.word %s\n", sym_ptr->toAsmStr().c_str());
    }
}
