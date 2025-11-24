class OneInst<dag outs, dag ins, string asmstr, list<dag> pattern> : Instruction {
    field bits<32> SoftFail=0;

    let Size=4;
    let Namespace="One";

    dag OutOperandList=outs;
    dag InOperandList=ins;
    let AsmString=asmstr;
    let Pattern=pattern;
}

class PseudoInst<dag outs, dag ins, list<dag> pattern,string asmstr="">
    : OneInst<outs, ins, asmstr, pattern> {
    let isPseudo=1;
    let isCodeGenOnly=1;
}

class I<dag outs, dag ins, string asmstr, list<dag> pattern>
    : OneInst<outs, ins, asmstr, pattern> {
}
