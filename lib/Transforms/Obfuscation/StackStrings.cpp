#include "llvm/Transforms/Obfuscation/StackStrings.h"
#include "llvm/Transforms/Obfuscation/Utils.h"

#define DEBUG_TYPE "stacksting"

using namespace llvm;

STATISTIC(StackStringsed, "str value stackstringsed");

namespace {
struct StackString : public FunctionPass {
  static char ID;
  bool flag;

  StackString() : FunctionPass(ID) {}
  StackString(bool flag) : FunctionPass(ID) { this->flag = flag; }

  bool runOnFunction(Function &F);
  bool stackstr(Function *f);
};
}

char StackString::ID = 0;
static RegisterPass<StackString> X("stackstring", "change str value stackstring");
Pass *llvm::createStackString(bool flag) { return new StackString(flag); }

bool StackString::runOnFunction(Function &F) {
  Function *tmp = &F;
  if (toObfuscate(flag, tmp, "ss")) {
    if (stackstr(tmp)) {
      ++StackStringsed;
    }
  }

  return false;
}

bool StackString::stackstr(Function *f) {
  errs() << "StackStrings: ";
  errs().write_escaped(f->getName()) << '\n';

  DEBUG_WITH_TYPE("opt", errs() << "ss: Started on function " << f->getName() << "\n");


  fixStack(f);

  return true;
}
