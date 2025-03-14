#ifndef ONEASMPRINTER_H
#define ONEASMPRINTER_H

#include "OneTargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class OneSubtarget;
class OneAsmPrinter : public AsmPrinter {
public:
  const OneSubtarget *Subtarget;

  explicit OneAsmPrinter(TargetMachine &TM,
                         std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {
    Subtarget = static_cast<OneTargetMachine &>(TM).getSubtargetImpl();
  }

  StringRef getPassName() const override { return "One Assembly Printer"; }

  virtual bool runOnMachineFunction(MachineFunction &MF) override;

  void emitInstruction(const MachineInstr *MI) override;

private:
  bool lowerPseudoInstExpansion(const MachineInstr *MI, MCInst &Inst);
  void lowerToMCInst(const MachineInstr *MI,MCInst &Out);
};
} // namespace llvm

#endif
