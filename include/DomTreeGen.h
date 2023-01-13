
class MachineFunction;
class MachineUnit;
class MachineOperand;
class MachineBlock;

class DomTreeGen
{
private:

public:
    DomTreeGen();
    ~DomTreeGen();

public:
    void pass(MachineFunction *func);
};


