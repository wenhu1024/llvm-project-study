
#include "TargetInfo/OneTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheOneTarget() {
    static Target TheOneTarget;
    return TheOneTarget;
}


extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOneTargetInfo() {
    RegisterTarget<Triple::one,
    /*HasJIT=*/false>
X(getTheOneTarget(), "one", "ONE (32-bit little endian)", "One");
}
