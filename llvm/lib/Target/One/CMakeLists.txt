add_llvm_component_group(One)

add_llvm_target(OneCodeGen

    OneTargetMachine.cpp

    LINK_COMPONENTS
    Support
    OneDesc
    OneInfo
)

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
