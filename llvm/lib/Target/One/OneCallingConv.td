def RetCC_One : CallingConv<[
    CCIfType<[i32], CCAssignToReg<[A0, A1]>>,
]>;

def CC_Save : CalleeSavedRegs<(add RA, S0, S1)>;
