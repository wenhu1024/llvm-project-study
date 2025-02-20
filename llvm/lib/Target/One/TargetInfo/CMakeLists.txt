add_llvm_component_library(LLVMOneInfo
  OneTargetInfo.cpp

  LINK_COMPONENTS
  Support
  MC
)
