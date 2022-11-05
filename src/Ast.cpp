#include "Ast.h"
#include "SymbolTable.h"
#include <string>
#include "Type.h"

extern FILE *yyout;
int Node::counter = 0;

Node::Node()
{
    seq = counter++;
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
}

// void BinaryExpr::output(int level)
// {
//     std::string op_str;
//     switch(op)
//     {
//         case ADD:
//             op_str = "add";
//             break;
//         case SUB:
//             op_str = "sub";
//             break;
//         case AND:
//             op_str = "and";
//             break;
//         case OR:
//             op_str = "or";
//             break;
//         case LESS:
//             op_str = "less";
//             break;
//     }
//     fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
//     expr1->output(level + 4);
//     expr2->output(level + 4);
// }
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

void EmptyStmt::output(int level)
{
    fprintf(yyout, "%*cEmptyStmt\n", level, ' ');
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

void DeclStmt::addDecl(Id* next, ExprNode *exp, DimArray *dim){
    idList.push_back(next);
    exprList.push_back(exp);
    dimArrayList.push_back(dim);
}
void DeclStmt::output(int level)
{
    // fprintf(yyout, "DeclStmt\n");
    // id->output(level + 4);
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
    stmt->output(level + 4);
}
