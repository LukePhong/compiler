#ifndef __BASIC_BLOCK_H__
#define __BASIC_BLOCK_H__
#include <vector>
#include <set>
#include "Instruction.h"
#include "AsmBuilder.h"

class Function;

class BasicBlock
{
    typedef std::vector<BasicBlock *>::iterator bb_iterator;

private:
    std::vector<BasicBlock *> pred, succ;   // 每一个有pred的地方都有对应的succ关系，所以信息是重复的 只用获取一种即可
    Instruction *head;
    Function *parent;
    int no;
    std::vector<BasicBlock *> domFrontier;
    BasicBlock* idom = nullptr;

public:
    BasicBlock(Function *);
    ~BasicBlock();
    void insertFront(Instruction *);
    void insertBack(Instruction *);
    void insertBefore(Instruction *, Instruction *);
    void remove(Instruction *);
    bool empty() const { return head->getNext() == head;}
    void output() const;
    bool succEmpty() const { return succ.empty(); };
    bool predEmpty() const { return pred.empty(); };
    void addSucc(BasicBlock *);
    void removeSucc(BasicBlock *);
    void addPred(BasicBlock *);
    void removePred(BasicBlock *);
    int getNo() { return no; };
    Function *getParent() { return parent; };
    //服务于迭代器
    Instruction* begin() { return head->getNext();};
    Instruction* end() { return head;};
    Instruction* rbegin() { return head->getPrev();};
    Instruction* rend() { return head;};
    bb_iterator succ_begin() { return succ.begin(); };
    bb_iterator succ_end() { return succ.end(); };
    bb_iterator pred_begin() { return pred.begin(); };
    bb_iterator pred_end() { return pred.end(); };
    int getNumOfPred() const { return pred.size(); };
    int getNumOfSucc() const { return succ.size(); };
    void genMachineCode(AsmBuilder*);
    bool isInstAfterInst(Instruction* a, Instruction* b);

    std::vector<BasicBlock *> getDomFrontier() { return domFrontier; }
    void addDomFrontier(BasicBlock* b) { domFrontier.push_back(b); }
    bool theBlockInDomFrontier(BasicBlock* b) { return std::find(domFrontier.begin(), domFrontier.end(), b) != domFrontier.end(); }
    void setIdom(BasicBlock* b) { idom = b; }
    bool theBlockDomMe(BasicBlock* b);
};

#endif