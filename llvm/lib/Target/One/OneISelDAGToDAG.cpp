#include "One.h"
#include "OneSubtarget.h"
#include "OneTargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"

using namespace llvm;

#define DEBUG_TYPE "one-isel"
#define PASS_NAME "One DAG->DAG Pattern Instruction Selection"

class OneDAGToDAGISel : public SelectionDAGISel {
public:
  OneDAGToDAGISel() = delete;
  explicit OneDAGToDAGISel(OneTargetMachine &TM)
      : SelectionDAGISel(TM), Subtarget(nullptr) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

private:
  const OneSubtarget *Subtarget;

#include "OneGenDAGISel.inc"

  const OneTargetMachine &getTargetMachine() {
    return static_cast<const OneTargetMachine &>(TM);
  }

  void Select(SDNode *Node) override;
};

bool OneDAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
  Subtarget = &MF.getSubtarget<OneSubtarget>();
  return SelectionDAGISel::runOnMachineFunction(MF);
}

void OneDAGToDAGISel::Select(SDNode *Node){
  SDLoc DL(Node);

  LLVM_DEBUG(dbgs() << "Selecting: "; Node->dump(CurDAG); dbgs() << '\n');

  SelectCode(Node);
}

class OneDAGToDAGISelLegacy : public SelectionDAGISelLegacy {
public:
  static char ID;
  explicit OneDAGToDAGISelLegacy(OneTargetMachine &TM)
      : SelectionDAGISelLegacy(ID, std::make_unique<OneDAGToDAGISel>(TM)) {}
};

char OneDAGToDAGISelLegacy::ID;

INITIALIZE_PASS(OneDAGToDAGISelLegacy, DEBUG_TYPE, PASS_NAME, false, false)

FunctionPass *
llvm::createOneISelDag(OneTargetMachine &TM) {
  return new OneDAGToDAGISelLegacy(TM);
}
