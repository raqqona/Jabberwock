#include "llvm/Transforms/Obfuscation/StackStrings.h"
#include "llvm/Transforms/Obfuscation/Utils.h"

#define DEBUG_TYPE "stacksting"

using namespace llvm;

STATISTIC(StackStringsed, "str value stackstringsed");

namespace {
struct StackString : public ModulePass {
  static char ID;
  bool flag;

  StackString() : ModulePass(ID) {}
  StackString(bool flag) : ModulePass(ID) { this->flag = flag; }

  bool stackstr(Module &M);
  void instrument_module(Module &M);

  bool runOnModule(Module &M) override {
    errs() << "Welcome to stackstrings" << "\n";
    errs() << "Running stackstring on " << M.getName() << "\n";
    if (stackstr(M)) {
      ++StackStringsed;
    }

    return false;
  } 
};
}

char StackString::ID = 0;
static RegisterPass<StackString> X("stackstring", "change str value stackstring");
Pass *llvm::createStackString(bool flag) { return new StackString(flag); }


bool StackString::stackstr(Module &M) {
  errs() << "StackStrings the module\n";
  errs() << *M.getGlobalVariable("alice", true) << "\n";
  llvm::IRBuilder<> Builder(M.getContext());
  for (auto &F : M) {
      errs() << "Function: " << F.getName() << "\n";
      for (auto &I : instructions(F)){
          if (isa<AllocaInst>(I)) {
              AllocaInst *alloca_inst = cast<AllocaInst>(&I); 
              errs() << alloca_inst->getName() << ":";
              StringRef var_name = F.getName().str() + "." + alloca_inst->getName().str();
              errs() << var_name << "\n";
              auto g_val = M.getGlobalVariable(var_name, true);
              if(g_val != nullptr){
                errs() << *g_val << "\n";
              }else{
                errs() << "not strings" << "\n";
              }
          }
      }
  }
  errs() << "Finished StackStrings the module\n";
  return true;
}