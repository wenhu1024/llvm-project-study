#include "OneFrameLowering.h"
#include "MCTargetDesc/OneMCTargetDesc.h"
#include "One.h"
#include "OneSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"

using namespace llvm;

uint64_t OneFrameLowering::computeStackSize(MachineFunction &MF) const {
  uint64_t StackSize = MF.getFrameInfo().getStackSize();
  if (getStackAlignment() > 0) {
    StackSize = ROUND_UP(StackSize, getStackAlignment());
  }
  return StackSize;
}

void OneFrameLowering::emitPrologue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {

  const TargetInstrInfo &TII = *STI.getInstrInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();

  int STACKSIZE = computeStackSize(MF);

  if (STACKSIZE == 0)
    return;

  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  BuildMI(MBB, MBBI, DL, TII.get(One::ADDI), One::SP)
      .addReg(One::SP)
      .addImm(-STACKSIZE)
      .setMIFlag(MachineInstr::FrameSetup);
}

void OneFrameLowering::emitEpilogue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {

  const TargetInstrInfo &TII = *STI.getInstrInfo();
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();

  int STACKSIZE = computeStackSize(MF);

  if (STACKSIZE == 0)
    return;

  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  BuildMI(MBB, MBBI, DL, TII.get(One::ADDI), One::SP)
      .addReg(One::SP)
      .addImm(STACKSIZE)
      .setMIFlag(MachineInstr::FrameDestroy);
}

void OneFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                            BitVector &SavedRegs,
                                            RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);

  if (MF.getFrameInfo().hasCalls()) {
    SavedRegs.set(One::RA);
  }
}

bool OneFrameLowering::hasFPImpl(const MachineFunction &MF) const {
  return false;
}
