#Make file for basic lkp_analysis

OUT = $(ex).bc $(ex).ll

LLVM_DIR = /home/saim/Desktop/llvm-project/build
lkp_analysis = llvm/pass/lkp_analysis.so

CLANG = $(LLVM_DIR)/bin/clang
DIS = $(LLVM_DIR)/bin/llvm-dis
OPT = $(LLVM_DIR)/bin/opt
LLC = $(LLVM_DIR)/bin/llc

VPATH = tests 

CFLAGS = \
	-isysroot /none \
	-g \
	-std=c99 \
	-Wall \
	-MD \

all: $(OUT)

$(ex).bc: $(ex).c 
	$(CLANG) -emit-llvm -c -O -Xclang -disable-llvm-passes -fheinous-gnu-extensions $(CFLAGS) $^

$(ex).ll: $(ex).bc
	$(DIS) $^
	$(OPT) -load ../bld/pass/lkp.so --lkp_analysis -time-passes $@

clean:

	$(RM) -f *.ll *.bc *d
