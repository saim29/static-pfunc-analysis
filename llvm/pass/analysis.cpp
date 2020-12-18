#include "analysis.h"

void lkp_analysis::getAnalysisUsage(AnalysisUsage &AU) const {

}

bool lkp_analysis::doInitialization(Module &M) {

    errs() << "Running the lkp compiler analysis pass on " << M.getName() << "\n";

}

bool lkp_analysis::runOnModule(Module &M) {

    // Get all private functions and normal functions
    for(Function &F: M){

        if (F.getName().contains("mpt_")) {
            
            //skip mpt functions
            continue;
        }
        else if (F.getName().contains("private_")) {

            pWorkList.push_back(&F);

        } else {

            workList.push_back(&F);
        }
    }

    for(auto pfunc: pWorkList){
        analyzeTrusted(*pfunc);
    }

    for(auto func: workList) {
        analyzeUntrusted(*func);
    }
}

bool lkp_analysis::doFinalization(Module &M) {

    errs() << "Exiting Analysis and printing stats\n";

}

bool lkp_analysis::backtrackStore(Value* var, set<Value*>& stackFrameVars) {

    // vector<Value*> stack;
    // stack.push_back(var);

    // while (!stack.empty()) {

    //     Value *cur = stack.back();
    //     stackFrameVars.insert(cur);
    //     stack.pop_back();

    //     for (auto u : cur->users()) {

    //         stack.push_back(u);
    //     }        
    // }

    // for (auto i = var->use_begin(), e = var->use_end(); i != e; ++i)
    // if (Instruction *Inst = dyn_cast<Instruction>(*i)) {
    //     errs() << "F is used in instruction:\n";
    //     errs() << *Inst << "\n";
    // }

    //check if var is gepop or bitcastop or simple instruction/load

    //enclose in while

    Value *back = var;
    while (back) {

        //back->dump();
        if (auto i = dyn_cast<GEPOperator>(back)) {

            //do stuff with gep
            back = i->getOperand(0);

        } else if (auto i = dyn_cast<BitCastOperator>(back)) {

            //do stuff with bitcast
            back = i->getOperand(0);

        } else if (auto i = dyn_cast<Instruction>(back)) {

            if (auto i = dyn_cast<AllocaInst>(back)) {

                //errs() << "here2\n";
                back = i;
                return false;

            } 

            //do stuff with inst
            back = i->getOperand(0);

        } else if (auto i = dyn_cast<GlobalValue>(back)){

            //errs() << "here\n";
            back = i;
            return true;

        } else {

            //errs() << "here3\n";
            return false;
        }
    }
}

void lkp_analysis::analyzeTrusted(Function &F) {

    //typedef SmallPtrSet<Value*, 32> localVars;
    typedef set<Value*> localVars;

    // data structure to store all variables on the local function's stack
    localVars stackFrameVars;
    //traverse from the start of the function to end of Allocas
    Instruction *f_start = &(F.front().front());

    while(isa<AllocaInst>(f_start)) {

        //insert the localVariables in the stackFrame data structure
        //getLocalVarAccesses(f_start, stackFrameVars);
        f_start = f_start->getNextNode();

    }

    //the instruction right after should be mpt_begin
    bool found = false;
    BasicBlock *first = &F.front();
    for (Instruction &I: *first) {

        if (!is_mpt_begin(&I)) {

            continue;

        } else {

            found = true;
            break;
        }
    }

    if (!found)
        errs() << "ERROR: mpt_begin required before the start of function: " << F.getName() << "\n";

    //analyze data leak
    for (BasicBlock &B: F) {

        for (Instruction &I: B) {

            //check for copy to memory outside the function
            if (StoreInst *l = dyn_cast<StoreInst>(&I)) {

                Value *to = l->getOperand(1);

                if (backtrackStore(to, stackFrameVars)) {

                    unsigned line = getSourceLocation(l);
                    errs() << "WARNING: data is being copied to non-local memory at " << line << " in function " << F.getName() << "\n";

                }

            } else if (CallInst *l = dyn_cast<CallInst>(&I)) {

                //check for a memcpy
                if (l->getCalledFunction()->getName().contains("llvm.memcpy")){

                    Value *argTo = l->getArgOperand(0);

                    if (backtrackStore(argTo, stackFrameVars)) {

                        unsigned line = getSourceLocation(l);
                        errs() << "WARNING: data is being copied to non-local memory at " << line << " in function " << F.getName() << "\n";
                    }
                }
            }
        }
    }

    //search for all returns in the function. They should have an mpt_end before them
    for (BasicBlock &B : F) {

        for (Instruction &I : B) {

            if (isa<ReturnInst>(&I)) {

                if (!backtrackRet(&I)) {

                    errs() << "ERROR: MPT_END EXPECTED AT THE END OF A FUNCTION\n";
                }
            }
        }
    }
}

void lkp_analysis::analyzeUntrusted(Function &F) {

    for (auto &B: F) {

        for (auto &I: B) {

            if(is_mpt_begin(&I)) {

                unsigned line = getSourceLocation(&I);

                errs() << "ERROR: mpks cannot be set in untrusted function " << F.getName() << " line " << line << "\n";
            }
        }
    }
}

bool lkp_analysis::is_mpt_begin(Instruction *I) {

    if (CallInst *l = dyn_cast<CallInst>(I)) {

        if (l->getCalledFunction()->getName().contains("mpt_begin")){  

            return true;
        }
    }

    return false;
}

bool lkp_analysis::is_mpt_end(Instruction *I) {
    
    if (CallInst *l = dyn_cast<CallInst>(I)) {

        if (l->getCalledFunction()->getName().contains("mpt_end")){  

            return true;
        }
    }

    return false;
}

unsigned lkp_analysis::getSourceLocation(Instruction *I) {

    DebugLoc dbgInfo = I->getDebugLoc();
    unsigned line = 0;

    if (dbgInfo){

        line = dbgInfo.getLine();

    }

    return line;
}

bool lkp_analysis::backtrackRet(Instruction *ret) {

    BasicBlock *b = ret->getParent();

    //backtrack from return instruction until the end of the basic block to ensure that mpt_end lies in the same execution pathl
    for (BasicBlock::iterator s = BasicBlock::iterator(ret), e = b->begin(); s != e; s--) {

        if (is_mpt_begin(&*s)) {

            errs() << "LOG: mpt_begin called before ret instruction\n";
            return false;

        } else if (is_mpt_end(&*s)){

            errs() << "LOG: mpt_end found before ret instruction\n";
            return true;
        }
    }
}

char lkp_analysis::ID = 1;
static RegisterPass<lkp_analysis> Y("lkp_analysis", "Module analysis for our custom LIBMPK applications");
