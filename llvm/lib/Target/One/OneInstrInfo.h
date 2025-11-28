#ifndef ONEINSTRINFO_H
#define ONEINSTRINFO_H
#include "MCTargetDesc/OneMCTargetDesc.h"

#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "OneGenInstrInfo.inc"

namespace llvm {
class OneInstrInfo : public OneGenInstrInfo {
public:
  explicit OneInstrInfo();

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI, Register SrcReg,
                           bool isKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI,
                           Register VReg) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MI, Register DestReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI,
                            Register VReg) const override;

  void copyPhysReg(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MI, const DebugLoc &DL,
                           MCRegister DestReg, MCRegister SrcReg, bool KillSrc,
                           bool RenamableDest = false,
                           bool RenamableSrc = false) const override;
};
} // namespace llvm
#endif
