# If we don't need RTTI or EH, there's no reason to export anything
# from the hello plugin.


add_llvm_library(lkp MODULE
	analysis.cpp
	)
