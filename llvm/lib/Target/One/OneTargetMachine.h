#ifndef LLVM_LIB_TARGET_ONE_ONETARGETMACHINE_H
#define LLVM_LIB_TARGET_ONE_ONETARGETMACHINE_H

#include "OneSubtarget.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class OneTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  OneSubtarget Subtarget;

public:
  OneTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options,
                   std::optional<Reloc::Model> RM,
                   std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                   bool JIT);

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
  const OneSubtarget *getSubtargetImpl() const  { return &Subtarget; }
  const OneSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
};

} // namespace llvm

#endif
