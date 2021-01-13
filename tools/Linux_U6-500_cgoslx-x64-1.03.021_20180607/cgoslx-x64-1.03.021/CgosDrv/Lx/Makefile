
# Kernel-Makefile for building cgos for kernel version 2.6.x and 3.x
#

# by default, the kernel source is assumed to be in
# /lib/modules/`uname -r`/build

# ATTENTION: use the install option with care: it`s assumed that the
# driver was build for the currently running kernel.

KERNELDIR ?= /lib/modules/`uname -r`/build
CONFIG_CGOS ?= m

TARGET = cgosdrv.ko
C_LXSRC = DrvLx.c DrvOsaLx.c
C_LXOBJ = DrvLx.o DrvOsaLx.o
C_SRC = ../CgosDrv.c ../Cgeb.c
C_OBJ = ../Cgeb.o ../CgosDrv.o
C_EMU = ../CgebEmu.c

DRV_EMUOBJ = cgosdrv.o

make26_pre16 = $(MAKE) -C $(KERNELDIR) M=$(PWD) modules
make26 = $(MAKE) -C $(KERNELDIR) M=$(PWD)

sublevel = $(shell sed -n s/SUBLEVEL\ =\ *//p $(KERNELDIR)/Makefile)
patchlevel = $(shell sed -n s/PATCHLEVEL\ =\ *//p $(KERNELDIR)/Makefile)

mkcmd = $(shell if [ "$(patchlevel)" == 6 && "$(sublevel)" -lt 16 ]; then echo $(make26_pre16); \
	else echo $(make26); fi; )

ifdef KERNELRELEASE

	EXTRA_CFLAGS += -I$(obj)/. -I$(obj)/.. -I$(obj)/../../CgosLib
	EXTRA_CFLAGS += -I$(obj)/../../CgosLib/Lx
#	EXTRA_CFLAGS += -O2 -mcmodel=kernel -DAMD64 -fno-strict-aliasing
#	EXTRA_CFLAGS += -O2 -DAMD64 -fno-strict-aliasing
	EXTRA_CFLAGS += -O2 -fno-strict-aliasing
	EXTRA_CFLAGS += -I$(obj)/../..
	obj-$(CONFIG_CGOS) += $(DRV_EMUOBJ)

	cgosdrv-objs := $(C_LXOBJ) $(C_OBJ)
	clean-files := *.o
else
	PWD := $(shell pwd)
	obj := $(PWD)

	DEF = -DUNIX -D__KERNEL__ -Dlinux -DMODULE
	EXTRA_CFLAGS = $(DEF) -O2 -Wall -Wl,-r -nostdlib
	EXTRA_CFLAGS += -I. -I.. -I../.. -I$(KERNELDIR)/include -I../../CgosLib
#	EXTRA_CFLAGS += -mcmodel=kernel -DAMD64 -fno-strict-aliasing
#	EXTRA_CFLAGS += -DAMD64 -fno-strict-aliasing
	EXTRA_CFLAGS += -fno-strict-aliasing

default:
	$(call mkcmd)

emu: $(C_LXSRC) $(C_SRC) $(C_EMU)
	gcc -o $(DRV_EMUOBJ) -D CGEBEMU $(EXTRA_CFLAGS) $(C_LXSRC) $(C_SRC) $(C_EMU)

endif

clean:
	rm -rf *~ *.ko *.mod.* .*.cmd .tmp* Drv*.o $(DRV_EMUOBJ)
	rm -rf $(C_LXOBJ) built-in.o
	rm -rf .*.flags
	rm -rf Module.symvers modules.order
	rm -rf ../*.o

install:
	install -m 644 -o root -g root $(TARGET) /lib/modules/`uname -r`/kernel/drivers/misc


