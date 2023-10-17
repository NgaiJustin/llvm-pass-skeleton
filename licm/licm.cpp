#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct LICMPass : public PassInfoMixin<LICMPass> {
  static bool isRequired() { return true; }

  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U) {
    
    // print loop info
    errs() << "Loop: " << L.getName() << "\n";
    errs() << "Loop depth: " << L.getLoopDepth() << "\n";
    errs() << "Loop header: " << L.getHeader()->getName() << "\n";
    errs() << "Loop preheader: " << L.getLoopPreheader()->getName() << "\n";
    errs() << "Loop basic blocks: " << L.getNumBlocks() << "\n";

    return PreservedAnalyses::none();
  }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {.APIVersion = LLVM_PLUGIN_API_VERSION,
          .PluginName = "LICM pass",
          .PluginVersion = "v0.2",
          .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerLoopOptimizerEndEPCallback(
                [](LoopPassManager &LPM, OptimizationLevel Level) {
                  LPM.addPass(LICMPass());
                });
          }};
}
