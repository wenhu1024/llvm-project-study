#include "OneRegisterInfo.h"
#include "MCTargetDesc/OneMCTargetDesc.h"
#include "OneSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "One.h"


using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "OneGenRegisterInfo.inc"

#define DEBUG_TYPE "one-reg-info"

OneRegisterInfo::OneRegisterInfo(const OneSubtarget &STI) : OneGenRegisterInfo(One::RA), STI(STI) {}

const MCPhysReg *
OneRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_SaveList;
}

const uint32_t *OneRegisterInfo::getCallPreservedMask(const MachineFunction &MF, CallingConv::ID) const{
  return CSR_RegMask;
}

BitVector OneRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  Reserved.set(One::ZERO);
  Reserved.set(One::SP);
  Reserved.set(One::RA);
  return Reserved;
}

bool OneRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  MachineInstr &MI = *II;
  LLVM_DEBUG(errs()<<MI);
        
  uint i=0;
  while(!MI.getOperand(i).isFI()) {
    ++i;
    assert(i < MI.getNumOperands());
  }

  const int FI = MI.getOperand(i).getIndex();
  const MachineFunction &MF = *MI.getParent()->getParent();
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  int64_t Offset = MFI.getObjectOffset(FI);
  uint64_t StackSize = ROUND_UP(MFI.getStackSize(), STI.getFrameLowering()->getStackAlignment());
  Offset += static_cast<int64_t>(StackSize);

  MI.getOperand(i).ChangeToRegister(One::SP, false);
  MI.getOperand(i+1).ChangeToImmediate(Offset);
  return true;
}

Register OneRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
    return One::SP;
}
