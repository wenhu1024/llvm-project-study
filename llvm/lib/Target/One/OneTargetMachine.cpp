#include "OneTargetMachine.h"
#include "One.h"
#include "TargetInfo/OneTargetInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"

#define DEBUG_TYPE "one"

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOneTarget() {
  RegisterTargetMachine<OneTargetMachine> X(getTheOneTarget());
}

static StringRef computeDataLayout(const Triple &TT,
                                   const TargetOptions &Options) {

  assert(TT.isArch32Bit() && "only support 32bit !");
  return "e-m:e-p:32:32-i64:64-n32-S128";
}

static Reloc::Model getEffectiveReloadModel(const Triple &TT,
                                            std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::Static);
}

OneTargetMachine::OneTargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   std::optional<Reloc::Model> RM,
                                   std::optional<CodeModel::Model> CM,
                                   CodeGenOptLevel OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT, Options), TT, CPU, FS, Options,
                        getEffectiveReloadModel(TT, RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()),
      Subtarget(TT, CPU, FS, *this) {
  initAsmInfo();
}

namespace {
class OnePassConfig : public TargetPassConfig {
public:
  OnePassConfig(OneTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  OneTargetMachine &getOneTargetMachine() const {
    return getTM<OneTargetMachine>();
  }

  const OneSubtarget &getOneSubtarget() const {
    return *getOneTargetMachine().getSubtargetImpl();
  }

  bool addInstSelector() override;
};
} // namespace

TargetPassConfig *OneTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new OnePassConfig(*this, PM);
}

bool OnePassConfig::addInstSelector() {
  addPass(createOneISelDag(getOneTargetMachine()));
  return false;
}
