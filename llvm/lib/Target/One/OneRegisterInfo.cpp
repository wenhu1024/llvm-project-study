#include "OneRegisterInfo.h"
#include "MCTargetDesc/OneMCTargetDesc.h"
#include "OneSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "OneGenRegisterInfo.inc"

OneRegisterInfo::OneRegisterInfo() : OneGenRegisterInfo(One::X1) {}

const MCPhysReg *
OneRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  static const MCPhysReg CalleeSavedRegs[] = {One::X2, 0};
  return CalleeSavedRegs;
}

BitVector OneRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  Reserved.set(One::X0);
  return Reserved;
}

bool OneRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  return false;
}

Register OneRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
    return One::X2;
}
