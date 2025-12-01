include "OneInstrFormats.td"

def retglue : SDNode<"OneISD::RET_GLUE", SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def imm12 : Operand<i32>, ImmLeaf<i32, [{ return isInt<12>(Imm);}]>;


def mem : Operand<iPTR> {
  let MIOperandInfo = (ops GPR, imm12);
  let PrintMethod = "printMemOperand";
}

def memPat : ComplexPattern<iPTR, 2, "SelectAddrFI", [frameindex], [SDNPWantParent]>;

def LOADFI : OneInst<(outs GPR: $ra), (ins mem: $addr), "lw \t$ra, $addr", [(set GPR :$ra, (load memPat:$addr))]>;
def LOAD : OneInst<(outs GPR: $ra),(ins GPR: $rb), "lw \t$ra, 0($rb)",[(set GPR :$ra, (load GPR:$rb))]>;
//def : Pat<(load memPat:$addr), (LOADFI memPat:$addr)>;

def STOREFI : OneInst<(outs), (ins GPR: $ra, mem: $addr), "sw \t$ra, $addr", [(store GPR:$ra, memPat:$addr)]>;
def STORE : OneInst<(outs), (ins GPR: $ra, GPR: $rb), "sw \t$ra, 0($rb)", [(store GPR:$ra, GPR:$rb)]>;
//def : Pat<(store GPR:$ra, memPat:$addr), (STOREFI GPR:$ra, memPat:$addr)>;

class AtrithLogicalI<string inst, SDNode node> : I<
  (outs GPR:$rd),
  (ins GPR:$rs1, imm12:$imm12),
  !strconcat(inst, "\t$rd, $rs1, $imm12"),
  [(set GPR:$rd, (node GPR:$rs1, imm12:$imm12))]
  >;

def ADDI : AtrithLogicalI<"addi", add>;
def ANDI : AtrithLogicalI<"andi", and>;
def ORI  : AtrithLogicalI<"ori",  or>;
def XORI : AtrithLogicalI<"xori", xor>;
def SLLI : AtrithLogicalI<"slli", shl>;
def SRLT : AtrithLogicalI<"srli", srl>;
def SRAI : AtrithLogicalI<"srai", sra>;

// def ADDI : I<(outs GPR:$rd),
//             (ins GPR:$rs1, imm12: $imm12),
//             "addi $rd, $rs1, $imm12",
//             [(set GPR:$rd,(add GPR: $rs1, imm12: $imm12))]>;

class AtrithLogicalR<string inst, SDNode node> : I<
  (outs GPR:$rd),
  (ins GPR:$rs1, GPR:$rs2),
  !strconcat(inst, "\t$rd, $rs1, $rs2"),
  [(set GPR:$rd, (node GPR:$rs1, GPR:$rs2))]
  >;

def ADD : AtrithLogicalR<"add", add>;
def SUB : AtrithLogicalR<"sub", sub>; 
def MUL : AtrithLogicalR<"mul", mul>;
def DIV : AtrithLogicalR<"div", sdiv>;
def REM : AtrithLogicalR<"rem", srem>;
def AND : AtrithLogicalR<"and", and>;
def OR  : AtrithLogicalR<"or",  or>;
def XOR : AtrithLogicalR<"xor", xor>;
def SLL : AtrithLogicalR<"sll", shl>;
def SRL : AtrithLogicalR<"srl", srl>;
def SRA : AtrithLogicalR<"sra", sra>;



// def ADD : I<(outs GPR:$rd),
//             (ins GPR:$rs1, GPR: $rs2),
//             "add $rd, $rs1, $rs2",
//             [(set GPR:$rd,(add GPR: $rs1, GPR: $rs2))]>;

def : Pat<(i32 (imm12:$imm12)), (ADDI ZERO, imm12:$imm12)>;

let isCall = 1 in {
  def JALR :  I<(outs GPR:$rd),
              (ins GPR:$rs1, imm12:$imm12),
              "jalr $rd, $rs1, $imm12",
              []>;
}

let isBarrier = 1, isReturn = 1, isTerminator = 1 in {
  def PseudoRET : PseudoInst<
              (outs),
              (ins),
              [(retglue)]>,
              PseudoInstExpansion<(JALR ZERO, RA, 0)>;
}

def OneSDT_Call : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def one_call : SDNode<"OneISD::Call", OneSDT_Call, [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>; 

def call_symbol : Operand<iPTR>;
def CALL : I<(outs),(ins call_symbol:$rs1), "call\t $rs1", [(one_call tglobaladdr:$rs1)]>{
  let isCall = 1;
}

def : InstAlias<"ret", (JALR ZERO, RA, 0)>;
def : InstAlias<"li $rd, $imm",  (ADDI GPR:$rd, ZERO, imm12:$imm)>;
