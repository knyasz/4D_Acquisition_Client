
# Location of the CUDA Toolkit
CUDA_PATH       ?= /usr/local/cuda-6.0

OSUPPER = $(shell uname -s 2>/dev/null | tr "[:lower:]" "[:upper:]")
OSLOWER = $(shell uname -s 2>/dev/null | tr "[:upper:]" "[:lower:]")

OS_SIZE = $(shell uname -m | sed -e "s/i.86/32/" -e "s/x86_64/64/" -e "s/armv7l/32/")
OS_ARCH = $(shell uname -m | sed -e "s/i386/i686/")

#OS_SIZE = 32
#OS_ARCH = armv7l

DARWIN = $(strip $(findstring DARWIN, $(OSUPPER)))
ifneq ($(DARWIN),)
	XCODE_GE_5 = $(shell expr `xcodebuild -version | grep -i xcode | awk '{print $$2}' | cut -d'.' -f1` \>= 5)
endif

# Take command line flags that override any of these settings
#ifeq ($(i386),1)
#	OS_SIZE = 32
#	OS_ARCH = i686
#endif
#ifeq ($(x86_64),1)
#	OS_SIZE = 64
#	OS_ARCH = x86_64
#endif
#ifeq ($(ARMv7),1)
#	OS_SIZE = 32
#	OS_ARCH = armv7l
#endif

# Common binaries
ifneq ($(DARWIN),)
	ifeq ($(XCODE_GE_5),1)
	  GCC ?= clang
	else
	  GCC ?= g++
	endif
else
  GCC ?= g++
endif

NVCC := $(CUDA_PATH)/bin/nvcc -ccbin $(GCC)

# internal flags
NVCCFLAGS   := -m${OS_SIZE}
CCFLAGS     :=
LDFLAGS     :=

# Extra user flags
EXTRA_NVCCFLAGS   ?= 
EXTRA_LDFLAGS     ?=	
EXTRA_CCFLAGS     ?=	

# OS-specific build flags
ifneq ($(DARWIN),)
  LDFLAGS += -rpath $(CUDA_PATH)/lib
  CCFLAGS += -arch $(OS_ARCH)
else
  ifeq ($(OS_ARCH),armv7l)
    ifeq ($(abi),gnueabi)
      CCFLAGS += -mfloat-abi=softfp
    else
      # default to gnueabihf
      override abi := gnueabihf
      LDFLAGS += --dynamic-linker=/lib/ld-linux-armhf.so.3
      CCFLAGS += -mfloat-abi=hard
    endif
  endif
endif

NVCCFLAGS += -target-cpu-arch ARM

ifneq ($(TARGET_FS),)
CCFLAGS += --sysroot=$(TARGET_FS)
LDFLAGS += --sysroot=$(TARGET_FS)
LDFLAGS += -rpath-link=$(TARGET_FS)/lib
LDFLAGS += -rpath-link=$(TARGET_FS)/usr/lib
LDFLAGS += -rpath-link=$(TARGET_FS)/usr/lib/arm-linux-$(abi)
endif


# Debug build flags
ifeq ($(dbg),1)
      NVCCFLAGS += -g -G
      TARGET := debug
else
      TARGET := release
endif

ALL_CCFLAGS :=
ALL_CCFLAGS += $(NVCCFLAGS)
ALL_CCFLAGS += $(EXTRA_NVCCFLAGS)
ALL_CCFLAGS += $(addprefix -Xcompiler ,$(CCFLAGS))
ALL_CCFLAGS += $(addprefix -Xcompiler ,$(EXTRA_CCFLAGS))

ALL_LDFLAGS :=
ALL_LDFLAGS += $(ALL_CCFLAGS)
ALL_LDFLAGS += $(addprefix -Xlinker ,$(LDFLAGS))
ALL_LDFLAGS += $(addprefix -Xlinker ,$(EXTRA_LDFLAGS))

# Common includes and paths for CUDA
INCLUDES  := -I../../common/inc -I/usr/local/include/libfreenect -I/usr/include/opencv  -I/usr/local/include/opencv
#LIBRARIES := -L/usr/local/lib -lfreenect udpSocket.a -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -L/usr/local/cuda/lib -lcudart -lnpps -lnppi -lnppc -lcufft
LIBRARIES := -L/usr/local/lib -lpthread -lfreenect udpSocket64bit.a -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -L/usr/local/cuda/lib64 -lcudart -lnpps -lnppi -lnppc -lcufft
################################################################################

# CUDA code generation flags
ifneq ($(OS_ARCH),armv7l)
GENCODE_SM10    := -gencode arch=compute_10,code=sm_10
endif
GENCODE_SM20    := -gencode arch=compute_20,code=sm_20
GENCODE_SM30    := -gencode arch=compute_30,code=sm_30
GENCODE_SM32    := -gencode arch=compute_32,code=sm_32
GENCODE_SM35    := -gencode arch=compute_35,code=sm_35
GENCODE_SM50    := -gencode arch=compute_50,code=sm_50
GENCODE_SMXX    := -gencode arch=compute_50,code=compute_50
ifeq ($(OS_ARCH),armv7l)
GENCODE_FLAGS   ?= $(GENCODE_SM32)
else
GENCODE_FLAGS   ?= $(GENCODE_SM10) $(GENCODE_SM20) $(GENCODE_SM30) $(GENCODE_SM32) $(GENCODE_SM35) $(GENCODE_SM50) $(GENCODE_SMXX)
endif

################################################################################

# Target rules
all:build

build:test

test.o:test.cpp
#	$(NVCC) $(INCLUDES) $(ALL_LDFLAGS) $(GENCODE_FLAGS) -o $@ -c $^ $(LIBRARIES)
	g++ $(INCLUDES) -o $@ -c $< 
	
dtmGpu.o:dtmGpu.cu
	$(NVCC) $(INCLUDES) $(ALL_CCFLAGS) $(GENCODE_FLAGS) -o $@ -c $<
	
MyFreenectDevice.o:MyFreenectDevice.cpp
#	$(NVCC) $(INCLUDES) $(ALL_LDFLAGS) $(GENCODE_FLAGS) -o $@ -c $^ $(LIBRARIES)
	g++ $(INCLUDES) -o $@ -c $< 
test:dtmGpu.o test.o MyFreenectDevice.o
#	$(NVCC) $(INCLUDES) $(ALL_LDFLAGS) $(GENCODE_FLAGS) -o $@ $+ $(LIBRARIES)
#	mkdir -p ../../bin/$(OS_ARCH)/$(OSLOWER)/$(TARGET)$(if $(abi),/$(abi))
#	cp $@ ../../bin/$(OS_ARCH)/$(OSLOWER)/$(TARGET)$(if $(abi),/$(abi))
	g++ $(INCLUDES) -o $@ $^ $(LIBRARIES)

run:build
	./test

clean:
	rm -f test *.o 
	rm -rf /bin/$(OS_ARCH)/$(OSLOWER)/$(TARGET)$(if $(abi),/$(abi))/test
clobber:clean

