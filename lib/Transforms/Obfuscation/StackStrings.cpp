#include "llvm/Transforms/Obfuscation/StackStrings.h"
#include "llvm/Transforms/Obfuscation/Utils.h"

#define DEBUG_TYPE "stackstring"

using namespace llvm;

STATISTIC(StackStringsed, "str value stackstringsed");

namespace {
struct StackString : public FunctionPass {
  static char ID;
  bool flag;

  StackString() : FunctionPass(ID) {}
  StackString(bool flag) : FunctionPass(ID) { this->flag = flag; }

  bool runOnFunction(Function &f);
  bool localStackstr(Function &F);
};
}
char StackString::ID = 0;
static RegisterPass<StackString> X("stackstring", "change str value stackstring");
Pass *llvm::createStackString(bool flag) { return new StackString(flag); }

bool StackString::runOnFunction(Function &F) {
  Function *tmp = &F;
  if (toObfuscate(flag, tmp, "ss")) {
    if (localStackstr(*tmp)) {
      ++StackStringsed;
    }
  }

  return false;
}
bool StackString::localStackstr(Function &F){
  
  Module *M = F.getParent();
  IRBuilder<> Builder(M->getContext());
  std::vector<GlobalVariable*> erase_gVar_list;
  std::vector<Instruction*> erase_inst_list;

  for (auto &I : instructions(F)){
    if (isa<AllocaInst>(I)) {
      AllocaInst *alloca_inst = cast<AllocaInst>(&I); 
      StringRef var_name = F.getName().str() + "." + alloca_inst->getName().str();
      auto g_val = M->getGlobalVariable(var_name, true);
      if(g_val != nullptr){ //detected global strings
        auto value_var = g_val->getOperand(0);
        const ConstantDataArray *data = dyn_cast<ConstantDataArray>(value_var);
        Builder.SetInsertPoint(I.getNextNode());
        for(auto id = 0; id < data->getNumElements(); ++id){ //insert ss instruction
          auto *dest_address = Builder.CreateInBoundsGEP(alloca_inst->getAllocatedType(), alloca_inst, {Builder.getInt32(0), Builder.getInt32(id)}, "arrayidx");
          Value *load_elem = data->getElementAsConstant(id);
          Builder.CreateStore(load_elem, dest_address);
        }
        auto *tmp_var = g_val;
        tmp_var->eraseFromParent();
      }
    }
    if (isa<CallInst>(I)){
      Function *tmp_func = cast<CallInst>(&I)->getCalledFunction();
      if(tmp_func->getName() == "llvm.memcpy.p0i8.p0i8.i64"){
        Instruction *rm_inst = &I;
        rm_inst->eraseFromParent();
      }
    }
  }
  return false;
}
