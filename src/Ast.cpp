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
    Type* shouldReturn;
    bool isInFunc = false;
    bool isInWhile = false;

    std::stack<int> cntEle;
    
} flag;

Node::Node()
{
    seq = counter++;
}

void Node::backPatch(std::vector<Instruction*> &list, BasicBlock*bb)
{
    for(auto &inst:list)
    {
        if(inst->isCond())
            dynamic_cast<CondBrInstruction*>(inst)->setTrueBranch(bb);
        else if(inst->isUncond())
            dynamic_cast<UncondBrInstruction*>(inst)->setBranch(bb);
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
    Function *func = new Function(unit, se);
    BasicBlock *entry = func->getEntry();
    // set the insert point to the entry basicblock of this function.
    builder->setInsertBB(entry);

    if(params!=nullptr){
        params->genCode();
    }
    stmt->genCode();

    /**
     * Construct control flow graph. You need do set successors and predecessors for each basic block.
     * Todo
    */

    std::vector<BasicBlock*> toExit;
   //q3function连接基本块
   auto bl = func->getBlockList();
   for (auto &&i : bl)
   {
        //去除ret后面还有指令的情况
        bool shouldErase = false;
        for(auto j = i->begin(); j != i->end(); j = j->getNext()){
            if(shouldErase){
                i->remove(j);
            }
            if(j->isRet()){
                shouldErase = true;
            }
        }
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
    // set the insert point to the entry basicblock of this function.
    builder->setInsertBB(exit);
   //q4为function加入exit块
   for (auto &&i : toExit)
   {
        exit->addPred(i);
        i->addSucc(exit);
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
        // Todo
    }
    else if(op >= LESS && op <= GREATER)
    {
        // Todo
    }
    else if(op >= ADD && op <= SUB)
    {
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
        }
        new BinaryInstruction(opcode, dst, src1, src2, bb);
    }
}

void Constant::genCode()
{
    // we don't need to generate code.
}

void Id::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    new LoadInstruction(dst, addr, bb);
}

void IfStmt::genCode()
{
    Function *func;
    BasicBlock *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    cond->genCode();
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
    // Todo
}

void CompoundStmt::genCode()
{
    // Todo
    //q2补全代码生成调用链
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
    //配合idList
    for (const auto id:idList){
        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymPtr());
        if(se->isGlobal())
        {
            Operand *addr;
            SymbolEntry *addr_se;
            addr_se = new IdentifierSymbolEntry(*se);
            addr_se->setType(new PointerType(se->getType()));
            addr = new Operand(addr_se);
            se->setAddr(addr);
        }
        else if(se->isLocal())
        {
            Function *func = builder->getInsertBB()->getParent();
            BasicBlock *entry = func->getEntry();
            Instruction *alloca;
            Operand *addr;
            SymbolEntry *addr_se;
            Type *type;
            type = new PointerType(se->getType());
            addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
            addr = new Operand(addr_se);
            alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
            entry->insertFront(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
            se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.
        }
    }
}

void ReturnStmt::genCode()
{
    // Todo
}

void AssignStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    expr->genCode();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(lval->getSymPtr())->getAddr();
    Operand *src = expr->getOperand();
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just store the result of the `expr` to the addr of the id.
     * If you want to implement array, you have to caculate the address first and then store the result into it.
     */
    new StoreInstruction(addr, src, bb);
}
//------------------NEW GEN-CODE--------------------
void FuncCall::genCode() {

    //q2补全代码生成调用链
    for (auto &&i : arg)
    {
        i->genCode();
    }
    
}

void UnaryExpr::genCode() {
    
    //q2补全代码生成调用链
    expr->genCode();
}

void DimArray::genCode() {

    for (auto &&i : dimList)
    {
        i->genCode();
    }
    
}

void ArrayDef::genCode() {

    for (auto &&i : arrDefList)
    {
        i->genCode();
    }
    
}

void ArrayIndex::genCode() {

    dim->genCode();
}

void EmptyStmt::genCode() {

}

void ExprStmt::genCode() {

    exp->genCode();
}

void BreakStmt::genCode() {

}

void ContinueStmt::genCode() {

}

void WhileStmt::genCode() {

    cond->genCode();
    whileBodyStmt->genCode();
}

void FuncParam::genCode() {

    for (auto &&i : paramList)
    {
        i->genCode();
    }
    
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
    stmt->typeCheck();
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

    if(!flag.isInWhile){
        std::cout<<"错误！Break语句出现在非法位置！"<<std::endl;
    }

}

void ContinueStmt::typeCheck() {
    if(!flag.isInWhile){
        std::cout<<"错误！Continue语句出现在非法位置！"<<std::endl;
    }
}

void WhileStmt::typeCheck() {

    //p10break\continue的位置检查
    flag.isInWhile = true;
    cond->typeCheck();
    whileBodyStmt->typeCheck();
    flag.isInWhile = false;
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


