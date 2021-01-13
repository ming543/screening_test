
# Target
TARGETNAME := DMCT
TARGET1NAME := DMCT_IO
TARGET2NAME := DMCT_WDT
TARGET3NAME := DMCT_FAN
#TARGETLIBNAME
TARGETLIB1NAME := DMCI
TARGETLIB2NAME := DDMI
#TARGETTYPE := DYNLINK
TARGETLIBMAINVER := 0
TARGETLIBMINORVER := 5.0
#BUILD := debug
BUILD ?= release

# Target Folders
TARGETDIR := ${BUILD}/
BUILDDIR := build/

# GCC Envirument Detection
#   MACHINE : i386 i486 i586 i686 x86_64 ..
#MACHINE := $(shell arch)
#MACHINE := $(shell uname -m)
#   ARCH : 32 64
ARCH := $(shell getconf LONG_BIT)
ifeq (${ARCH},32)
	MX32 :=
else
	MX32 := -m32 -L/usr/lib32
endif
ifeq (${ARCH},64)
	MX64 :=
else
	MX64 := -m64 -L/usr/lib64
endif

ifndef TARGETTYPE
	TARGETTYPE := ALL
endif
ifndef MCUS
	#MCUS := ${ARCH}
	MCUS := 32 64
endif

TARGETLIBSNAME := $(foreach i,1 2,${TARGETLIB${i}NAME})
TARGETSNAME := $(foreach i,1 2 3,${TARGET${i}NAME}) ${TARGETNAME}
#TARGETSNAME := $(foreach i,1 2 3,${TARGET${i}NAME})
TARGETDIRS := $(foreach m,${MCUS},${TARGETDIR:%=%X${m}/})
TARGET32DIR := ${TARGETDIR}X32/
TARGET64DIR := ${TARGETDIR}X64/
BUILDDIRS := $(foreach m,${MCUS},${BUILDDIR:%=%X${m}/})
BUILD32DIR := ${BUILDDIR}X32/
BUILD64DIR := ${BUILDDIR}X64/
INSTLIBDIR1 := /usr/local/lib/
INSTLIBDIR2 := /usr/lib/
INSTLIBDIR3 := /lib/

# _C : .h / .c .i
# _CXX : .h .hh .H .hp .hxx .hpp .HPP .h++ .tcc / .cpp .ii .cc .cp .cxx .CPP .c++ .C
TARGETOBJBIN1_C := $(wildcard ${TARGET1NAME:%=%.c})
TARGETOBJBIN1_C := ${TARGETOBJBIN1_C:%.c=%.o}
TARGETOBJBIN1_CXX := $(wildcard ${TARGET1NAME:%=%.cpp})
TARGETOBJBIN1_CXX := ${TARGETOBJBIN1_CXX:%.cpp=%.o}
TARGETOBJBIN2_C := $(wildcard ${TARGET2NAME:%=%.c})
TARGETOBJBIN2_C := ${TARGETOBJBIN2_C:%.c=%.o}
TARGETOBJBIN2_CXX := $(wildcard ${TARGET2NAME:%=%.cpp})
TARGETOBJBIN2_CXX := ${TARGETOBJBIN2_CXX:%.cpp=%.o}
TARGETOBJBIN3_C := $(wildcard ${TARGET3NAME:%=%.c})
TARGETOBJBIN3_C := ${TARGETOBJBIN3_C:%.c=%.o}
TARGETOBJBIN3_CXX := $(wildcard ${TARGET3NAME:%=%.cpp})
TARGETOBJBIN3_CXX := ${TARGETOBJBIN3_CXX:%.cpp=%.o}
TARGETOBJBIN_C := $(wildcard ${TARGETNAME:%=%.c})
TARGETOBJBIN_C := ${TARGETOBJBIN_C:%.c=%.o}
TARGETOBJBIN_CXX := $(wildcard ${TARGETNAME:%=%.cpp})
TARGETOBJBIN_CXX := ${TARGETOBJBIN_CXX:%.cpp=%.o}
TARGETOBJLIB1_C := $(wildcard ${TARGETLIB1NAME:%=%.c})
TARGETOBJDLL1_C := ${TARGETOBJLIB1_C:%.c=%.lo}
TARGETOBJLIB1_C := ${TARGETOBJLIB1_C:%.c=%.ao}
TARGETOBJLIB1_CXX := $(wildcard ${TARGETLIB1NAME:%=%.cpp})
TARGETOBJDLL1_CXX := ${TARGETOBJLIB1_CXX:%.cpp=%.lo}
TARGETOBJLIB1_CXX := ${TARGETOBJLIB1_CXX:%.cpp=%.ao}
TARGETOBJLIB2_C := $(wildcard ${TARGETLIB2NAME:%=%.c})
TARGETOBJDLL2_C := ${TARGETOBJLIB2_C:%.c=%.lo}
TARGETOBJLIB2_C := ${TARGETOBJLIB2_C:%.c=%.ao}
TARGETOBJLIB2_CXX := $(wildcard ${TARGETLIB2NAME:%=%.cpp})
TARGETOBJDLL2_CXX := ${TARGETOBJLIB2_CXX:%.cpp=%.lo}
TARGETOBJLIB2_CXX := ${TARGETOBJLIB2_CXX:%.cpp=%.ao}

# if (defined(TARGETTYPE)) && (TARGETTYPE == DYNLINK || TARGETTYPE == ALL)
ifneq (,$(findstring ${TARGETTYPE},DYNLINK ALL))
	TARGETS += DLL
#	$(foreach (m,n),(${MCUS},1 2),TARGETOBJ${m}DLL{n}_C) := $(foreach (m,n),(${MCUS},1 2),${TARGETOBJDLL{n}_C:%=${BUILD${m}DIR}%}) 
	TARGETOBJ32DLL1_C := ${TARGETOBJDLL1_C:%=${BUILD32DIR}%}
	TARGETOBJ64DLL1_C := ${TARGETOBJDLL1_C:%=${BUILD64DIR}%}
	TARGETOBJ32DLL2_C := ${TARGETOBJDLL2_C:%=${BUILD32DIR}%}
	TARGETOBJ64DLL2_C := ${TARGETOBJDLL2_C:%=${BUILD64DIR}%}
#	$(foreach (m,n),(${MCUS},1 2),TARGETOBJ${m}DLL{n}_CXX) := $(foreach (m,n),(${MCUS},1 2),${TARGETOBJDLL{n}_CXX:%=${BUILD${m}DIR}%})
	TARGETOBJ32DLL1_CXX := ${TARGETOBJDLL1_CXX:%=${BUILD32DIR}%}
	TARGETOBJ64DLL1_CXX := ${TARGETOBJDLL1_CXX:%=${BUILD64DIR}%}
	TARGETOBJ32DLL2_CXX := ${TARGETOBJDLL2_CXX:%=${BUILD32DIR}%}
	TARGETOBJ64DLL2_CXX := ${TARGETOBJDLL2_CXX:%=${BUILD64DIR}%}
endif
# if (defined(TARGETTYPE)) && (TARGETTYPE == LIBRARY || TARGETTYPE == ALL)
ifneq (,$(findstring ${TARGETTYPE},LIBRARY ALL))
	TARGETS += LIB
#	$(foreach (m,n),(${MCUS},1 2),TARGETOBJ${m}LIB{n}_C) := $(foreach (m,n),(${MCUS},1 2),${TARGETOBJLIB{n}_C:%=${BUILD${m}DIR}%})
	TARGETOBJ32LIB1_C := ${TARGETOBJLIB1_C:%=${BUILD32DIR}%}
	TARGETOBJ64LIB1_C := ${TARGETOBJLIB1_C:%=${BUILD64DIR}%}
	TARGETOBJ32LIB2_C := ${TARGETOBJLIB2_C:%=${BUILD32DIR}%}
	TARGETOBJ64LIB2_C := ${TARGETOBJLIB2_C:%=${BUILD64DIR}%}
#	$(foreach (m,n),(${MCUS},1 2),TARGETOBJ${m}LIB{n}_CXX) := $(foreach (m,n),(${MCUS},1 2),${TARGETOBJLIB{n}_CXX:%=${BUILD${m}DIR}%})
	TARGETOBJ32LIB1_CXX := ${TARGETOBJLIB1_CXX:%=${BUILD32DIR}%}
	TARGETOBJ64LIB1_CXX := ${TARGETOBJLIB1_CXX:%=${BUILD64DIR}%}
	TARGETOBJ32LIB2_CXX := ${TARGETOBJLIB2_CXX:%=${BUILD32DIR}%}
	TARGETOBJ64LIB2_CXX := ${TARGETOBJLIB2_CXX:%=${BUILD64DIR}%}
endif
# if (defined(TARGETTYPE)) && (TARGETTYPE == PROGRAM || TARGETTYPE == ALL)
ifneq (,$(findstring ${TARGETTYPE},PROGRAM ALL))
	TARGETS += BIN
#	$(foreach m,${MCUS},TARGETOBJ${m}BIN1_C) := $(foreach m,${MCUS},${TARGETOBJBIN1_C:%=${BUILD${m}DIR}%})
	TARGETOBJ32BIN1_C := ${TARGETOBJBIN1_C:%=${BUILD32DIR}%}
	TARGETOBJ64BIN1_C := ${TARGETOBJBIN1_C:%=${BUILD64DIR}%}
#	$(foreach m,${MCUS},TARGETOBJ${m}BIN1_CXX) := $(foreach m,${MCUS},${TARGETOBJBIN1_CXX:%=${BUILD${m}DIR}%})
	TARGETOBJ32BIN1_CXX := ${TARGETOBJBIN1_CXX:%=${BUILD32DIR}%}
	TARGETOBJ64BIN1_CXX := ${TARGETOBJBIN1_CXX:%=${BUILD64DIR}%}
#	$(foreach m,${MCUS},TARGETOBJ${m}BIN2_C) := $(foreach m,${MCUS},${TARGETOBJBIN2_C:%=${BUILD${m}DIR}%})
	TARGETOBJ32BIN2_C := ${TARGETOBJBIN2_C:%=${BUILD32DIR}%}
	TARGETOBJ64BIN2_C := ${TARGETOBJBIN2_C:%=${BUILD64DIR}%}
#	$(foreach m,${MCUS},TARGETOBJ${m}BIN2_CXX) := $(foreach m,${MCUS},${TARGETOBJBIN2_CXX:%=${BUILD${m}DIR}%})
	TARGETOBJ32BIN2_CXX := ${TARGETOBJBIN2_CXX:%=${BUILD32DIR}%}
	TARGETOBJ64BIN2_CXX := ${TARGETOBJBIN2_CXX:%=${BUILD64DIR}%}
#	$(foreach m,${MCUS},TARGETOBJ${m}BIN3_C) := $(foreach m,${MCUS},${TARGETOBJBIN3_C:%=${BUILD${m}DIR}%})
	TARGETOBJ32BIN3_C := ${TARGETOBJBIN3_C:%=${BUILD32DIR}%}
	TARGETOBJ64BIN3_C := ${TARGETOBJBIN3_C:%=${BUILD64DIR}%}
#	$(foreach m,${MCUS},TARGETOBJ${m}BIN3_CXX) := $(foreach m,${MCUS},${TARGETOBJBIN3_CXX:%=${BUILD${m}DIR}%})
	TARGETOBJ32BIN3_CXX := ${TARGETOBJBIN3_CXX:%=${BUILD32DIR}%}
	TARGETOBJ64BIN3_CXX := ${TARGETOBJBIN3_CXX:%=${BUILD64DIR}%}
#	$(foreach m,${MCUS},TARGETOBJ${m}BIN_C) := $(foreach m,${MCUS},${TARGETOBJBIN_C:%=${BUILD${m}DIR}%})
	TARGETOBJ32BIN_C := ${TARGETOBJBIN_C:%=${BUILD32DIR}%}
	TARGETOBJ64BIN_C := ${TARGETOBJBIN_C:%=${BUILD64DIR}%}
#	$(foreach m,${MCUS},TARGETOBJ${m}BIN_CXX) := $(foreach m,${MCUS},${TARGETOBJBIN_CXX:%=${BUILD${m}DIR}%})
	TARGETOBJ32BIN_CXX := ${TARGETOBJBIN_CXX:%=${BUILD32DIR}%}
	TARGETOBJ64BIN_CXX := ${TARGETOBJBIN_CXX:%=${BUILD64DIR}%}
endif

# Often-used with default options
RM := rm -f
RMDIR := rm -fr
MKDIR := mkdir -p
LN := ln -sf
INST := install -m
CHMODX := chmod +x
# GCC 4.8
#CCVER := 4.8
#CC_VER := 4.8 48
# GCC 4.9
#CCVER := 4.9
#CC_VER := 4.9 49
# GCC 5.X
#CCVER := 5
#CC_VER := 5 60

#${CC}-${CCVER}
CC := gcc
#${CXX}-${CCVER}
#CXX := g++
CXX := gcc
#AR := ar / gcc-ar-${CCVER}
AR := ar
ARFLAGS = rcvs
#ARFLAGS = -rcs

#WARNS := all extra shadow format=2 float-equal conversion logical-op cast-qual cast-align
#WARNS += shift-overflow=2 duplicated-cond
#DEFS := _GLIBCXX_DEBUG _GLIBCXX_DEBUG_PEDANTIC _FORTIFY_SOURCE=2
#OPTS := sanitize=address
#OPTS := sanitize=undefined no-sanitize-recover stack-protector
DLLDEFS := _DLL_
LIBDEFS := _LIB_
BIN1DEFS := _BIN_
BIN2DEFS := _BIN_
BIN3DEFS := _BIN_
BINDEFS := _BIN_

CFLAGS := -I.
ifdef
	CFLAGS += ${WARNS:%=-W%}
else
	CFLAGS += -w
endif
CFLAGS += ${DEFS:%=-D%} ${OPTS:%=-f%}
CFLAGS += -pedantic -std=c++11

LDFLAGS := ${LIBDIR:%=-L%} ${LIBS:%=-l%}
ifeq (${BUILD},debug)
	#CFLAGS += -O -g -DDBUG
	CFLAGS += -O0 -g -DDBUG
else
	#CFLAGS += -O2
	CFLAGS += -Os
endif
CLANG ?= c
CCFLAGS := ${CFLAGS} ${CLANG:%=-x%}
CDLLFLAGS := ${CCFLAGS} ${DLLDEFS:%=-D%} -fPIC -c
CDLLLDFLAGS := -shared ${LDFLAGS}
CLIBFLAGS := ${CCFLAGS} ${LIBDEFS:%=-D%} ${LDFLAGS} -c
CBINFLAGS := ${CCFLAGS} ${BINDEFS:%=-D%} -c
CBINLDFLAGS := ${LDFLAGS} -ldl

CXXLANG ?= c++
CXXCFLAGS := ${CFLAGS} ${CXXLANG:%=-x%}
CXXDLLFLAGS := ${CXXCFLAGS} ${DLLDEFS:%=-D%} -fPIC -c
CXXDLLLDFLAGS := -shared-libgcc -lstdc++ ${LDFLAGS}
CXXLIBFLAGS := ${CXXCFLAGS} ${LIBDEFS:%=-D%} -lstdc++ ${LDFLAGS} -c
CXXBINFLAGS := ${CXXCFLAGS} ${BINDEFS:%=-D%} -c
CXXBINLDFLAGS := -lstdc++ ${LDFLAGS} -ldl

SH_NC := \033[0m
SH_BCRED := \033[0;31m

.PHONY: all
all: build install-lib run
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@echo -n "${SH_NC}"

.PHONY: build
build: clean ${TARGETDIRS} ${BUILDDIRS} ${TARGETS}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@echo -n "${SH_NC}"

${TARGETDIRS} ${BUILDDIRS}:
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${MKDIR} $@
	@echo -n "${SH_NC}"

DLL: $(foreach m,${MCUS},${TARGETLIBSNAME:%=${TARGET${m}DIR}lib%.so})
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@echo -n "${SH_NC}"

LIB: $(foreach m,${MCUS},${TARGETLIBSNAME:%=${TARGET${m}DIR}lib%.a})
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@echo -n "${SH_NC}"

BIN: $(foreach m,${MCUS},${TARGETSNAME:%=${TARGET${m}DIR}%})
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@echo -n "${SH_NC}"

##### MULTI TARGET REDEFINE ## BEGIN #####

lib%.so: lib%.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER}
	sudo ${LN} $(notdir $@).${TARGETLIBMAINVER} $@
	sudo ${LN} $(notdir $@).${TARGETLIBMAINVER}.${TARGETLIBMINORVER} $@.${TARGETLIBMAINVER}

${TARGETLIB1NAME:%=${TARGET32DIR}lib%.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER}}: ${TARGETOBJ32DLL1_C} ${TARGETOBJ32DLL1_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@if [ -n "${TARGETOBJ32DLL1_CXX}" ]; then ${CXX} ${MX32} $^ -o $@ ${CXXDLLLDFLAGS} -Wl,-soname,$(notdir $@); \
	elif [ -n "${TARGETOBJ32DLL1_C}" ]; then ${CC} ${MX32} $^ -o $@ ${CDLLLDFLAGS} -Wl,-soname,$(notdir $@); fi
	@sudo ${CHMODX} $@
	@cd ${TARGET32DIR}
	@echo -n "${SH_NC}"

${TARGETLIB2NAME:%=${TARGET32DIR}lib%.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER}}: ${TARGETOBJ32DLL2_C} ${TARGETOBJ32DLL2_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@if [ -n "${TARGETOBJ32DLL2_CXX}" ]; then ${CXX} ${MX32} $^ -o $@ ${CXXDLLLDFLAGS} -Wl,-soname,$(notdir $@); \
	elif [ -n "${TARGETOBJ32DLL2_C}" ]; then ${CC} ${MX32} $^ -o $@ ${CDLLLDFLAGS} -Wl,-soname,$(notdir $@); fi
	@sudo ${CHMODX} $@
	@cd ${TARGET32DIR}
	@echo -n "${SH_NC}"

${TARGETLIB1NAME:%=${TARGET64DIR}lib%.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER}}: ${TARGETOBJ64DLL1_C} ${TARGETOBJ64DLL1_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@if [ -n "${TARGETOBJ64DLL1_CXX}" ]; then ${CXX} ${MX64} $^ -o $@ ${CXXDLLLDFLAGS} -Wl,-soname,$(notdir $@); \
	elif [ -n "${TARGETOBJ64DLL1_C}" ]; then ${CC} ${MX64} $^ -o $@ ${CDLLLDFLAGS} -Wl,-soname,$(notdir $@); fi
	@sudo ${CHMODX} $@
	@cd ${TARGET64DIR}
	@echo -n "${SH_NC}"

${TARGETLIB2NAME:%=${TARGET64DIR}lib%.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER}}: ${TARGETOBJ64DLL2_C} ${TARGETOBJ64DLL2_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@if [ -n "${TARGETOBJ64DLL2_CXX}" ]; then ${CXX} ${MX64} $^ -o $@ ${CXXDLLLDFLAGS} -Wl,-soname,$(notdir $@); \
	elif [ -n "${TARGETOBJ64DLL2_C}" ]; then ${CC} ${MX64} $^ -o $@ ${CDLLLDFLAGS} -Wl,-soname,$(notdir $@); fi
	@sudo ${CHMODX} $@
	@cd ${TARGET64DIR}
	@echo -n "${SH_NC}"

${TARGETLIB1NAME:%=${TARGET32DIR}lib%.a}: ${TARGETOBJ32LIB1_C} ${TARGETOBJ32LIB1_CXX}
${TARGETLIB2NAME:%=${TARGET32DIR}lib%.a}: ${TARGETOBJ32LIB2_C} ${TARGETOBJ32LIB2_CXX}
${TARGETLIB1NAME:%=${TARGET64DIR}lib%.a}: ${TARGETOBJ64LIB1_C} ${TARGETOBJ64LIB1_CXX}
${TARGETLIB2NAME:%=${TARGET64DIR}lib%.a}: ${TARGETOBJ64LIB2_C} ${TARGETOBJ64LIB2_CXX}

$(foreach m,${MCUS},${TARGETLIBSNAME:%=${TARGET${m}DIR}lib%.a}):
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${AR} ${ARFLAGS} $@ $^
	@echo -n "${SH_NC}"

##### MULTI TARGET REDEFINE ## END #####

${TARGET1NAME:%=${TARGET32DIR}%}: ${TARGETOBJ32BIN1_C} ${TARGETOBJ32BIN1_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
#	@${CC} ${MX32} ${CBINLDFLAGS} $^ -o $@
	@if [ -n "${TARGETOBJ32BIN1_CXX}" ]; then ${CXX} ${MX32} $^ -o $@ ${CXXBINLDFLAGS}; \
	elif [ -n "${TARGETOBJ32BIN1_C}" ]; then ${CC} ${MX32} $^ -o $@ ${CBINLDFLAGS}; fi
	@sudo ${CHMODX} $@
	@echo -n "${SH_NC}"

${TARGET1NAME:%=${TARGET64DIR}%}: ${TARGETOBJ64BIN1_C} ${TARGETOBJ64BIN1_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
#	@${CC} ${MX64} ${CBINLDFLAGS} $^ -o $@
	@if [ -n "${TARGETOBJ64BIN1_CXX}" ]; then ${CXX} ${MX64} $^ -o $@ ${CXXBINLDFLAGS}; \
	elif [ -n "${TARGETOBJ64BIN1_C}" ]; then ${CC} ${MX64} $^ -o $@ ${CBINLDFLAGS}; fi
	@sudo ${CHMODX} $@
	@echo -n "${SH_NC}"

${TARGET2NAME:%=${TARGET32DIR}%}: ${TARGETOBJ32BIN2_C} ${TARGETOBJ32BIN2_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
#	@${CC} ${MX32} ${CBINLDFLAGS} $^ -o $@
	@if [ -n "${TARGETOBJ32BIN2_CXX}" ]; then ${CXX} ${MX32} $^ -o $@ ${CXXBINLDFLAGS}; \
	elif [ -n "${TARGETOBJ32BIN2_C}" ]; then ${CC} ${MX32} $^ -o $@ ${CBINLDFLAGS}; fi
	@sudo ${CHMODX} $@
	@echo -n "${SH_NC}"

${TARGET2NAME:%=${TARGET64DIR}%}: ${TARGETOBJ64BIN2_C} ${TARGETOBJ64BIN2_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
#	@${CC} ${MX64} ${CBINLDFLAGS} $^ -o $@
	@if [ -n "${TARGETOBJ64BIN2_CXX}" ]; then ${CXX} ${MX64} $^ -o $@ ${CXXBINLDFLAGS}; \
	elif [ -n "${TARGETOBJ64BIN2_C}" ]; then ${CC} ${MX64} $^ -o $@ ${CBINLDFLAGS}; fi
	@sudo ${CHMODX} $@
	@echo -n "${SH_NC}"

${TARGET3NAME:%=${TARGET32DIR}%}: ${TARGETOBJ32BIN3_C} ${TARGETOBJ32BIN3_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
#	@${CC} ${MX32} ${CBINLDFLAGS} $^ -o $@
	@if [ -n "${TARGETOBJ32BIN3_CXX}" ]; then ${CXX} ${MX32} $^ -o $@ ${CXXBINLDFLAGS}; \
	elif [ -n "${TARGETOBJ32BIN3_C}" ]; then ${CC} ${MX32} $^ -o $@ ${CBINLDFLAGS}; fi
	@sudo ${CHMODX} $@
	@echo -n "${SH_NC}"

${TARGET3NAME:%=${TARGET64DIR}%}: ${TARGETOBJ64BIN3_C} ${TARGETOBJ64BIN3_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
#	@${CC} ${MX64} ${CBINLDFLAGS} $^ -o $@
	@if [ -n "${TARGETOBJ64BIN3_CXX}" ]; then ${CXX} ${MX64} $^ -o $@ ${CXXBINLDFLAGS}; \
	elif [ -n "${TARGETOBJ64BIN3_C}" ]; then ${CC} ${MX64} $^ -o $@ ${CBINLDFLAGS}; fi
	@sudo ${CHMODX} $@
	@echo -n "${SH_NC}"

${TARGETNAME:%=${TARGET32DIR}%}: ${TARGETOBJ32BIN_C} ${TARGETOBJ32BIN_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
#	@${CC} ${MX32} ${CBINLDFLAGS} $^ -o $@
	@if [ -n "${TARGETOBJ32BIN_CXX}" ]; then ${CXX} ${MX32} $^ -o $@ ${CXXBINLDFLAGS}; \
	elif [ -n "${TARGETOBJ32BIN_C}" ]; then ${CC} ${MX32} $^ -o $@ ${CBINLDFLAGS}; fi
	@sudo ${CHMODX} $@
	@echo -n "${SH_NC}"

${TARGETNAME:%=${TARGET64DIR}%}: ${TARGETOBJ64BIN_C} ${TARGETOBJ64BIN_CXX}
	@echo -n "${SH_BCRED}"
	@echo $@ $^
#	@${CC} ${MX64} ${CBINLDFLAGS} $^ -o $@
	@if [ -n "${TARGETOBJ64BIN_CXX}" ]; then ${CXX} ${MX64} $^ -o $@ ${CXXBINLDFLAGS}; \
	elif [ -n "${TARGETOBJ64BIN_C}" ]; then ${CC} ${MX64} $^ -o $@ ${CBINLDFLAGS}; fi
	@sudo ${CHMODX} $@
	@echo -n "${SH_NC}"

${BUILD32DIR}%.lo: %.c %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CC} ${MX32} ${CDLLFLAGS} $< -o $@
	@echo -n "${SH_NC}"

${BUILD64DIR}%.lo: %.c %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CC} ${MX64} ${CDLLFLAGS} $< -o $@
	@echo -n "${SH_NC}"

${BUILD32DIR}%.ao: %.c %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CC} ${MX32} ${CLIBFLAGS} $< -o $@
	@echo -n "${SH_NC}"

${BUILD64DIR}%.ao: %.c %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CC} ${MX64} ${CLIBFLAGS} $< -o $@
	@echo -n "${SH_NC}"

${BUILD32DIR}%.o: %.c %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CC} ${MX32} ${CBINFLAGS} $< -o $@
	@echo -n "${SH_NC}"

${BUILD64DIR}%.o: %.c %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CC} ${MX64} ${CBINFLAGS} $< -o $@
	@echo -n "${SH_NC}"

${BUILD32DIR}%.lo: %.cpp %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CXX} ${MX32} ${CXXDLLFLAGS} $< -o $@
	@echo -n "${SH_NC}"

${BUILD64DIR}%.lo: %.cpp %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CXX} ${MX64} ${CXXDLLFLAGS} $< -o $@
	@echo -n "${SH_NC}"

${BUILD32DIR}%.ao: %.cpp %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CXX} ${MX32} ${CXXLIBFLAGS} $< -o $@
	@echo -n "${SH_NC}"

${BUILD64DIR}%.ao: %.cpp %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CXX} ${MX64} ${CXXLIBFLAGS} $< -o $@
	@echo -n "${SH_NC}"

${BUILD32DIR}%.o: %.cpp %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CXX} ${MX32} ${CXXBINFLAGS} $< -o $@
	@echo -n "${SH_NC}"

${BUILD64DIR}%.o: %.cpp %.h
	@echo -n "${SH_BCRED}"
	@echo $@ $^
	@${CXX} ${MX64} ${CXXBINFLAGS} $< -o $@
	@echo -n "${SH_NC}"

.PHONY: clean
clean:
	@echo 'Clean up ..'
	@echo -n "${SH_BCRED}"
	@${RMDIR} ${BUILDDIR}
	@echo -n "${SH_NC}"
	@echo 'Cleanup complete!'

.PHONY: cleanall
cleanall:
	@echo 'Clean up ..'
	@echo -n "${SH_BCRED}"
	@${RMDIR} ${TARGETDIR} ${BUILDDIR}
	@echo -n "${SH_NC}"
	@echo 'Cleanup complete!'

.PHONY: install-dev
install-dev:
	@echo 'Installed List Upgrade ..'
	@echo -n "${SH_BCRED}"
	@-sudo add-apt-repository ppa:ubuntu-toolchain-r/test
	@-sudo apt-get update 2> /dev/null && sudo apt-get upgrade
	@-sudo apt-get install libc6-dev-i386 build-essential g++-multilib
	@echo -n "${SH_NC}"
	@echo "Installation Developement complete! ($(shell ${CC} --version | grep ^${CC}))"
#	@sudo apt-get install gcc-8 g++-8 gcc-8-multilib
#	@if ! which gcc-8 | grep gcc -c > /dev/null ; then sudo apt-get install gcc-7 g++-7 gcc-7-multilib ; fi
#	@if ! which gcc-7 | grep gcc -c > /dev/null ; then sudo apt-get install gcc-6 g++-6 gcc-6-multilib ; fi
#	@if ! which gcc-6 | grep gcc -c > /dev/null ; then sudo apt-get install gcc-5 g++-5 gcc-5-multilib ; fi
#	@if which gcc-8 | grep gcc -c > /dev/null ; then sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 90 \
#	--slave /usr/bin/g++ g++ /usr/bin/g++-8 \
#	--slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-8 \
#	--slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-8 \
#	--slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-8 \
#	; elif which gcc-7 | grep gcc -c > /dev/null ; then sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 80 \
#	--slave /usr/bin/g++ g++ /usr/bin/g++-7 \
#	--slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-7 \
#	--slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-7 \
#	--slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-7 \
#	; elif which gcc-6 | grep gcc -c > /dev/null ; then sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-6 70 \
#	--slave /usr/bin/g++ g++ /usr/bin/g++-6 \
#	--slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-6 \
#	--slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-6 \
#	--slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-6 \
#	; elif which gcc-5 | grep gcc -c > /dev/null ; then sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 \
#	--slave /usr/bin/g++ g++ /usr/bin/g++-5 \
#	--slave /usr/bin/gcc-ar gcc-ar /usr/bin/gcc-ar-5 \
#	--slave /usr/bin/gcc-nm gcc-nm /usr/bin/gcc-nm-5 \
#	--slave /usr/bin/gcc-ranlib gcc-ranlib /usr/bin/gcc-ranlib-5 \
#	; fi
#	@sudo updatedb && sudo ldconfig
#	@echo -n "${SH_NC}"
#	@echo "Installation Developement complete! ($(shell ${CC} --version | grep ^${CC}))"

.PHONY: install-lib
install-lib:
	@echo 'Installed Libraries ..'
	@echo -n "${SH_BCRED}"
	@sudo ${RM} ${TARGETLIB1NAME:%=${INSTLIBDIR1}lib%.so*} ${TARGETLIB2NAME:%=${INSTLIBDIR1}lib%.so*} ${TARGETLIB1NAME:%=${INSTLIBDIR2}lib%.so*} ${TARGETLIB2NAME:%=${INSTLIBDIR2}lib%.so*} ${TARGETLIB1NAME:%=${INSTLIBDIR3}lib%.so*} ${TARGETLIB2NAME:%=${INSTLIBDIR3}lib%.so*} 2>/dev/null
	@sudo ${INST} 644 ${TARGET${ARCH}DIR}lib*.a ${INSTLIBDIR1}
	@sudo ${INST} 644 ${TARGET${ARCH}DIR}lib*.a ${INSTLIBDIR2}
	@sudo ${INST} 644 ${TARGET${ARCH}DIR}lib*.a ${INSTLIBDIR3}
	@sudo ${INST} 755 ${TARGET${ARCH}DIR}lib*.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER} ${INSTLIBDIR1}
	@sudo ${INST} 755 ${TARGET${ARCH}DIR}lib*.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER} ${INSTLIBDIR2}
	@sudo ${INST} 755 ${TARGET${ARCH}DIR}lib*.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER} ${INSTLIBDIR3}
	@sudo ${LN} ${TARGETLIB1NAME:%=${INSTLIBDIR1}lib%.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER}} ${TARGETLIB1NAME:%=${INSTLIBDIR1}lib%.so.${TARGETLIBMAINVER}}
	@sudo ${LN} ${TARGETLIB1NAME:%=${INSTLIBDIR2}lib%.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER}} ${TARGETLIB1NAME:%=${INSTLIBDIR2}lib%.so.${TARGETLIBMAINVER}}
	@sudo ${LN} ${TARGETLIB1NAME:%=${INSTLIBDIR3}lib%.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER}} ${TARGETLIB1NAME:%=${INSTLIBDIR3}lib%.so.${TARGETLIBMAINVER}}
	@sudo ${LN} ${TARGETLIB1NAME:%=${INSTLIBDIR1}lib%.so.${TARGETLIBMAINVER}} ${TARGETLIB1NAME:%=${INSTLIBDIR1}lib%.so}
	@sudo ${LN} ${TARGETLIB1NAME:%=${INSTLIBDIR2}lib%.so.${TARGETLIBMAINVER}} ${TARGETLIB1NAME:%=${INSTLIBDIR2}lib%.so}
	@sudo ${LN} ${TARGETLIB1NAME:%=${INSTLIBDIR3}lib%.so.${TARGETLIBMAINVER}} ${TARGETLIB1NAME:%=${INSTLIBDIR3}lib%.so}
	@sudo ${LN} ${TARGETLIB2NAME:%=${INSTLIBDIR1}lib%.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER}} ${TARGETLIB2NAME:%=${INSTLIBDIR1}lib%.so.${TARGETLIBMAINVER}}
	@sudo ${LN} ${TARGETLIB2NAME:%=${INSTLIBDIR2}lib%.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER}} ${TARGETLIB2NAME:%=${INSTLIBDIR2}lib%.so.${TARGETLIBMAINVER}}
	@sudo ${LN} ${TARGETLIB2NAME:%=${INSTLIBDIR3}lib%.so.${TARGETLIBMAINVER}.${TARGETLIBMINORVER}} ${TARGETLIB2NAME:%=${INSTLIBDIR3}lib%.so.${TARGETLIBMAINVER}}
	@sudo ${LN} ${TARGETLIB2NAME:%=${INSTLIBDIR1}lib%.so.${TARGETLIBMAINVER}} ${TARGETLIB2NAME:%=${INSTLIBDIR1}lib%.so}
	@sudo ${LN} ${TARGETLIB2NAME:%=${INSTLIBDIR2}lib%.so.${TARGETLIBMAINVER}} ${TARGETLIB2NAME:%=${INSTLIBDIR2}lib%.so}
	@sudo ${LN} ${TARGETLIB2NAME:%=${INSTLIBDIR3}lib%.so.${TARGETLIBMAINVER}} ${TARGETLIB2NAME:%=${INSTLIBDIR3}lib%.so}
	@echo -n "${SH_NC}"
	@echo 'Installation Libraries complete!'

.PHONY: run
run:
	@cd ${TARGET${ARCH}DIR} ; \
	if [ -n "${TARGET1NAME}" ]; then sudo ./${TARGET1NAME}; fi ; \
	if [ -n "${TARGET2NAME}" ]; then sudo ./${TARGET2NAME}; fi ; \
	if [ -n "${TARGET3NAME}" ]; then sudo ./${TARGET3NAME}; fi ; \
	if [ -n "${TARGETNAME}" ]; then sudo ./${TARGETNAME}; fi

.PHONY: version
version:
	@echo 'Ekit Version: ${TARGETLIBMAINVER}.${TARGETLIBMINORVER}'

.PHONY: help
help:
	@echo 'Make targets are:'
	@echo '  build: build libraries and userspace programs'
	@echo '  install-dev: install develope kit'
	@echo '  install-lib: install ${TARGETLIBSNAME}'
	@echo '  clean: cleanup'
