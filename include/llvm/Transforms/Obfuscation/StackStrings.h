#ifndef _STACKSTRINGS_INCLUDES_
#define _STACKSTRINGS_INCLUDES_


#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Transforms/Utils/Local.h" // For DemoteRegToStack and DemotePHIToStack
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"

// Namespace
using namespace std;

namespace llvm {
	Pass *createStackString();
	Pass *createStackString(bool flag);
}

#endif

