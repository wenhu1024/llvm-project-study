class OneReg<string n> : Register<n> {
    let Namespace = "One";
}

def ZERO  : OneReg<"zero">, DwarfRegNum<[0]>;
def RA  : OneReg<"ra">, DwarfRegNum<[1]>;
def SP  : OneReg<"sp">, DwarfRegNum<[2]>;
def T0  : OneReg<"t0">, DwarfRegNum<[5]>;
def T1  : OneReg<"t1">, DwarfRegNum<[6]>;
def T2  : OneReg<"t2">, DwarfRegNum<[7]>;
def S0  : OneReg<"s0">, DwarfRegNum<[8]>;
def S1  : OneReg<"s1">, DwarfRegNum<[9]>;
def A0  : OneReg<"a0">, DwarfRegNum<[10]>;
def A1  : OneReg<"a1">, DwarfRegNum<[11]>;
def A2  : OneReg<"a2">, DwarfRegNum<[12]>;

def GPR : RegisterClass<"One", [i32], 32, (add
    ZERO, RA, SP,
    A0, A1, A2,
    T0, T1, T2,
    S0, S1
)>;
