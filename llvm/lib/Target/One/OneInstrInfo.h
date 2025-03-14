#ifndef ONEINSTRINFO_H
#define ONEINSTRINFO_H
#include "MCTargetDesc/OneMCTargetDesc.h"

#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "OneGenInstrInfo.inc"

namespace llvm {
class OneInstrInfo : public OneGenInstrInfo {
public:
    explicit OneInstrInfo();
};
}

#endif
