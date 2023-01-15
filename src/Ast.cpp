#include "Ast.h"
#include "SymbolTable.h"
#include "Unit.h"
#include "Instruction.h"
#include "IRBuilder.h"
#include <string>
#include <stack>
#include "Type.h"

extern FILE *yyout;
int Node::counter = 0;
IRBuilder* Node::builder = nullptr;

struct flags{
    bool exprShouldCheck = true;
    Type* shouldReturn = nullptr;
    bool haveReturn = false;
    bool isInFunc = false;
    int isInWhileCnt = 0;
    
    //break和continue需要获得当前层次whileStmt的Cond_bb和End_bb
    std::stack<BasicBlock*> whileCondStack;
    std::stack<BasicBlock*> whileEndStack;

    //是否是最后一层条件表达式
    bool isOuterCond = false;
    //运算是否处于条件表达式中
    bool isUnderCond = false;

    std::stack<int> cntEle;

    std::stringstream arrayDefString;
    // IdentifierSymbolEntry* arrayId;
    std::stack<IdentifierSymbolEntry*> arrayIdStk;
    std::stack<std::vector<ArrayDef*>::iterator> arrDefIterStk;
    std::vector<ExprNode*>::iterator dimListIter;
    bool isOuterArrDecl = false;
    
    int cntParam = 0;
} flag;

//q13添加数组IR支持
/*
    method
    -1 若栈顶为空，返回nullptr
    0 栈顶是end则把自己弹栈，把栈顶+1，返回第-1步
    1 本身是不是指向一个有expr的block？如果是的话返回他，迭代器自增
        如果不是的话就压入下一层的头迭代器，返回第0步
*/
ExprNode* getNextExprInArrDef(){
    while(1){
        if(flag.arrDefIterStk.empty())
            return nullptr;
        auto& top = flag.arrDefIterStk.top();
        // if(top.it == top.vec.end()){
        if(!(*top.base())){
            flag.arrDefIterStk.pop();
            if(!flag.arrDefIterStk.empty()){
                flag.arrDefIterStk.top()++;
            }
            continue;
        }
        auto e = (*top.base())->getLeaf();
        if(e){
            top++;
            return e;
        }else{
            auto& list = (*top.base())->getDefList();
            flag.arrDefIterStk.push(list.begin());
        }
    }
}

/*
    method
    idx是第几维，从0开始
    对于一棵满树，循环部分会按照它的理想层次结构遍历这棵树，
    即使arrDefList所构成的树在内存里未必是这个样子，但是getNextExprInArrDef()都能
    准确的返回内存中树的下一个没有被打印过的值，于是我们能够打印出理想的满树对应的字符串
*/
void getArrayDefStr(int idx, bool checkTop = false){
    auto dims = ((ArrayType*)flag.arrayIdStk.top()->getType())->getDimList();
    auto p = ((ArrayType*)flag.arrayIdStk.top()->getType());
    if(idx == dims.size()){
        auto& top = flag.arrDefIterStk.top();
        ExprNode* e;
        if(checkTop && !(*top.base())){   //这样就可以补零了
            flag.arrayDefString<<"0";
            e = new Constant(new ConstantSymbolEntry(p->getElementType(), 0));
        }else{
            e = getNextExprInArrDef();
            flag.arrayDefString<<((ConstantSymbolEntry*)e->getSymbolEntry())->genStr(p->getElementType());
            // flag.arrayIdStk.top()->addArrExpr(e);
        }
        flag.arrayIdStk.top()->addArrExpr(e);
        return;
    }
    flag.arrayDefString<<"[";
    int dimNum = ((ConstantSymbolEntry*)dims[idx]->getSymbolEntry())->getValueInt();
    for (size_t i = 0; i < dimNum; i++)
    {
        flag.arrayDefString<<p->getDimTypeStrings()[idx]<<" ";
        getArrayDefStr(idx + 1, i != 0);
        if(i != dimNum - 1)
            flag.arrayDefString<<",";
    }
    flag.arrayDefString<<"]";
}

/*void DimArray::genCode() {

    BasicBlock *bb = builder->getInsertBB();

    auto trim = ((ArrayType*)flag.arrayId->getType())->getTrimType();
    auto type = new PointerType(trim);//这里应该剥壳一层
    auto addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
    auto addr = new Operand(addr_se); 
    Operand* lastAddr;
    for (size_t i = 0; i < dimList.size(); i++)
    {
        dimList[i]->genCode();
        new GetElementPtrInstruction(addr, i == 0 ? flag.arrayId->getAddr() : lastAddr, dimList[i]->getOperand(), bb);
        lastAddr = addr;
        if(i == dimList.size() - 1){
            dst = lastAddr;
            return;
        }
        trim = ((ArrayType*)trim)->getTrimType();
        type = new PointerType(trim);     //这里应该剥壳一层
        addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        addr = new Operand(addr_se); 
    }
    dst = lastAddr;
}*/

void ArrayDef::getArrayDefCode(int idx, Operand* defOp, Type* toTrim, bool checkTop){
    BasicBlock *bb = builder->getInsertBB();
    auto dims = ((ArrayType*)flag.arrayIdStk.top()->getType())->getDimList();
    auto p = ((ArrayType*)flag.arrayIdStk.top()->getType());
    if(idx == dims.size()){
        auto& top = flag.arrDefIterStk.top();
        if(checkTop && !(*top.base())){   //这样就可以补零了
            // flag.arrayDefString<<"0";
            // auto e = new Constant(new ConstantSymbolEntry(p->getElementType(), 0));
            ;
        }else{
            auto e = getNextExprInArrDef();
            e->genCode();
            new StoreInstruction(defOp, e->getOperand(), bb);
        }
        return;
    }
    int dimNum = ((ConstantSymbolEntry*)dims[idx]->getSymbolEntry())->getValueInt();
    auto trim = ((ArrayType*)toTrim)->getTrimType();
    auto type = new PointerType(trim);//这里应该剥壳一层
    for (size_t i = 0; i < dimNum; i++)
    {
        auto addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        auto addr = new Operand(addr_se); 
        auto dimOp = new Operand(new ConstantSymbolEntry(TypeSystem::longIntType, (int)i));
        new GetElementPtrInstruction(addr, defOp, dimOp, bb, flag.arrayIdStk.top());
        arrDstVec.push_back(addr);
        getArrayDefCode(idx + 1, arrDstVec.back(), trim, i != 0);
    }
}

/*============================AST NODE FUNCS=============================================*/
Node::Node()
{
    seq = counter++;
}

void Node::backPatch(std::vector<Instruction*> &list, BasicBlock*bb, bool setTrueBr)
{
    for(auto &inst:list)
    {
        if(inst->isCond()){
            if(setTrueBr)
                dynamic_cast<CondBrInstruction*>(inst)->setTrueBranch(bb);
            else
                dynamic_cast<CondBrInstruction*>(inst)->setFalseBranch(bb);
        }
        else if(inst->isUncond())
            dynamic_cast<UncondBrInstruction*>(inst)->setBranch(bb);
    }
}

Operand *Node::typeConvention(Type* target, Operand * toConvert, BasicBlock*bb){
    Operand* n;
    if(target->getKind() == toConvert->getType()->getKind()){
        return toConvert;
    }

    if(target->isBool() && (toConvert->getType()->isInt() || toConvert->getType()->isFloat())){
        // std::cout<<"警告！int转换为bool将损失精度！"<<std::endl;
        auto se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        n = new Operand(se);
        Operand* src1;
        if(toConvert->getType()->isInt())
            src1 = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0));
        else
            src1 = new Operand(new ConstantSymbolEntry(TypeSystem::floatType, 0));
        new CmpInstruction(CmpInstruction::NE, n, src1, toConvert, bb);
    }else if(target->isInt() && toConvert->getType()->isBool()){
        auto se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        n = new Operand(se);
        new ZextInstruction(n, toConvert, bb);
    }else if(target->isFloat() && toConvert->getType()->isInt()){
        auto se = new TemporarySymbolEntry(TypeSystem::floatType, SymbolTable::getLabel());
        n = new Operand(se);
        new IntFloatCastInstruction(IntFloatCastInstruction::I2F, n, toConvert, bb);
    }else if(target->isInt() && toConvert->getType()->isFloat()){
        auto se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        n = new Operand(se);
        new IntFloatCastInstruction(IntFloatCastInstruction::F2I, n, toConvert, bb);
    }
    return n;
}

void Node::typeConsist(Operand** op1, Operand** op2, BasicBlock*bb, bool wider){
    if(!wider){
        // std::cout<<"警告！int转换为bool将损失精度！"<<std::endl;
        if((*op1)->getType()->isInt() && (*op2)->getType()->isBool()){
            *op1 = typeConvention(TypeSystem::boolType, *op1, bb);
            return;
        }
        if((*op1)->getType()->isBool() && (*op2)->getType()->isInt()){
            *op2 = typeConvention(TypeSystem::boolType, *op2, bb);
            return;
        }
    }else{
        if((*op1)->getType()->isInt() && (*op2)->getType()->isBool()){
            *op2 = typeConvention(TypeSystem::intType, *op2, bb);
            return;
        }
        if((*op1)->getType()->isBool() && (*op2)->getType()->isInt()){
            *op1 = typeConvention(TypeSystem::intType, *op1, bb);
            return;
        }
        if((*op1)->getType()->isFloat() && !(*op2)->getType()->isFloat()){
            *op2 = typeConvention(TypeSystem::floatType, *op2, bb);
            return;
        }
        if((*op2)->getType()->isFloat() && !(*op1)->getType()->isFloat()){
            *op1 = typeConvention(TypeSystem::floatType, *op1, bb);
            return;
        }
    }
}

std::vector<Instruction*> Node::merge(std::vector<Instruction*> &list1, std::vector<Instruction*> &list2)
{
    std::vector<Instruction*> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void Ast::genCode(Unit *unit)
{
    IRBuilder *builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    root->genCode();
}

void FunctionDef::genCode()
{
    Unit *unit = builder->getUnit();
    Function *func = new Function(unit, se, this);
    BasicBlock *entry = func->getEntry();
    // set the insert point to the entry basicblock of this function.
    builder->setInsertBB(entry);

    flag.shouldReturn = ((FunctionType*)(se->getType()))->getRetType();

    if(params!=nullptr){
        params->genCode();
    }
    stmt->genCode();
    //加入的ret void
    if(this->voidAddRet!=nullptr)
    {
        this->voidAddRet->genCode();
    }
    //q4为function加入exit块
    // 为返回地址分配储存空间，不应该在return处，因为应该一个函数只执行一遍
    if(retAddr){
        Instruction *alloca;
        SymbolEntry *addr_se;
        addr_se = new TemporarySymbolEntry(((FunctionType*)se->getType())->getReturnType(), SymbolTable::getLabel());
        alloca = new AllocaInstruction(retAddr, addr_se); 
        entry->insertFront(alloca);                      
    }
    /**
     * Construct control flow graph. You need do set successors and predecessors for each basic block.
     * Todo
    */

    std::vector<BasicBlock*> toExit;
    std::vector<Operand*> isConstVec;
   //q3function连接基本块
   auto bl = func->getBlockList();
   for (auto &&i : bl)
   {
        //去除ret后面还有指令的情况
        //如果要增加end block就要把return语句也去除
        bool shouldErase = false;
        for(auto j = i->begin(); j != i->end(); j = j->getNext()){
            if(j->isRet()){
                shouldErase = true;
                //返回常量的特殊情况
                if(!this->voidAddRet && j->getOperands()[0]->getSymbolEntry()->isConstant()){
                    // new StoreInstruction(retAddr, j->getOperands()[0], i);
                    isConstVec.push_back(j->getOperands()[0]);
                }else{
                    isConstVec.push_back(nullptr);
                }
            }
            if(shouldErase){
                i->remove(j);
            }
        }
        // 增加一个需要和end block连接起来的块
        if(shouldErase){
            toExit.push_back(i);
        }

        auto lastInst = i->rbegin();
        if(lastInst->isUncond()){
            auto toBB = ((UncondBrInstruction*)lastInst)->getBranch();
            i->addSucc(toBB);
            toBB->addPred(i);
        }else if(lastInst->isCond()){
            auto toBB_t = ((CondBrInstruction*)lastInst)->getTrueBranch();
            auto toBB_f = ((CondBrInstruction*)lastInst)->getFalseBranch();
            i->addSucc(toBB_t);
            i->addSucc(toBB_f);
            toBB_t->addPred(i);
            toBB_f->addPred(i);
        }else if(lastInst->isRet()){}
   }
   
    BasicBlock *exit = func->getExit();
    builder->setInsertBB(exit);
   //q4为function加入exit块
   int cnt = 0;
   for (auto &&i : toExit)
   {
        if(retAddr){
            if(!isConstVec[cnt]){
                //向原先有return的地方添加将结果保存到retAddr的指令
                new StoreInstruction(retAddr, i->rbegin()->getOperands()[0], i);
            }else{
                new StoreInstruction(retAddr, isConstVec[cnt], i);
            }
        }
        exit->addPred(i);
        i->addSucc(exit);
        // 一个基本块结束后面一个不会自动执行
        new UncondBrInstruction(exit, i);
        cnt++;
   }

   if(retAddr){
        // 向exit block中添加指令
        auto addr_se = new TemporarySymbolEntry(((FunctionType*)se->getType())->getReturnType(), SymbolTable::getLabel());
        auto retOp = new Operand(addr_se);
        new LoadInstruction(retOp, retAddr, exit);
        new RetInstruction(retOp, exit);
   }else
   {
        new RetInstruction(nullptr, exit);
   }
   
}

void BinaryExpr::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    if (op == LOGIC_AND)
    {
        BasicBlock *trueBB = new BasicBlock(func);  // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();
        backPatch(expr1->trueList(), trueBB);
        builder->setInsertBB(trueBB);               // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        true_list = expr2->trueList();
        false_list = merge(expr1->falseList(), expr2->falseList());
    }
    else if(op == LOGIC_OR)
    {
        //q7if语句
        BasicBlock *falseBB = new BasicBlock(func);  // if the result of lhs is true, jump to the falseBB.
        expr1->genCode();
        backPatch(expr1->falseList(), falseBB);
        builder->setInsertBB(falseBB);              
        expr2->genCode();
        false_list = expr2->falseList();
        true_list = merge(expr1->trueList(), expr2->trueList());
    }
    else if(op >= LESS && op <= NOT_EQUAL_TO)
    {
        //q7if语句
        // 为了防止isOuterCond为真时引起ID也添加分支基本块导致错误
        bool tempOuter = flag.isOuterCond;
        if(flag.isOuterCond){
            flag.isOuterCond = false;
        }
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case LESS:
            opcode = CmpInstruction::L;
            break;
        case LESS_EQUAL:
            opcode = CmpInstruction::LE;
            break;
        case GREATER:
            opcode = CmpInstruction::G;
            break;
        case GREATER_EQUAL:
            opcode = CmpInstruction::GE;
            break;
        case EQUAL_TO:
            opcode = CmpInstruction::E;
            break;
        case NOT_EQUAL_TO:
            opcode = CmpInstruction::NE;
            break;
        }

        typeConsist(&src1, &src2, bb, true);

        new CmpInstruction(opcode, dst, src1, src2, bb);
        if(tempOuter){
            BasicBlock *falseBlock;
            falseBlock = new BasicBlock(func);
            true_list.push_back(new CondBrInstruction(nullptr, falseBlock, dst, bb));
            // without it break for the same reason but found at toBB_f->addPred(i);
            false_list.push_back(new UncondBrInstruction(nullptr, falseBlock)); // when && break at a CondBrInstruction miss false_branch found at output and none of block end with CondBrInstruction
            flag.isOuterCond = true;
        }
    }
    else if(op >= ADD && op <= REMAINDER)
    {
        bool tempOuter = flag.isOuterCond;
        if(flag.isOuterCond){
            flag.isOuterCond = false;
        }
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case ADD:
            opcode = BinaryInstruction::ADD;
            break;
        case SUB:
            opcode = BinaryInstruction::SUB;
            break;
        case PRODUCT:
            opcode = BinaryInstruction::MUL;
            break;
        case DIVISION:
            opcode = BinaryInstruction::DIV;
            break;
        case REMAINDER:
            opcode = BinaryInstruction::MOD;
            break;
        }

        typeConsist(&src1, &src2, bb, true);

        new BinaryInstruction(opcode, dst, src1, src2, bb);

        if(flag.isUnderCond && tempOuter){
            BasicBlock *falseBlock;
            falseBlock = new BasicBlock(func);
            auto ret = typeConvention(TypeSystem::boolType, dst, bb);
            true_list.push_back(new CondBrInstruction(nullptr, falseBlock, ret, bb));
            // without it break for the same reason but found at toBB_f->addPred(i);
            false_list.push_back(new UncondBrInstruction(nullptr, falseBlock)); // when && break at a CondBrInstruction miss false_branch found at output and none of block end with CondBrInstruction
            // flag.isOuterCond = true;
        }
    }
}

void Constant::genCode()
{
    // we don't need to generate code.
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    // 不是最外层的const是不能调用的
    if(flag.isUnderCond && flag.isOuterCond){
        BasicBlock *falseBlock;
        falseBlock = new BasicBlock(func);
        auto ret = typeConvention(TypeSystem::boolType, dst, bb);
        true_list.push_back(new CondBrInstruction(nullptr, falseBlock, ret, bb));
        // without it break for the same reason but found at toBB_f->addPred(i);
        false_list.push_back(new UncondBrInstruction(nullptr, falseBlock)); // when && break at a CondBrInstruction miss false_branch found at output and none of block end with CondBrInstruction
    }
}

void Id::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    // 现在数组的Operand并不是数组类型而是指针类型
    if(!((IdentifierSymbolEntry*)symbolEntry)->getType()->isArrayType())
        new LoadInstruction(dst, addr, bb);
    else{
        // 为函数传入数组的情况
        auto dim = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0));
        // auto se = new IdentifierSymbolEntry(*(IdentifierSymbolEntry*)symbolEntry);
        // se->setType(new PointerType(((ArrayType*)(symbolEntry->getType()))->getElementType()));
        auto p = new PointerType(((ArrayType*)(symbolEntry->getType()))->getElementType()); // 不要直接把new的对象写在实参里
        auto se = new TemporarySymbolEntry(p, SymbolTable::getLabel());
        dst = new Operand(se);
        new GetElementPtrInstruction(dst, addr, dim, bb, (IdentifierSymbolEntry*)symbolEntry);
    }

    // 不是最外层的ID是不能调用的，因为比如a==5是不行的
    if(flag.isUnderCond && flag.isOuterCond){
        BasicBlock *falseBlock;
        falseBlock = new BasicBlock(func);
        auto ret = typeConvention(TypeSystem::boolType, dst, bb);
        true_list.push_back(new CondBrInstruction(nullptr, falseBlock, ret, bb));
        // without it break for the same reason but found at toBB_f->addPred(i);
        false_list.push_back(new UncondBrInstruction(nullptr, falseBlock)); // when && break at a CondBrInstruction miss false_branch found at output and none of block end with CondBrInstruction
    }
}

void IfStmt::genCode()
{
    Function *func;
    BasicBlock *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    flag.isUnderCond = true;
    flag.isOuterCond = true;
    cond->genCode();
    flag.isOuterCond = false;
    flag.isUnderCond = false;
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(end_bb);
}

void IfElseStmt::genCode()
{
    //q8ifelse语句
    Function *func;
    BasicBlock *then_bb, *else_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    else_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    flag.isUnderCond = true;
    flag.isOuterCond = true;
    cond->genCode();
    flag.isOuterCond = false;
    flag.isUnderCond = false;
    backPatch(cond->trueList(), then_bb);
    // backPatch(cond->trueList(), end_bb, false);
    backPatch(cond->falseList(), else_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(else_bb);
    elseStmt->genCode();
    else_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, else_bb);

    builder->setInsertBB(end_bb);
}

void CompoundStmt::genCode()
{
    // Todo
    //q2补全代码生成调用链
    if(stmt)
        stmt->genCode();
}

void SeqNode::genCode()
{
    // Todo
    //q2补全代码生成调用链
    for (auto &&i : stmtList)
    {
        i->genCode();
    }
    
}

void DeclStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    //配合idList
    int cnt = 0;
    for (const auto id:idList){
        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymPtr());
        // 保证后续关于数组的判断可以正常进行
        if(se->getType()->isArrayType()){
            se->setArray();
            se->setArrayType(((ArrayType*)se->getType()));
        }
        if(se->isGlobal())
        {
            Operand *addr;
            SymbolEntry *addr_se;
            addr_se = new IdentifierSymbolEntry(*se);
            if(se->getType()->isArrayType())
                addr_se->setType(new PointerType(se->getType(), false, true));
            else
                addr_se->setType(new PointerType(se->getType()));
            addr = new Operand(addr_se);
            se->setAddr(addr);
            //q6在全局区添加系统函数声明和全局变量
            if(exprList[cnt]){
                // 运算出现在全局区
                if(bb){
                    // continue;    
                    exprList[cnt]->genCode();
                }
                se->setGlbConst(exprList[cnt]->getSymbolEntry());
            }else if(defArrList[cnt]){
                //q13添加数组IR支持
                ((ArrayType*)idList[cnt]->getSymbolEntry()->getType())->countEleNum();
                ((ArrayType*)idList[cnt]->getSymbolEntry()->getType())->genDimTypeStrings();
                flag.arrayIdStk.push(((IdentifierSymbolEntry*)idList[cnt]->getSymbolEntry()));
                flag.dimListIter = ((ArrayType*)flag.arrayIdStk.top()->getType())->getDimList().begin();
                flag.isOuterArrDecl = true;
                defArrList[cnt]->genCode();
                builder->getUnit()->getGlbIds().push_back(flag.arrayIdStk.top());
                flag.arrayIdStk.pop();
                continue;
            }
            builder->getUnit()->getGlbIds().push_back(se);
        }
        else if(se->isLocal() || se->isParam())
        {
            Function *func = builder->getInsertBB()->getParent();
            BasicBlock *entry = func->getEntry();
            Operand *addr;
            Instruction *alloca;
            SymbolEntry *addr_se;
            Type *type;
            
            if(se->isLocal()){
                if(!se->getType()->isArrayType())
                    type = new PointerType(se->getType());
                else
                    type = new PointerType(se->getType(), false, true);
                //普通局部变量，只需要一个新的label，分配一个空间即可
                addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
                addr = new Operand(addr_se); 
                alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
            }
            if(se->isParam()){
                auto seType = se->getType();
                if(seType->isArrayType()){
                    seType = new PointerType(((ArrayType*)seType)->getElementType());   //这个地方arr不能是true，genMachineOp有错
                }
                type = new PointerType(seType);
                //形参需要一个label用于传参时写入
                // addr_se = new TemporarySymbolEntry(se->getType(), flag.cntParam);
                // addr_se = new IdentifierSymbolEntry(*se);
                // ((IdentifierSymbolEntry*)addr_se)->setParamNumber(flag.cntParam);
                se->setParamNumber(flag.cntParam);
                //形参label（变量保存位置）
                // func->addLabelParam(((TemporarySymbolEntry*)addr_se)->getLabel());
                func->addLabelParam(flag.cntParam);
                //另一个label，分配一片空间
                auto tempPtr = new TemporarySymbolEntry(type, SymbolTable::getLabel());
                addr = new Operand(tempPtr); 
                Operand* srcParam;
                if(se->getType()->isArrayType()){
                    auto seTarget = new IdentifierSymbolEntry(*se);
                    seTarget->setType(seType);
                    srcParam = new Operand(seTarget);
                    alloca = new AllocaInstruction(addr, seTarget);
                }else{
                    srcParam = new Operand(se);
                    alloca = new AllocaInstruction(addr, se);
                }
                //把形参的值存入另一个label指向的空间中
                new StoreInstruction(addr, srcParam, bb);
            }
            entry->insertFront(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
            se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.

            if(se->getType()->isArrayType()){
                //q13添加数组IR支持
                ((ArrayType*)idList[cnt]->getSymbolEntry()->getType())->countEleNum();
                ((ArrayType*)idList[cnt]->getSymbolEntry()->getType())->genDimTypeStrings();
                ((IdentifierSymbolEntry*)se)->setNameOfFunc(((IdentifierSymbolEntry*)func->getSymPtr())->toStr());
            }

            auto expr = exprList[cnt];
            if(expr){
                expr->genCode();
                Operand *src = expr->getOperand();
                src = typeConvention(se->getType(), src, bb);
                new StoreInstruction(addr, src, bb);
            }else if(defArrList[cnt]){
                flag.arrayIdStk.push(((IdentifierSymbolEntry*)idList[cnt]->getSymbolEntry()));
                flag.dimListIter = ((ArrayType*)flag.arrayIdStk.top()->getType())->getDimList().begin();
                flag.isOuterArrDecl = true;
                defArrList[cnt]->genCode();
                flag.arrayIdStk.pop();
            }
        }
        cnt++;
    }
}

void ReturnStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    // Todo
    //q4为function加入exit块
    //将要返回的语句储存起来，然后再补充load指令、ret指令最后两条指令会被提取出来放到函数的返回块中
    if(!retValue){
        //返回值为空，只插入return语句
        new RetInstruction(nullptr, bb);
    }else{
        retValue->genCode();
        // dst = retValue->getOperand();
        Operand *src = retValue->getOperand();  //获得计算结果（寄存器，三地址码中的一个）
        assert(flag.shouldReturn != nullptr);
        src = typeConvention(flag.shouldReturn, src, bb);
        // 不同return语句的addr必须一样，也就是说一个函数只有一个RetInstruction但是这个在这里实现有难度，所以我们放到function那里
        // 先把return的表达式的计算结果取出来
        // 不需要，假如是最简单的return a那么这里已经有一条load了
        // new LoadInstruction(dst, src, bb);
        // 存到一个新地址中
        // auto op = new Operand(retValue->getSymbolEntry()); 这个没用这个Operand的label是expr的、不是return的
        // 那么return究竟需要几个label?
        // new StoreInstruction(op, src, bb); 
        // new LoadInstruction(dst, op, bb);
        // 既然我们又要保证每一个生成的return语句都可用，又想到function里面一起生成end block那么这里可以用最简单的形式
        new RetInstruction(src, bb);
    }
}

void AssignStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    expr->genCode();
    Operand *addr;
    if(lval->isInArr()){
        ((ArrayIndex*)lval)->genLvalCode();
        addr = lval->getOperand();
    }else{
        addr = dynamic_cast<IdentifierSymbolEntry*>(lval->getSymPtr())->getAddr();
    }
    Operand *src = expr->getOperand();
    src = typeConvention(lval->getSymbolEntry()->getType(), src, bb);
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just store the result of the `expr` to the addr of the id.
     * If you want to implement array, you have to caculate the address first and then store the result into it.
     */
    new StoreInstruction(addr, src, bb);
}
//------------------NEW GEN-CODE--------------------
void FuncCall::genCode() {

    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    std::vector<Operand*> params;
    //q2补全代码生成调用链
    auto paramTypes = ((FunctionType*)(funcDef->getType()))->getParamsType();
    int cnt = 0;
    bool tempflag = flag.isOuterCond;
    flag.isOuterCond = false;
    for (auto &&i : arg)
    {
        i->genCode();
        Operand* dst = i->getOperand();
        if(dst->getType()->isNumber())
            dst = typeConvention(paramTypes[cnt], dst, bb);
        params.push_back(dst);
        cnt++;
    }
    flag.isOuterCond = tempflag;
    //q5FunctionCall的代码生成
    new FunctionCallInstuction(dst, params, funcDef, bb);
    
    if(flag.isUnderCond && flag.isOuterCond){
        BasicBlock *falseBlock;
        falseBlock = new BasicBlock(func);
        auto ret = typeConvention(TypeSystem::boolType, dst, bb);
        true_list.push_back(new CondBrInstruction(nullptr, falseBlock, ret, bb));
        // without it break for the same reason but found at toBB_f->addPred(i);
        false_list.push_back(new UncondBrInstruction(nullptr, falseBlock)); // when && break at a CondBrInstruction miss false_branch found at output and none of block end with CondBrInstruction
        // flag.isOuterCond = true;
    }
}

void UnaryExpr::genCode() {
    
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    if (op == SUB && !flag.isOuterCond)
    {
        expr->genCode();
        Operand *src1;
        Operand *src2;
        if(expr->getOperand()->getType()->isBool()) {
            src1 = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0));
            src2 = expr->getOperand();
            typeConsist(&src1, &src2, bb, true);
            int opcode = BinaryInstruction::SUB;
            new BinaryInstruction(opcode, dst, src1, src2, bb);
        }
        else if(expr->getOperand()->getType()->isInt()){
            src1 = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0));
            src2 = expr->getOperand();
            // typeConsist(&src1, &src2, bb, true);
            int opcode = BinaryInstruction::SUB;
            new BinaryInstruction(opcode, dst, src1, src2, bb);
        }
        else if(expr->getOperand()->getType()->isFloat()) {
            src1 = new Operand(new ConstantSymbolEntry(TypeSystem::floatType, 0));
            src2 = expr->getOperand();
            int opcode = BinaryInstruction::SUB;
            new BinaryInstruction(opcode, dst, src1, src2, bb);
        }
    }
    //q10单目运算作为条件语句
    // 在条件判断的情况下忽略取负值
    else if(op == LOGIC_NOT || flag.isOuterCond)
    {
        bool tempOuter = flag.isOuterCond;
        if(flag.isOuterCond){
            flag.isOuterCond = false;
        }

        expr->genCode();
        //TODO：在为负号时不应该执行，但是加上if会导致返回地址出错
        Operand *src1;
        Operand *src2 = expr->getOperand();
        if(op == LOGIC_NOT){
            if(flag.isUnderCond){
                if(src2->getType()->isInt() || src2->getType()->isFloat())
                    src2 = typeConvention(TypeSystem::boolType, src2, bb);
                src1 = new Operand(new ConstantSymbolEntry(TypeSystem::boolType, 0));
            }else{
                if(src2->getType()->isInt())
                    src1 = new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0));
                else
                    src1 = new Operand(new ConstantSymbolEntry(TypeSystem::floatType, 0));
            }
            //typeConsist(&dst, &src2, bb);
            new CmpInstruction(CmpInstruction::E, dst, src1, src2, bb);
            // 结果类型必须强制转换为bool
            dst->getSymbolEntry()->setType(TypeSystem::boolType);
        }else{
            if(src2->getType()->isInt() || src2->getType()->isFloat())
                src2 = typeConvention(TypeSystem::boolType, src2, bb);
            src1 = new Operand(new ConstantSymbolEntry(TypeSystem::boolType, 1));
            //跟1比较，不会改变原值，以此插入一条无效指令使返回地址匹配
            new CmpInstruction(CmpInstruction::E, dst, src1, src2, bb);
        }
        
        if(tempOuter){
            BasicBlock *falseBlock;
            falseBlock = new BasicBlock(func);
            true_list.push_back(new CondBrInstruction(nullptr, falseBlock, dst, bb));
            // without it break for the same reason but found at toBB_f->addPred(i);
            false_list.push_back(new UncondBrInstruction(nullptr, falseBlock)); // when && break at a CondBrInstruction miss false_branch found at output and none of block end with CondBrInstruction
            flag.isOuterCond = true;
        }
    }
}

void DimArray::genCode() {

    BasicBlock *bb = builder->getInsertBB();

    auto trim = ((ArrayType*)flag.arrayIdStk.top()->getType())->getTrimType();
    auto type = new PointerType(trim, false, true);//这里应该剥壳一层
    auto addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
    auto addr = new Operand(addr_se); 
    
    Operand* lastAddr;
    for (size_t i = 0; i < dimList.size(); i++)
    {
        dimList[i]->genCode();
        if(i==0){
            auto p = flag.arrayIdStk.top()->getAddr();
            Operand* target;
            auto pType = ((PointerType*)p->getType());
            if(pType->isMultiPtr()){
                auto pTempSe = new TemporarySymbolEntry(pType->getValueType(), SymbolTable::getLabel());
                target = new Operand(pTempSe); 
                new LoadInstruction(target, p, bb);
            }else{
                target = p;
            }
            new GetElementPtrInstruction(addr, target, dimList[i]->getOperand(), bb, flag.arrayIdStk.top());
        }else{
            new GetElementPtrInstruction(addr, lastAddr, dimList[i]->getOperand(), bb, flag.arrayIdStk.top());
        }
        lastAddr = addr;
        if(i == dimList.size() - 1){
            dst = lastAddr;
            return;
        }
        trim = ((ArrayType*)trim)->getTrimType();
        type = new PointerType(trim, false, true);     //这里应该剥壳一层
        addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        addr = new Operand(addr_se); 
    }
    dst = lastAddr;
    
    
}

void ArrayDef::genCode() {
    BasicBlock *bb = builder->getInsertBB();

    int cnt = 0;
    if(flag.isOuterArrDecl){
        flag.isOuterArrDecl = false;
        if(!arrDefList.empty() && isAllDefined(cnt)){
            flag.arrDefIterStk.push(arrDefList.begin());
            getArrayDefStr(0);
            flag.arrayIdStk.top()->setArrDefStr(flag.arrayDefString.str());
            flag.arrayDefString.clear();    // 清空流
            flag.arrayDefString.str("");
            std::stack<std::vector<ArrayDef*>::iterator>().swap(flag.arrDefIterStk);
            if(flag.arrayIdStk.top()->isLocal())
                builder->getUnit()->getGlbIds().push_back(flag.arrayIdStk.top());
            // 拷贝到栈区中
            // 在genMachineOperand的时候无法区分的根本原因在于，Pointer其实有两种，一种是指向栈上空间的，一种是指全局区的
            auto tmpEntryStk = new TemporarySymbolEntry(new PointerType(TypeSystem::shortIntType, false, true), SymbolTable::getLabel());
            auto opStk = new Operand(tmpEntryStk);
            new BitCastInstruction(opStk, flag.arrayIdStk.top()->getAddr(), bb);    // src的类型是array
            auto tmpEntryGlb = new TemporarySymbolEntry(new PointerType(TypeSystem::shortIntType, false, true), SymbolTable::getLabel());
            auto opGlb = new Operand(tmpEntryGlb);
            // ((PointerType*)flag.arrayIdStk.top()->getType())->setGlobal();  // 可能导致崩溃
            auto name = new IdentifierSymbolEntry(*flag.arrayIdStk.top());
            name->setType(new PointerType(name->getType(), true, true));    // src的类型是pointer
            auto opName = new Operand(name);
            new BitCastInstruction(opGlb, opName, bb);
            //调用memcpy函数
            std::vector<Operand*> ops;
            ops.push_back(opStk);
            ops.push_back(opGlb);
            auto c2 = new ConstantSymbolEntry(TypeSystem::longIntType, 32);
            auto op2 = new Operand(c2);
            ops.push_back(op2);
            auto c3 = new ConstantSymbolEntry(TypeSystem::boolType, 0);
            auto op3 = new Operand(c3);
            ops.push_back(op3);
            std::vector<Type*> types{new PointerType(TypeSystem::shortIntType), new PointerType(TypeSystem::shortIntType), TypeSystem::longIntType, TypeSystem::boolType};
            auto toCall = identifiers->lookup("llvm.memcpy.p0i8.p0i8.i64", types);
            auto tmpDst = new TemporarySymbolEntry(TypeSystem::voidType, SymbolTable::getLabel());
            auto opDst = new Operand(tmpDst);
            new FunctionCallInstuction(opDst, ops, (IdentifierSymbolEntry*)toCall, bb);

        }else if(arrDefList.empty()){
            auto tmpEntry = new TemporarySymbolEntry(new PointerType(TypeSystem::shortIntType, false, true), SymbolTable::getLabel());
            auto op = new Operand(tmpEntry);
            new BitCastInstruction(op, flag.arrayIdStk.top()->getAddr(), bb);
            std::vector<Operand*> ops;
            ops.push_back(op);
            auto c1 = new ConstantSymbolEntry(TypeSystem::shortIntType, 0);
            auto op1 = new Operand(c1);
            ops.push_back(op1);
            auto c2 = new ConstantSymbolEntry(TypeSystem::longIntType, 32);
            auto op2 = new Operand(c2);
            ops.push_back(op2);
            auto c3 = new ConstantSymbolEntry(TypeSystem::boolType, 0);
            auto op3 = new Operand(c3);
            ops.push_back(op3);
            std::vector<Type*> types{new PointerType(TypeSystem::shortIntType), TypeSystem::shortIntType, TypeSystem::longIntType, TypeSystem::boolType};
            auto toCall = identifiers->lookup("llvm.memset.p0i8.i64", types);
            auto tmpDst = new TemporarySymbolEntry(TypeSystem::voidType, SymbolTable::getLabel());
            auto opDst = new Operand(tmpDst);
            new FunctionCallInstuction(opDst, ops, (IdentifierSymbolEntry*)toCall, bb);
        }else{
            flag.arrDefIterStk.push(arrDefList.begin());
            getArrayDefCode(0, flag.arrayIdStk.top()->getAddr(), (ArrayType*)flag.arrayIdStk.top()->getType());
            std::stack<std::vector<ArrayDef*>::iterator>().swap(flag.arrDefIterStk);
        }
    }else{
        if(expr)
            expr->genCode();
        for (auto &&i : arrDefList)
        {
            i->genCode();
        }
    }
    
}

bool ArrayDef::isAllDefined(int& cnt){
    if(cnt == ((ArrayType*)flag.arrayIdStk.top()->getType())->getCntEleNum())
        return true;
    if(expr)
        return false;
    bool temp = false;
    bool isLastDim = false;
    for (auto &&i : arrDefList)
    {
        if(i->expr && i->expr->getSymPtr()->isConstant()){
            cnt++;
            isLastDim = true;
        }
        flag.dimListIter++;
        temp = temp ? true : i->isAllDefined(cnt);
        flag.dimListIter--;
    }
    if(isLastDim){
        int dimSize = ((ConstantSymbolEntry*)((*flag.dimListIter)->getSymPtr()))->getValueInt();
        if (arrDefList.size() < dimSize)
            cnt += dimSize - arrDefList.size();
    }
    return temp;
}

// void ArrayDef::getArrayDefStr(int idx){
//     auto dims = ((ArrayType*)flag.arrayId->getType())->getDimList();
//     auto p = ((ArrayType*)flag.arrayId->getType());
//     if(expr){
//         flag.arrayDefString<<expr->getSymbolEntry()->toStr();
//         return;
//     }
//     for (size_t i = 0; i < ((ConstantSymbolEntry*)dims[idx]->getSymbolEntry())->getValueInt(); i++)
//     {
//         if(!arrDefList.empty()){
//             flag.arrayDefString<<p->getDimTypeStrings()[idx];
//             arrDefList[i]->getArrayDefStr(idx + 1);
//         }
//         // else
//         //     flag.arrayDefString<<expr->getSymbolEntry()->toStr();
//     }
    
// }

// void ArrayDef::getArrayDefStr(int& idx){
//     auto p = ((ArrayType*)flag.arrayId->getType());
//     // if(idx == 0 && !expr && arrDefList.size() == 1){
//     //     arrDefList[0]->getArrayDefStr(idx);
//     //     return;
//     // }
//     if(idx == p->getDim())  
//         return;
//     size_t cnt = 0;
//     // if(idx >= 0)
//         flag.arrayDefString<<"[";
//     if(arrDefList[0]->expr && arrDefList[0]->arrDefList.empty()){
//         for (auto &&i : arrDefList)
//         {
//             flag.arrayDefString<<p->getDimTypeStrings()[idx]<<" "<<((ConstantSymbolEntry*)arrDefList[cnt]->expr->getSymbolEntry())->toStr();
//             cnt++;
//             if(idx >= 0 && cnt < arrDefList.size())
//                 flag.arrayDefString<<",";
//         }
//         flag.arrayDefString<<"]";
//         return;
//     }
//     for (auto &&i : arrDefList)
//     {
//         // if(idx >= 0){
//             // if(expr){
//             //     flag.arrayDefString<<p->getDimTypeStrings()[idx]<<" "<<((ConstantSymbolEntry*)expr->getSymbolEntry())->toStr();
//             //     continue;
//             // }else{
//                 flag.arrayDefString<<p->getDimTypeStrings()[idx];
//             // }
//         // }
//         idx++;
//         i->getArrayDefStr(idx);
//         idx--;
//         cnt++;
//         if(idx >= 0 && cnt < arrDefList.size())
//             flag.arrayDefString<<",";
//     }
//     // if(idx >= 0)
//         flag.arrayDefString<<"]";
//     return;
// }

void ArrayIndex::genCode() {

    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    flag.arrayIdStk.push((IdentifierSymbolEntry*)arrDef);
    bool temp = flag.isOuterCond;
    flag.isOuterCond = false;
    dim->genCode();
    flag.isOuterCond = temp;
    flag.arrayIdStk.pop();
    // Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(arrDef)->getAddr();
    new LoadInstruction(dst, dim->getDst(), bb);

    if(flag.isUnderCond && flag.isOuterCond){
        BasicBlock *falseBlock;
        falseBlock = new BasicBlock(func);
        auto ret = typeConvention(TypeSystem::boolType, dst, bb);
        true_list.push_back(new CondBrInstruction(nullptr, falseBlock, ret, bb));
        // without it break for the same reason but found at toBB_f->addPred(i);
        false_list.push_back(new UncondBrInstruction(nullptr, falseBlock)); // when && break at a CondBrInstruction miss false_branch found at output and none of block end with CondBrInstruction
    }
}

void ArrayIndex::genLvalCode()
{
    BasicBlock *bb = builder->getInsertBB();
    flag.arrayIdStk.push((IdentifierSymbolEntry*)arrDef);
    dim->genCode();
    flag.arrayIdStk.pop();
    dst = dim->getDst();
}

void EmptyStmt::genCode() {

}

void ExprStmt::genCode() {

    exp->genCode();
}

void BreakStmt::genCode() {
    assert(flag.whileEndStack.size()!=0);
    BasicBlock* bb = builder->getInsertBB();
    Function *func = bb->getParent();
    //获取当前while循环的end_bb
    BasicBlock* end_bb = flag.whileEndStack.top();
    //无条件跳转到end_bb
    new UncondBrInstruction(end_bb, bb);
    // 声明一个新的基本块用来插入后续的指令
    BasicBlock* nextBlock = new BasicBlock(func);
    builder->setInsertBB(nextBlock);
}

void ContinueStmt::genCode() {
    assert(flag.whileCondStack.size()!=0);
    BasicBlock* bb = builder->getInsertBB();
    Function *func = bb->getParent();
    //获取当前while循环的cond_bb
    BasicBlock* cond_bb = flag.whileCondStack.top();
    //无条件跳转到cond_bb
    new UncondBrInstruction(cond_bb, bb);
    // 声明一个新的基本块用来插入后续的指令
    BasicBlock* nextBlock = new BasicBlock(func);
    builder->setInsertBB(nextBlock);
}

void WhileStmt::genCode() {

    //q9while语句
    // whileBodyStmt->genCode();

    Function *func;
    BasicBlock *then_bb, *end_bb, *cond_bb, *bb_prev = builder->getInsertBB();

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    cond_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);
    
    //当前层次whileStmt的 cond和end bb入栈
    flag.whileCondStack.push(cond_bb);
    flag.whileEndStack.push(end_bb);

    new UncondBrInstruction(cond_bb, bb_prev);
    builder->setInsertBB(cond_bb);
    flag.isUnderCond = true;
    flag.isOuterCond = true;
    cond->genCode();
    flag.isOuterCond = false;
    flag.isUnderCond = false;
    cond_bb = builder->getInsertBB();
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(then_bb);
    whileBodyStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(cond_bb, then_bb);

    builder->setInsertBB(end_bb);
    
    //当前层次whileStmt的 cond和end bb出栈
    flag.whileCondStack.pop();
    flag.whileEndStack.pop();
}

void FuncParam::genCode() {

    for (auto &&i : paramList)
    {
        i->genCode();
        flag.cntParam++;
    }
    flag.cntParam = 0;
}

/*---------------------------TYPE CHECK----------------------------------------*/
void Ast::typeCheck()
{
    if(root != nullptr)
        root->typeCheck();
}

void FunctionDef::typeCheck()
{
    flag.isInFunc = true;   
    if(params)
        params->typeCheck();
    flag.shouldReturn = ((FunctionType*)(se->getType()))->getRetType();
    flag.haveReturn = false;
    stmt->typeCheck();
    if(!flag.haveReturn){
        if(!flag.shouldReturn->isVoid()){
            std::cout<<"错误！返回类型为非空的函数应有return语句！"<<std::endl;
        }
        flag.haveReturn = false;
    }
    //若返回void类型但没有写return语句，需要加上（否则false_block为空导致错误
    if(!flag.haveReturn && flag.shouldReturn->isVoid())
    {
        this->voidAddRet = new ReturnStmt(nullptr);
    }
    flag.isInFunc = false;
}

void BinaryExpr::typeCheck()
{
    //p4二元运算类型检查
    if(!(expr1->getSymbolEntry()->getType()->isNumber() &&
        expr2->getSymbolEntry()->getType()->isNumber())){
        std::cout<<"错误！二元运算出现非法类型！"<<std::endl;
    }

    //p8孤立Expr关闭void检查
    //不再需要结果类型检查
    flag.exprShouldCheck = true;

    expr1->typeCheck();
    expr2->typeCheck();
}

void Constant::typeCheck()
{
    // Todo
//p8孤立Expr关闭void检查
    //不再需要结果类型检查
    flag.exprShouldCheck = true;
    
}

void Id::typeCheck()
{
    // Todo
    //p8孤立Expr关闭void检查
    //不再需要结果类型检查
    flag.exprShouldCheck = true;
    
}

void IfStmt::typeCheck()
{
    // Todo
    cond->typeCheck();
    thenStmt->typeCheck();
}

void IfElseStmt::typeCheck()
{
    // Todo
    cond->typeCheck();
    thenStmt->typeCheck();
    elseStmt->typeCheck();
}

void CompoundStmt::typeCheck()
{
    // Todo
    if(stmt)
        stmt->typeCheck();
}

void SeqNode::typeCheck()
{
    // Todo


    for (auto &&i : this->stmtList)
    {
        i->typeCheck();
    }
    
}

void DeclStmt::typeCheck()
{
    // Todo
    for (size_t i = 0; i < idList.size(); i++)
    {
        //p5赋值运算类型检查
        if(dimArrayList[i] == nullptr && exprList[i] != nullptr &&
            !(idList[i]->getSymbolEntry()->getType()->isNumber() && 
            exprList[i]->getSymbolEntry()->getType()->isNumber()))
        {
            std::cout<<"错误！定义时赋值类型不匹配！"<<std::endl;
        }
        //p6CONST检查
        if(idList[i]->getSymbolEntry()->isConstant() &&
            (exprList[i] == nullptr && defArrList[i] == nullptr)){
            std::cout<<"错误！CONST未初始化！"<<std::endl;
        }

        if(idList[i])
            idList[i]->typeCheck();
        if(exprList[i])
            exprList[i]->typeCheck();
        if(dimArrayList[i])
            dimArrayList[i]->typeCheck();
        if(defArrList[i])
            defArrList[i]->typeCheck();
    }
    
}

void ReturnStmt::typeCheck()
{
    // Todo
    //p9函数返回类型检查
    if(!flag.isInFunc){
        std::cout<<"错误！函数外使用return语句！"<<std::endl;
        return;
    }
    assert(flag.shouldReturn != nullptr);
    if(flag.shouldReturn == TypeSystem::voidType){
        if(retValue){
            std::cout<<"错误！函数返回类型为VOID但是给出了返回值！"<<std::endl;
        }
    }else if(!retValue || !retValue->getSymbolEntry()->getType()->isNumber()){
        std::cout<<"错误！函数实际返回类型与定义不符！"<<std::endl;
    }
    flag.haveReturn = true;
    if(retValue)
        retValue->typeCheck();
}

void AssignStmt::typeCheck()
{
    //p5赋值运算类型检查
    if(!(lval->getSymbolEntry()->getType()->isNumber() && 
        expr->getSymbolEntry()->getType()->isNumber()))
    {   
        std::cout<<"错误！运算赋值时类型不匹配！"<<std::endl;
    }
    //p6CONST检查
    if(lval->getSymbolEntry()->isConstant()){
        std::cout<<"错误！CONST不能被修改！"<<std::endl;
    }

    lval->typeCheck();
    expr->typeCheck();
}
//------------------NEW TYPE CHECK--------------------
void FuncCall::typeCheck() {

    //p8孤立Expr关闭void检查
    if(flag.exprShouldCheck){
        if(((FunctionType*)(funcDef->getType()))->getRetType()->getKind() == TypeSystem::voidType->getKind()){
            std::cout<<"错误！函数返回类型为VOID无法运算！"<<std::endl;
        }
    }
    flag.exprShouldCheck = true;

    for (auto &&i : arg)
    {
        i->typeCheck();
    }
}

void UnaryExpr::typeCheck() {
    //p4二元运算类型检查
    // if(!(expr->getSymbolEntry()->getType()->isNumber())){
    //     std::cout<<"错误！一元运算出现非法类型！"<<std::endl;
    // }

    //p8孤立Expr关闭void检查
    //不再需要结果类型检查
    flag.exprShouldCheck = true;

    expr->typeCheck();
}

void DimArray::typeCheck() {


    for (auto &&i : dimList)
    {
        if(!i->getSymbolEntry()->getType()->isInt()){
            std::cout<<"错误！数组下标不是整数！"<<std::endl;
        }   
        i->typeCheck();
    }
}

void ArrayDef::typeCheck() {


    for (auto &&i : this->arrDefList)
    {
        i->typeCheck();
    }
    if(this->expr){
        //p5赋值运算类型检查
        if(!this->expr->getSymbolEntry()->getType()->isNumber()){
            std::cout<<"错误！数组定义出现非法初值！"<<std::endl;
        }
        this->expr->typeCheck();
    }
}

void ArrayIndex::typeCheck() {

    this->dim->typeCheck();
}

void EmptyStmt::typeCheck() {

}

void ExprStmt::typeCheck() {

    //p8孤立Expr关闭void检查
    //孤立的Expr+分号，关闭结果类型检查
    flag.exprShouldCheck = false;
    exp->typeCheck();
}

void BreakStmt::typeCheck() {

    if(!flag.isInWhileCnt){
        std::cout<<"错误！Break语句出现在非法位置！"<<std::endl;
    }

}

void ContinueStmt::typeCheck() {
    if(!flag.isInWhileCnt){
        std::cout<<"错误！Continue语句出现在非法位置！"<<std::endl;
    }
}

void WhileStmt::typeCheck() {

    //p10break\continue的位置检查
    flag.isInWhileCnt++;
    cond->typeCheck();
    whileBodyStmt->typeCheck();
    flag.isInWhileCnt--;
}

void FuncParam::typeCheck() {

    for (auto &&i : paramList)
    {
        i->typeCheck();
    }
    
}



void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
}

void BinaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break;
        case LOGIC_AND:
            op_str = "logic_and";
            break;
        case LOGIC_OR:
            op_str = "logic_or";
            break;
        case LESS:
            op_str = "less";
            break;
        case GREATER:
            op_str = "greater";
            break;
        case LESS_EQUAL:
            op_str = "less_equal";
            break;
        case GREATER_EQUAL:
            op_str = "greater_equal";
            break;
        case EQUAL_TO:
            op_str = "equal_to";
            break;
        case NOT_EQUAL_TO:
            op_str = "not_equal_to";
            break;
        case PRODUCT:
            op_str = "mul";
            break;
        case DIVISION:
            op_str = "div";
            break;
        case REMAINDER:
            op_str = "mod";
            break;
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

void UnaryExpr::output(int level)
{
    std::string op_str;
    switch (op)
    {
        case SUB:
            op_str = "neg";
            break;
        case LOGIC_NOT:
            op_str = "logic_not";
            break;
    }
    fprintf(yyout,"%*cUnaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr->output(level + 4);
}

void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    //q6浮点数支持
    if(symbolEntry->getType()->isInt())
        fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
                value.c_str(), type.c_str());
    else
        fprintf(yyout, "%*cFloatLiteral\tvalue: %s\ttype: %s\n", level, ' ',
                value.c_str(), type.c_str());
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}
//q14数组取值
void ArrayIndex::output(int level)
{
    std::string name, type;
    int scope;
    name = arrDef->toStr();
    type = symbolEntry->getType()->toStr();
    // scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    scope = dynamic_cast<IdentifierSymbolEntry*>(arrDef)->getScope();
    fprintf(yyout, "%*cArrayValue\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
    dim->output(level + 4);
}


//q12函数调用
void FuncCall::output(int level){
    fprintf(yyout, "%*cFunctionCall function name: %s, type: %s\n", level, ' ', 
            funcDef->toStr().c_str(), funcDef->getType()->toStr().c_str());
    for (auto i:arg){
        i->output(level + 4);
    }
}



void EmptyStmt::output(int level)
{
    fprintf(yyout, "%*cEmptyStmt\n", level, ' ');
}

void ExprStmt::output(int level)
{
    exp->output(level + 4);
}

void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}



// void CompoundStmt::output(int level)
// {
//     fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
//     stmt->output(level + 4);
// }
void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    if(stmt != nullptr){
        stmt->output(level + 4);
    }
    else{
        fprintf(yyout, "%*cNULL Stmt\n", level + 4, ' ');
    }
}


// void SeqNode::output(int level)
// {
//     fprintf(yyout, "%*cSequence\n", level, ' ');
//     stmt1->output(level + 4);
//     stmt2->output(level + 4);
// }
//q1解决seq支持数目过少的问题
void SeqNode::addNext(StmtNode* next)
{
    stmtList.push_back(next);
}
void SeqNode::output(int level)
{
    fprintf(yyout, "%*cSequence\n", level, ' ');
    for(auto stmt : stmtList){
        stmt->output(level + 4);
    }
}
//q8数组支持
void DimArray::addDim(ExprNode* next)
{
    dimList.push_back(next);
}
void DimArray::output(int level)
{
    fprintf(yyout, "%*cArray Dimension List\n", level, ' ');
    for(auto expr : dimList){
        expr->output(level + 4);
    }
}


//q9数组定义
void ArrayDef::addDef(ArrayDef* def)
{
    arrDefList.push_back(def);
}
void ArrayDef::output(int level)
{
    fprintf(yyout, "%*cArray Definition List\n", level, ' ');
    if(expr){
        expr->output(level + 4);
        return;
    }
    for(auto itm : arrDefList){
        itm->output(level + 4);
    }
}

//q9数组定义
// void ArrayItem::addExpr(ExprNode* exp)
// {
//     exprList.push_back(exp);
// }
// void ArrayItem::output(int level)
// {
//     // fprintf(yyout, "%*cArray Dimension List\n", level, ' ');
//     for(auto expr : exprList){
//         expr->output(level + 4);
//     }
// }


void DeclStmt::addDecl(Id* next, ExprNode *exp, DimArray *dim, ArrayDef *defList){
    idList.push_back(next);
    exprList.push_back(exp);
    dimArrayList.push_back(dim);
    defArrList.push_back(defList);
}
void DeclStmt::output(int level)
{
    for (size_t i = 0; i < idList.size(); i++)
    {
        fprintf(yyout, "%*c", level, ' ');
        // q2const常量支持
        //不对const另立节点类型，而是直接输出
        if(idList[i]->getSymbolEntry()->isConstant()){
            fprintf(yyout, "Constant\t");
        }
        // expr存在
        if(exprList[i]){
            fprintf(yyout, "DefStmt\n");
            idList[i]->output(level + 4);
            exprList[i]->output(level + 4);
        }else{
            fprintf(yyout, "DeclStmt\n");
            idList[i]->output(level + 4);
        }
        //q8数组支持
        if(dimArrayList[i]){
            dimArrayList[i]->output(level + 4);
        }
        //q9数组定义
        if(defArrList[i]){
            defArrList[i]->output(level + 4);
        }
    }
    
}
// void DefStmt::output(int level)
// {
//     fprintf(yyout, "%*c", level, ' ');
//     // q2const常量支持
//     //不对const另立节点类型，而是直接输出
//     if(id->getSymbolEntry()->isConstant()){
//         fprintf(yyout, "Constant\t");
//     }
//     fprintf(yyout, "DefStmt\n");
//     id->output(level + 4);
//     expr->output(level + 4);
// }

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

//q5参照if语句实现while循环
void WhileStmt::output(int level)
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);
    whileBodyStmt->output(level + 4);
}

// void ReturnStmt::output(int level)
// {
//     fprintf(yyout, "%*cReturnStmt\n", level, ' ');
//     retValue->output(level + 4);
// }
void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    if(retValue != nullptr){
        retValue->output(level + 4);
    }
    else{
        fprintf(yyout, "%*cVOID\n", level + 4, ' ');
    }
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    if(params){
        params->output(level + 4);
    }
    stmt->output(level + 4);
    if(voidAddRet!=nullptr)
    {
        voidAddRet->output(level + 4);
    }
}
//q10添加参数列表
void FuncParam::addNext(StmtNode* next)
{
    paramList.push_back(next);
}
void FuncParam::output(int level)
{
    fprintf(yyout, "%*cParameters List\n", level, ' ');
    for(auto stmt : paramList){
        stmt->output(level + 4);
    }
}


