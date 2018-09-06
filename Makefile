CC = nvcc
SOURCEDIR = ./
SPLATTDIR = /people/liji541/Software/Install
CUDADIR = /share/apps/cuda/9.2.148

EXE = main

SOURCES = $(SOURCEDIR)/MTTKRP.cpp \
	      $(SOURCEDIR)/convert.cpp \
	      $(SOURCEDIR)/readtensor.cpp \
	      $(SOURCEDIR)/tensor.cpp

CU_SOURCES = $(SOURCEDIR)/gpuMTTKRP.cu	

IDIR = -I$(CUDADIR)/samples/common/inc -I$(SPLATTDIR)/include

LDIR = -L$(CUDADIR)/lib64 -L$(SPLATTDIR)/lib

H_FILES = $(wildcard *.h)
OBJS = $(SOURCES:.cpp=.o)

CU_OBJS=$(CU_SOURCES:.cu=.o)

# DOUBLEFLAGS = -DDOUBLE
DOUBLEFLAGS = 
TYPEFLAGS = -DCHAR
# TYPEFLAGS = -DLONG -DINT -DSHORT -DCHAR

CFLAGS = -O3 -std=c++11  -Xcompiler -fopenmp $(DOUBLEFLAGS) $(TYPEFLAGS) -Wno-deprecated-declarations

LFLAGS = $(LDIR) -lm -lstdc++ -lsplatt 
SMS ?= 60
# SMS ?= 35 37 50 52 60
#SMS ?= 20 30 35 37 50 52 60

ifeq ($(SMS),)
$(info >>> WARNING - no SM architectures have been specified - waiving sample <<<)
SAMPLE_ENABLED := 0
endif

ifeq ($(GENCODE_FLAGS),)
# Generate SASS code for each SM architecture listed in $(SMS)
$(foreach sm,$(SMS),$(eval GENCODE_FLAGS += -gencode arch=compute_$(sm),code=sm_$(sm)))

# Generate PTX code from the highest SM architecture in $(SMS) to guarantee forward-compatibility
HIGHEST_SM := $(lastword $(sort $(SMS)))
ifneq ($(HIGHEST_SM),)
GENCODE_FLAGS += -gencode arch=compute_$(HIGHEST_SM),code=compute_$(HIGHEST_SM)
endif
endif

NVCCFLGAS = -O3 -std=c++11 $(DOUBLEFLAGS) $(TYPEFLAGS) -Xptxas -dlcm=ca -Xptxas -v

$(EXE) : $(OBJS) $(CU_OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) $(GENCODE_FLAGS) -o $@ $?
$(SOURCEDIR)/%.o: $(SOURCEDIR)/%.cpp $(H_FILES)
	$(CC) $(CFLAGS) $(LFLAGS) $(IDIR) -c -o $@ $<
$(SOURCEDIR)/%.o: $(SOURCEDIR)/%.cu $(H_FILES)
	$(CC) $(NVCCFLGAS) $(LFLAGS) $(GENCODE_FLAGS) $(IDIR) -c -o $@ $<

clean:
	rm -f *.o main	
