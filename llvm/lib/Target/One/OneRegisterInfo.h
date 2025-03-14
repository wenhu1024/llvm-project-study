#ifndef ONEREGISTERINFO_H
#define ONEREGISTERINFO_H

#define GET_REGINFO_HEADER
#include "OneGenRegisterInfo.inc"

namespace llvm {
class OneRegisterInfo : public OneGenRegisterInfo {
public:
  OneRegisterInfo();
  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  bool eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;
};

} // namespace llvm

#endif
