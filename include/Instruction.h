#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "Operand.h"
#include "AsmBuilder.h"
#include <vector>
#include <map>
#include <sstream>

class BasicBlock;

class Instruction
{
public:
    Instruction(unsigned instType, BasicBlock *insert_bb = nullptr);
    virtual ~Instruction();
    BasicBlock *getParent();
    bool isUncond() const {return instType == UNCOND;};
    bool isCond() const {return instType == COND;};
    bool isAlloc() const {return instType == ALLOCA;};
    bool isRet() const {return instType == RET;};
    void setParent(BasicBlock *);
    void setNext(Instruction *);
    void setPrev(Instruction *);
    Instruction *getNext();
    Instruction *getPrev();
    virtual void output() const = 0;
    MachineOperand* genMachineOperand(Operand*);
    MachineOperand* genMachineReg(int reg);
    MachineOperand* genMachineVReg(bool isFlt = false);
    MachineOperand* genMachineImm(int val);
    MachineOperand* genMachineLabel(int block_no);
    virtual void genMachineCode(AsmBuilder*) = 0;
    std::vector<Operand*>& getOperands() { return operands; }
    // 这是在后期优化时用于分析的，返回一个需要的类型，欢迎重载
    Type* getType() { return nullptr; }
    // 我们把这条指令从基本块里删除
    void removeCurrInst();
    // 这就意味着必须每一条指令的op必须全部来自operands
    // 替换operands中的指针并重新设置use关系
    bool changeOperand(Operand* op, Operand* old);
protected:
    unsigned instType;
    unsigned opcode;
    Instruction *prev;
    Instruction *next;
    BasicBlock *parent;
    std::vector<Operand*> operands;
    enum {BINARY, COND, UNCOND, RET, LOAD, STORE, CMP, ALLOCA, CALL, ZEXT, CAST, PHI};
};

// meaningless instruction, used as the head node of the instruction list.
class DummyInstruction : public Instruction
{
public:
    DummyInstruction() : Instruction(-1, nullptr) {};
    void output() const {};
    void genMachineCode(AsmBuilder*) {};
};

class AllocaInstruction : public Instruction
{
public:
    AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb = nullptr);
    ~AllocaInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    Type* getType() { return se->getType(); }   // 我们不返回指针类型
private:
    SymbolEntry *se;
};

class LoadInstruction : public Instruction
{
public:
    LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb = nullptr);
    ~LoadInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    Type* getType() { return operands[0]->getType(); }
    // 所有用到了我的dst的地方，你们use这个op吧
    void replaceAllUsesWith(Operand* op);
};

class GetElementPtrInstruction : public LoadInstruction
{
public:
    GetElementPtrInstruction(Operand *dst, Operand *src_addr, Operand * dim, /*bool isParam = false,*/ BasicBlock *insert_bb = nullptr, IdentifierSymbolEntry* ident = nullptr);
    void output() const;
    void genMachineCode(AsmBuilder*);
    bool isZeroDim();
private:
    // Operand * dim;
    // bool isParam = false;
    IdentifierSymbolEntry* arr;
};

class StoreInstruction : public Instruction
{
public:
    StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb = nullptr);
    ~StoreInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    Type* getType() { return operands[1]->getType(); }
    bool isStoringGlobalVal() { return operands[1]->getEntry()->isVariable() && ((IdentifierSymbolEntry*)operands[1]->getEntry())->isGlobal(); }
};

class BinaryInstruction : public Instruction
{
public:
    BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb = nullptr);
    ~BinaryInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    enum {SUB, ADD, MUL, DIV, MOD, AND, OR, LSL};
};

class CmpInstruction : public Instruction
{
public:
    CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb = nullptr);
    ~CmpInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    enum {E, NE, L, LE, G, GE};
};

// unconditional branch
class UncondBrInstruction : public Instruction
{
public:
    UncondBrInstruction(BasicBlock*, BasicBlock *insert_bb = nullptr);
    void output() const;
    void setBranch(BasicBlock *);
    BasicBlock *getBranch();
    void genMachineCode(AsmBuilder*);
protected:
    BasicBlock *branch;
};

// conditional branch
class CondBrInstruction : public Instruction
{
public:
    CondBrInstruction(BasicBlock*, BasicBlock*, Operand *, BasicBlock *insert_bb = nullptr);
    ~CondBrInstruction();
    void output() const;
    void setTrueBranch(BasicBlock*);
    BasicBlock* getTrueBranch();
    void setFalseBranch(BasicBlock*);
    BasicBlock* getFalseBranch();
    void genMachineCode(AsmBuilder*);
protected:
    BasicBlock* true_branch;
    BasicBlock* false_branch;
};

class RetInstruction : public Instruction
{
public:
    RetInstruction(Operand *src, BasicBlock *insert_bb = nullptr);
    ~RetInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    bool isVoid() { return operands.empty(); }
};

//q5FunctionCall的代码生成
class FunctionCallInstuction : public Instruction
{
private:
    IdentifierSymbolEntry* func;
    //需要支持不同长度的参数列表
public:
    FunctionCallInstuction(Operand *dst, std::vector<Operand*> params, IdentifierSymbolEntry* func, BasicBlock *insert_bb = nullptr);
    ~FunctionCallInstuction();
    void output() const;
    void genMachineCode(AsmBuilder*);
};

// 符号扩展零填充指令
class ZextInstruction : public Instruction
{
public:
    ZextInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    ~ZextInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
};

//q11浮点数支持
class IntFloatCastInstruction : public Instruction
{
public:
    IntFloatCastInstruction(unsigned opcode, Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    ~IntFloatCastInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
    enum {I2F, F2I};
};

// 强制类型转换
class BitCastInstruction : public ZextInstruction
{
public:
    BitCastInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr) : ZextInstruction(dst, src, insert_bb) {};
    void output() const;
    void genMachineCode(AsmBuilder*);
};

class PhiInstruction : public Instruction
{
public:
    PhiInstruction(BasicBlock *insert_bb = nullptr) : Instruction(PHI, insert_bb) {};
    PhiInstruction(Operand *dst, std::vector<Operand *> srcVec, std::vector<BasicBlock *> blkVec, BasicBlock *insert_bb = nullptr);
    ~PhiInstruction();
    void output() const;
    void genMachineCode(AsmBuilder*);
private:
    std::vector<BasicBlock *> blkVec;
};

#endif