#ifndef __IRBUILDER_H__
#define __IRBUILDER_H__

class Unit;
class Function;
class BasicBlock;

class IRBuilder
{
private:
    Unit *unit;     //编译单元，是我们中间代码的顶层模块，包含我们中间代码生成时创建的函数。
    BasicBlock *insertBB;   // The current basicblock that instructions should be inserted into.

public:
    IRBuilder(Unit*unit) : unit(unit){};
    void setInsertBB(BasicBlock*bb){insertBB = bb;};
    Unit* getUnit(){return unit;};
    BasicBlock* getInsertBB(){return insertBB;};
};

#endif