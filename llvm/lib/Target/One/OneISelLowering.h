#ifndef ONEISELLOWERING_H
#define ONEISELLOWERING_H

#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
class OneSubtarget;
namespace OneISD {
enum NodeType : unsigned { FIRST_NUMBER = ISD::BUILTIN_OP_END, RET_GLUE, Call };
}
class OneTargetLowering : public TargetLowering {
  const OneSubtarget &Subtarget;

public:
  explicit OneTargetLowering(const TargetMachine &TM, const OneSubtarget &STI);

  const OneSubtarget &getSubtarget() const { return Subtarget; }

  SDValue LowerCall(CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  const char *getTargetNodeName(unsigned Opcode) const override;
};

} // namespace llvm

#endif
