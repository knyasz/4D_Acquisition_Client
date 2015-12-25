

OSUPPER = $(shell uname -s 2>/dev/null | tr "[:lower:]" "[:upper:]")
#OSUPPER = LINUX
OSLOWER = $(shell uname -s 2>/dev/null | tr "[:upper:]" "[:lower:]")
#OSLOWER = linux

OS_SIZE = $(shell uname -m | sed -e "s/i.86/32/" -e "s/x86_64/64/" -e "s/armv7l/32/")
OS_ARCH = $(shell uname -m | sed -e "s/i386/i686/")

#OS_SIZE = 32
#OS_ARCH = armv7l


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
DARWIN = $(strip $(findstring DARWIN, $(OSUPPER)))
ifneq ($(DARWIN),)
	XCODE_GE_5 = $(shell expr `xcodebuild -version | grep -i xcode | awk '{print $$2}' | cut -d'.' -f1` \>= 5)
endif
ifneq ($(DARWIN),)
	ifeq ($(XCODE_GE_5),1)
	  GCC ?= clang
	else
	  GCC ?= g++
	endif
else
  GCC ?= g++
endif

INCLUDES  := -I/usr/local/include/libfreenect 
LD_LIBS_LOCATION := -L/usr/local/lib
LD_LIBS :=  -lfreenect  -lopencv_core -lopencv_highgui -lopencv_imgproc -lpthread -lX11
LIBRARIES := $(LD_LIBS_LOCATION) $(LD_LIBS) \


# Location of the CUDA Toolkit
CUDA_PATH       ?= /usr/local/cuda-6.0
NVCC := $(CUDA_PATH)/bin/nvcc -ccbin $(GCC)

# Common includes and paths for CUDA
INCLUDES += -I/usr/local/include/opencv
ifeq ($(OS_ARCH),armv7l)
	LD_CUDALIBS_LOCATION := -L/usr/local/cuda/lib
else
	LD_CUDALIBS_LOCATION := -L/usr/local/cuda/lib64
endif
LD_CUDALIBS := -lcudart -lnpps -lnppi -lnppc -lcufft
LIBRARIES += $(LD_CUDALIBS_LOCATION) $(LD_CUDALIBS) 



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
dbg = 1
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


################################################################################

# CUDA code generation flags
ifneq ($(OS_ARCH),armextrv7l)
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
GENCODE_FLAGS   ?= $(GENCODE_SM1-std=c++110) \
					$(GENCODE_SM20) \
					$(GENCODE_SM30) \
					$(GENCODE_SM32) \
					$(GENCGCCODE_SM35) \
					$(GENCODE_SM50) \
					$(GENCODE_SMXX)
endif

################################################################################

ifeq ($(OS_ARCH),armv7l)
	UDP_SOCKET := udpSocket.a 
else
	UDP_SOCKET := udpSocket64bit.a 
endif

OBJS=$(patsubst %.cpp,%.o,$(wildcard *.cpp))
OBJS+=$(patsubst %.cu,%.o,$(wildcard *.cu))
#OBJS+=$(UDP_SOCKET)

CFLAGS= -std=c++11 -fPIC -g -Wall
 

# Target rules
all:build

build:Client



%.o:%.cpp
	@$(GCC) $(CFLAGS) $(INCLUDES) -o $@ -c $< $(LIBRARIES)
%.o:%.cu
	@$(NVCC) $(ALL_CCFLAGS) $(GENCODE_FLAGS) -o $@ -c $< $(LIBRARIES)
 
Client:$(OBJS)
#	@$(NVCC)$(ALL_LDFLAGS) $(GENCODE_FLAGS) -o $@ $+ $(LIBRARIES) $(UDP_SOCKET)
	@$(NVCC)$(ALL_LDFLAGS) $(GENCODE_FLAGS) -o $@ $+ $(LIBRARIES)

run:build
	./Client

clean:
	rm -f Client *.o 
clobber:clean