#ifndef ONE_H
#define ONE_H

#define DIV_ROUND_UP(n, d)  (((n) + (d) - 1) / (d))
#define ROUND_UP(x, align)  (DIV_ROUND_UP(x, align) * (align))

namespace llvm {
class FunctionPass;
class OneTargetMachine;
class PassRegistry;

FunctionPass *createOneISelDag(OneTargetMachine &TM);

void initializeOneDAGToDAGISelLegacyPass(PassRegistry &);

}

#endif