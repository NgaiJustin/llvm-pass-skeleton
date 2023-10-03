#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"

using namespace llvm;

namespace {

struct SkeletonPass : public PassInfoMixin<SkeletonPass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        for (auto &F : M) {
            // errs() << "I saw a function called " << F.getName() << "!\n";
            
            LLVMContext& Ctx = F.getContext();

            // F.getParent() is a module
            auto logFunc = F.getParent()->getOrInsertFunction(
                // name, return type, arg type
                "logop", Type::getVoidTy(Ctx), Type::getInt32Ty(Ctx)
            );

            for (auto &B : F) {
                // errs() << "\nBasic block.\n";

                for (auto &I : B) {
                    // errs() << "Instructions: " << I << "\n";
                    
                    if (auto *op = dyn_cast<BinaryOperator>(&I)) {
                        
                        // insert right before function by default
                        IRBuilder<> opBuilder(op);
                        IRBuilder<> logBuilder(op);

                        // newOp is SUB if ADD, MUL if SUB and ADD if MUL
                        Value *lhs = op->getOperand(0);
                        Value *rhs = op->getOperand(1);
                        
                        Value *newOp = (op->getOpcode() == Instruction::Add) 
                            ? opBuilder.CreateSub(lhs,rhs) : (op->getOpcode() == Instruction::Sub) 
                            ? opBuilder.CreateMul(lhs,rhs) : opBuilder.CreateAdd(lhs,rhs); 

                        // replace add instruction
                        for (auto &U : op->uses()){
                            User *user = U.getUser();
                            user->setOperand(U.getOperandNo(), newOp);
                        }

                        // set insertion point for log right after the op
                        logBuilder.SetInsertPoint(&B, ++logBuilder.GetInsertPoint());

                        Value* args = {op};
                        logBuilder.CreateCall(logFunc, args);
                    }
                }
            }
        }
        // return PreservedAnalyses::all();
        return PreservedAnalyses::none();  // we have mutated the code now
    };
};

}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "Skeleton pass",
        .PluginVersion = "v0.1",
        .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                    MPM.addPass(SkeletonPass());
                });
        }
    };
}
