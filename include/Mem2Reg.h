#include <vector>
using namespace std;

#include "DomTreeGen.h"

class MachineUnit;
class MachineOperand;
class MachineFunction;

class Mem2Reg
{
private:
    MachineUnit* unit;
    MachineFunction *currFunc;
public:
    Mem2Reg(MachineUnit* u);
    ~Mem2Reg();
public:
    void buildSSA();
private:
    void promoteMemoryToRegister();
    void primarySimplify();
    void genDomTree(), insertPhiNode(), renamePass(), cleanState();
    bool isPromotable(AllocaInstruction* alloc);
};


