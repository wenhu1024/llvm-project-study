#include "OneISelLowering.h"
#include "MCTargetDesc/OneMCTargetDesc.h"
#include "OneSubtarget.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "MCTargetDesc/OneMCExpr.h"


using namespace llvm;
#include "OneGenCallingConv.inc"

OneTargetLowering::OneTargetLowering(const TargetMachine &TM,
                                     const OneSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {

  addRegisterClass(MVT::i32, &One::GPRRegClass);

  setOperationAction(ISD::GlobalAddress,MVT::i32,Custom);
  setOperationAction(ISD::BR_CC, MVT::i32, Expand);
  computeRegisterProperties(STI.getRegisterInfo());
}

SDValue OneTargetLowering::LowerCall(CallLoweringInfo &CLI,
                                     SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc DL = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  CallingConv::ID CallConv = CLI.CallConv;
  bool IsVarArg = CLI.IsVarArg;

  MachineFunction &MF = DAG.getMachineFunction();

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_One);

  SmallVector<std::pair<unsigned, SDValue>> RegsPair;
  SDValue StackPtr;

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    if (VA.isRegLoc()) {
      RegsPair.push_back(std::make_pair(VA.getLocReg(), OutVals[i]));
    } else {
      assert(VA.isMemLoc());
      if (!StackPtr.getNode()) {
        StackPtr = DAG.getCopyFromReg(Chain, DL, One::SP,
                                      getPointerTy(DAG.getDataLayout()));
      }
      unsigned LocMemOffset = VA.getLocMemOffset();
      SDValue PtrOff = DAG.getIntPtrConstant(LocMemOffset, DL);
      PtrOff = DAG.getNode(ISD::ADD, DL, getPointerTy(DAG.getDataLayout()),
                           StackPtr, PtrOff);
      /// store val -> reg + offset
      Chain = DAG.getStore(Chain, DL, OutVals[i], PtrOff,
                           MachinePointerInfo::getStack(MF, LocMemOffset));
    }
  }

  GlobalAddressSDNode *N = dyn_cast<GlobalAddressSDNode>(Callee);
  MVT Ty = getPointerTy(DAG.getDataLayout());
  // Callee = DAG.getTargetGlobalAddress(N->getGlobal(), DL,
  //                                     getPointerTy(DAG.getDataLayout()));

  SDValue Hi = DAG.getTargetGlobalAddress(N->getGlobal(), DL, Ty, 0, OneMCExpr::HI);
  SDValue Lo = DAG.getTargetGlobalAddress(N->getGlobal(), DL, Ty, 0, OneMCExpr::LO);

  SDValue MHiNode = SDValue( DAG.getMachineNode(One::LUI, DL, Ty, Hi),0);
  Callee = SDValue(DAG.getMachineNode(One::ADDI, DL, Ty, MHiNode, Lo),0);

  
  SmallVector<SDValue, 8> Ops(1, Chain);
  Ops.push_back(Callee);

  SDValue Glue;

  for (unsigned i = 0, e = RegsPair.size(); i != e; ++i) {
    auto &[reg, val] = RegsPair[i];
    Chain = DAG.getCopyToReg(Chain, DL, reg, val, Glue);
    Glue = Chain.getValue(1);
    Ops.push_back(DAG.getRegister(reg, val.getValueType()));
  }

  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
  const uint32_t *Mask =
      TRI->getCallPreservedMask(DAG.getMachineFunction(), CallConv);
  Ops.push_back(DAG.getRegisterMask(Mask));
  if (Glue.getNode()) {
    Ops.push_back(Glue);
  }

  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  Chain = DAG.getNode(OneISD::Call, DL, NodeTys, Ops);

  {
    SmallVector<CCValAssign, 2> RVLocs;
    CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                   *DAG.getContext());
    CCInfo.AnalyzeCallResult(Ins, RetCC_One);

    SDValue Glue = Chain.getValue(1);
    for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
      CCValAssign &VA = RVLocs[i];
      assert(VA.isRegLoc());
      unsigned RVReg = VA.getLocReg();
      EVT vt = RVLocs[i].getLocVT();
      SDValue Val = DAG.getCopyFromReg(Chain, DL, RVReg, vt, Glue);
      Chain = Val.getValue(1);
      Glue = Val.getValue(2);
      InVals.push_back(Val);
    }
  }

  return Chain;
}

SDValue OneTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_One);

  SmallVector<SDValue, 8> Ops(1, Chain);

  SDValue ArgValue;

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    if (VA.isRegLoc()) {
      MVT RegVT = VA.getLocVT();
      Register Reg = MF.addLiveIn(VA.getLocReg(), &One::GPRRegClass);
      ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, RegVT);
      InVals.push_back(ArgValue);
    } else {
      assert(VA.isMemLoc());
      MVT ValVT = VA.getValVT();
      int Offset = VA.getLocMemOffset();
      int FI = MFI.CreateFixedObject(ValVT.getSizeInBits() / 8, Offset, true);
      SDValue FIN = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
      SDValue Val = DAG.getLoad(
          ValVT, DL, Chain, FIN,
          MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI));
      InVals.push_back(Val);
    }
  }

  return Chain;
}

SDValue
OneTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::OutputArg> &Outs,
                               const SmallVectorImpl<SDValue> &OutVals,
                               const SDLoc &DL, SelectionDAG &DAG) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeReturn(Outs, RetCC_One);

  SDValue Glue;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Glue);
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain;

  if (Glue.getNode()) {
    RetOps.push_back(Glue);
  }

  return DAG.getNode(OneISD::RET_GLUE, DL, MVT::Other, RetOps);
}

SDValue OneTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()){
  case ISD::GlobalAddress: {
    return LowerGlobalAddress(Op, DAG);
  }
  default:
    llvm_unreachable("unknown op");
  }
  return SDValue();
}

SDValue OneTargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const{
  EVT VT = Op.getValueType();
  GlobalAddressSDNode *N = dyn_cast<GlobalAddressSDNode>(Op);

  int64_t Offset = N->getOffset(); 

  SDLoc DL(N);
  SDValue Hi=DAG.getTargetGlobalAddress(N->getGlobal(), DL,VT,0,OneMCExpr::HI);
  SDValue Lo=DAG.getTargetGlobalAddress(N->getGlobal(), DL,VT,0,OneMCExpr::LO);
  
  SDValue MHiNode = SDValue(DAG.getMachineNode(One::LUI, DL, VT, Hi),0);
  SDValue BaseAddr = SDValue(DAG.getMachineNode(One::ADDI, DL, VT, MHiNode, Lo),0);

  if(Offset){
    return DAG.getNode(ISD::ADD, DL, VT, BaseAddr, DAG.getConstant(Offset, DL, VT));
  }

  return BaseAddr;
}

const char *OneTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case OneISD::RET_GLUE:
    return "OneISD::RET_GLUE";
  case OneISD::Call:
    return "OneISD::Call";
  case OneISD::HI:
    return "OneISD::HI";
  case OneISD::LO:
    return "OneISD::LO";
  default:
    return nullptr;
  }
}
