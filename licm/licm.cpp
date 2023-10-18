#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/LoopUtils.h"

using namespace llvm;

namespace {

struct LICMPass : public PassInfoMixin<LICMPass> {
  static bool isRequired() { return true; }

  // bool isLoopInvariant(const Loop *L, const Value *V) const {
  //   if (const Instruction *I = dyn_cast<Instruction>(V))
  //     return !L->contains(I);
  //   return true; // All non-instructions are loop invariant
  // }

  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U) {

    // get loop preheader block's terminator
    BasicBlock *preheader = L.getLoopPreheader();
    if (!preheader) {
      return PreservedAnalyses::all();
    }
    Instruction *preheaderTerm = preheader->getTerminator();

    bool converged = false;

    while (not converged) {

      converged = true;
      for (auto &BB : L.getBlocks()) {

        // set of loop invariant instructions to move
        std::vector<Instruction *> toMove;

        // MemorySSAUpdater MSSAU = MemorySSAUpdater(AR.MSSA);
        // SinkAndHoistLICMFlags Flags = {false, 0,
        //                          LicmMssaOptCap, LicmMssaNoAccForPromotionCap,
        //                          /*IsSink=*/true};

        for (auto &I : *BB) {

          // // if contains side effects or undefined behavior, not loop
          // invariant if (!isSafeToSpeculativelyExecute(&I)) {
          //   errs() << "Not safe to execute: " << I << "\n";
          //   continue;
          // }

          // // if may read from memory, not loop invariant
          // if (I.mayReadFromMemory()) {
          //   errs() << "May read from memory: " << I << "\n";
          //   continue;
          // }

          // bool isInvariant = true;

          // // if any args are not loop invariant, not loop invariant
          // for (int i = 0; i < I.getNumOperands(); ++i) {
          //   Value *operand = I.getOperand(i);
          //   if (!isLoopInvariant(&L, operand)) {
          //     // definition is in loop, not loop invariant
          //     isInvariant = false;
          //     break;
          //   }
          // }
          if (L.hasLoopInvariantOperands(&I)) {
            bool changed = false;
            L.makeLoopInvariant(&I, changed);
            if (changed) {
              errs() << "Hoisted: " << I << "\n";
              converged = false;
            }
          }

          // if (isInvariant) {
          //   toMove.push_back(&I);
          // } 
          
          // else if (canSinkOrHoistInst(I, &AR.AA, &AR.DT, &L, MSSAU, true, flags)) {
          //   errs() << "Can sink or hoist: " << I << "\n";
          //   toMove.push_back(&I);
          // } 
        }

        // // move instructions
        // for (auto &I : toMove) {
        //   errs() << "Moving: " << *I << "\n";
        //   // I->moveBefore(preheaderTerm);
        //   converged = false;
        // }
      }
    }

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
