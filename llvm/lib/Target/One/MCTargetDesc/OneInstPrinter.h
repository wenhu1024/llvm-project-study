#ifndef ONEINSTPRINTER_H
#define ONEINSTPRINTER_H

#include "MCTargetDesc/OneMCTargetDesc.h"
#include "llvm/MC/MCInstPrinter.h"

namespace llvm {
class OneInstPrinter : public MCInstPrinter {
public:
  OneInstPrinter(const MCAsmInfo &MAI, const MCInstrInfo &MII,
                 const MCRegisterInfo &MRI)
      : MCInstPrinter(MAI, MII, MRI) {}

  void printInstruction(const MCInst *MI, uint64_t Address, raw_ostream &O);
  static const char *getRegisterName(MCRegister Reg);
  static const char *getRegisterName(MCRegister Reg,unsigned AltIdx);

  void printRegName(raw_ostream &OS, MCRegister Reg) override;
  void printInst(const MCInst *MI, uint64_t Address, StringRef Annot,
                 const MCSubtargetInfo &STI, raw_ostream &O) override;

  bool printAliasInstr(const MCInst *MI, uint64_t Address, raw_ostream &OS);
  void printCustomAliasOperand(const MCInst *MI, uint64_t Address,
                               unsigned OpIdx, unsigned PrintMethodIdx,
                               raw_ostream &O);
  std::pair<const char *, uint64_t>
  getMnemonic(const MCInst &MI) const override;

private:
  void printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O);
  void printImmediate(const MCInst *MI, unsigned opNum, raw_ostream &O);
};
} // namespace llvm

#endif
