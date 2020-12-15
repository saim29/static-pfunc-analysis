#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/ADT/ilist.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/SymbolTableListTraits.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/LoopInfo.h" 
#include"llvm/IR/Dominators.h"
#include "llvm/IR/DebugLoc.h"

using namespace std;
using namespace llvm;

#ifndef ANALYSIS_H
#define ANALYSIS_H

namespace {
    
    class lkp_analysis : public ModulePass {

        typedef SmallVector<Function*, 16> functionList;

        public:
        static char ID;
        lkp_analysis() : ModulePass(ID) {}

        virtual void getAnalysisUsage(AnalysisUsage &AU) const;
        virtual bool doInitialization(Module &M);
        virtual bool runOnModule(Module &M);
        virtual bool doFinalization(Module &M);

        void analyzeTrusted(Function &F);
        void analyzeUntrusted(Function &F);
        bool is_mpt_begin(Instruction *I);
        bool is_mpt_end(Instruction *I);
        unsigned getSourceLocation(Instruction *I);

        private:
        functionList pWorkList;
        functionList workList;

    };
}

#endif