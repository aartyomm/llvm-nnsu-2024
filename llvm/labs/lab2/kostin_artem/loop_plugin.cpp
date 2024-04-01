#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

struct LoopStartEnd : public PassInfoMixin<LoopStartEnd> {
  PreservedAnalyses run(Function &Pfunction,
                        FunctionAnalysisManager &Panalysis) {

    FunctionType *LoopFuncType =
        FunctionType::get(Type::getVoidTy(Pfunction.getContext()), false);
    Module *ModuleOfPfuncParent = Pfunction.getParent();

    LoopAnalysis::Result &GetAnalysis =
        Panalysis.getResult<LoopAnalysis>(Pfunction);
    for (auto &Loop : GetAnalysis) {
      BasicBlock *EntryBlock = Loop->getLoopPreheader();
      IRBuilder<> FuncBuilder(Loop->getHeader()->getContext());
      if (EntryBlock != nullptr) {
        Instruction *FirstInst = EntryBlock->getFirstNonPHI();
        if (isa<CallInst>(FirstInst) &&
            cast<CallInst>(FirstInst)->getCalledFunction()->getName() ==
                "loop_start") {
          continue;
        }
        FuncBuilder.SetInsertPoint(EntryBlock->getTerminator());
        FuncBuilder.CreateCall(ModuleOfPfuncParent->getOrInsertFunction(
            "loop_start", LoopFuncType));
      }
      SmallVector<BasicBlock *, 4> ExitBlocks;
      Loop->getExitBlocks(ExitBlocks);
      for (BasicBlock *ExitBlock : ExitBlocks) {
        Instruction *LastInst = ExitBlock->getTerminator();
        if (isa<CallInst>(LastInst) &&
            cast<CallInst>(LastInst)->getCalledFunction()->getName() ==
                "loop_end") {
          continue;
        }
        FuncBuilder.SetInsertPoint(&*ExitBlock->getFirstInsertionPt());
        FuncBuilder.CreateCall(
            ModuleOfPfuncParent->getOrInsertFunction("loop_end", LoopFuncType));
      }
    }
    return PreservedAnalyses::all();
  }
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopStartEndPlugin", LLVM_VERSION_STRING,
          [](llvm::PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager &PM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "loop-start-end") {
                    PM.addPass(LoopStartEnd());
                    return true;
                  }
                  return false;
                });
          }};
}
