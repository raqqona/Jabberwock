#include "llvm/Transforms/Obfuscation/StackStrings.h"
#include "llvm/Transforms/Obfuscation/Utils.h"

#define DEBUG_TYPE "stackstring"

using namespace llvm;

STATISTIC(StackStringsed, "str value stackstringsed");

//namespace {
//struct StackString : public FunctionPass {
//  static char ID;
//  bool flag;
//
//  StackString() : FunctionPass(ID) {}
//  StackString(bool flag) : FunctionPass(ID) { this->flag = flag; }
//
//  bool runOnFunction(Function &f);
//  bool stackstr(Function &F);
//};
//}
//char StackString::ID = 0;
//static RegisterPass<StackString> X("stackstring", "change str value stackstring");
//Pass *llvm::createStackString(bool flag) { return new StackString(flag); }
//
//bool StackString::runOnFunction(Function &F) {
//  Function *tmp = &F;
//  if (toObfuscate(flag, tmp, "ss")) {
//    if (stackstr(*tmp)) {
//      ++StackStringsed;
//    }
//  }
//
//  return false;
//}
//bool StackString::stackstr(Function &F){
//  const ConstantDataArray *data;
//  const GlobalVariable *GR;
//  //ConstantDataArray *Add;
//  //GlobalVariable *G;
//  //Module * Mod;
//
//  IRBuilder<> builder(F.getContext());
//  for (inst_iterator idx = inst_begin(F), E = inst_end(F); idx != E; idx++){
//    if (isa<GetElementPtrInst>(&*idx)){
//      auto *GI = dyn_cast<GetElementPtrInst>(&*idx);
//      errs() << GI->getName().str() << ":";
//      errs() << *(GI->getOperand(0)) << "\n";
//      Value *LV = GI->getOperand(0);
//      if(GR->isConstant()){
//        data = dyn_cast<ConstantDataArray>(GR->getInitializer());
//        errs() << "Number of Elements we got: " << data->getNumElements() << "\n";
//      }else{
//        errs() << "not constant value\n";X
//      }
//      
//
//      
//
//      //for(const Value *Op : GI->operands()){
//      //    GR = dyn_cast<GlobalVariable>(GI->getOperand[0]);
//      //    if (isa<ConstantDataArray>(GR->getInitializer())){
//      //      data = dyn_cast<ConstantDataArray>(GR->getInitializer());
//      //      errs() << "Number of Elements we got: " << data->getNumElements() << "\n";
//      //    }
//      //  }
//      //}
//    }
//            //for(int k = 0, ed = data->getNumElements(); k < ed; ++k)
//            //  errs() << data->getElementAsInteger(k) << "\n";
//            //  Mod = F.getParent();
//            //  auto dude = Mod->getOrInsertGlobal("new_table", data->getType());, data->getNumElements()));
//            //  G = Mod->getNamedGlobal("new_table");
//            //  G->setLinkage(GlobalValue::LinkageTypes::CommonLinkage);
//            //  for(auto id = 0; id < data->getNumElements(); ++id){
//            //    auto srcAddress = builder.CreateGEP(data, std::vector<Value*>{ConstantInt::get(Type::getInt32Ty(Mod->getContext()), 0), id});
//            //    auto loadedElem = builder.CreateLoad(srcAddress);
//            //    auto destAddress = builder.CreateGEP(G, std::vector<Value*>{ConstantInt::get(Type::getInt32Ty(Mod->getContext()), 0), id});
//            //    builder.CreateStore(loadedElem, destAddress);
//            //  }
//            //}
//
//  }
//  return false;
//}



namespace {
struct StackString : public ModulePass {
  static char ID;
  bool flag;

  StackString() : ModulePass(ID) {}
  StackString(bool flag) : ModulePass(ID) { this->flag = flag; }

  bool stackstr(Module &M);

  bool runOnModule(Module &M) override {
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
  IRBuilder<> Builder(M.getContext());
  for (auto &F : M) {
    for (auto &I : instructions(F)){
      if (isa<AllocaInst>(I)) {
        AllocaInst *alloca_inst = cast<AllocaInst>(&I); 
        StringRef var_name = F.getName().str() + "." + alloca_inst->getName().str();
        auto g_val = M.getGlobalVariable(var_name, true);
        if(g_val != nullptr){
          auto value_val = g_val->getOperand(0);
          const ConstantDataArray *data = dyn_cast<ConstantDataArray>(value_val);
          StringRef str_val = data->getAsString();
          errs() << "we got: " << str_val << "\n";
          Builder.SetInsertPoint(I.getNextNode());
          for(auto id = 0; id < data->getNumElements(); ++id){
            auto *dest_address = Builder.CreateInBoundsGEP(alloca_inst->getAllocatedType(), alloca_inst, {Builder.getInt32(0), Builder.getInt32(id)}, "arrayidx");
            Value *load_elem = data->getElementAsConstant(id);
            //auto *a = Builder.CreateAlloca(Type::getInt32Ty(M.getContext()), nullptr, "a");
            //Builder.CreateStore(Builder.getInt32(data->getElementAsInteger(id)), a);
            //Builder.CreateStore(Builder.getInt32(data->getElementAsInteger(id)), dest_address);
            Builder.CreateStore(load_elem, dest_address);
          }
        }
      }
    }
  }
  return true;
}