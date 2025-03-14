#include "OneISelLowering.h"
#include "MCTargetDesc/OneMCTargetDesc.h"
#include "OneSubtarget.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"

using namespace llvm;
#include "OneGenCallingConv.inc"

OneTargetLowering::OneTargetLowering(const TargetMachine &TM,
                                     const OneSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {

  addRegisterClass(MVT::i32, &One::GPRRegClass);
  computeRegisterProperties(STI.getRegisterInfo());
}

SDValue OneTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  return Chain;
}

SDValue
OneTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::OutputArg> &Outs,
                               const SmallVectorImpl<SDValue> &OutVals,
                               const SDLoc &DL, SelectionDAG &DAG) const 
{
    SmallVector<CCValAssign,16> RVLocs;
    CCState CCInfo(CallConv, IsVarArg,DAG.getMachineFunction(),RVLocs,*DAG.getContext());
    CCInfo.AnalyzeReturn(Outs, RetCC_One);

    SDValue Glue;
    SmallVector<SDValue, 4>RetOps(1,Chain);

    for (unsigned i=0,e=RVLocs.size();i<e;++i){
        CCValAssign &VA=RVLocs[i];
        assert(VA.isRegLoc()&&"Can only return in registers!");

        Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Glue);
        Glue=Chain.getValue(1);
        RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    }

    RetOps[0]=Chain;

    if (Glue.getNode()){
        RetOps.push_back(Glue);
    }

    return DAG.getNode(OneISD::RET_GLUE,DL,MVT::Other,RetOps);
                               
}
