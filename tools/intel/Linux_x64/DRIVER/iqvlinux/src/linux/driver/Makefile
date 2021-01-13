###########################################################################
# Intel QV Linux kernel driver
# Copyright (c) 1999 - 2015, Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU General Public License,
# version 2, as published by the Free Software Foundation.
#
# This program is distributed in the hope it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
#
###########################################################################



CFILES = nalioctldrv.c linuxnaldriver.c linuxdriveros_i.c linuxdriverpci_i.c linuxdriverdevice_i.c



ifeq (,$(NALDIR))
    $(error NALDIR variable not set)
endif


KERNEL = $(shell [ -e /etc/chroot-kernel ] && cat /etc/chroot-kernel || uname -r)
BVER = $(shell uname -r | sed 's/\([0-9\.-]\)*//')

KSP := /usr/src/linux-$(shell echo $(KERNEL) | sed 's/$(BVER)/obj/')/$(shell uname -i)/$(BVER) \
       /usr/src/linux-$(KERNEL) \
       /usr/src/linux-$(shell echo $(KERNEL) | sed 's/-.*//') \
       /usr/src/linux-$(shell echo $(KERNEL) | sed 's/\([0-9]*\.[0-9]*\)\..*/\1/') \
       /usr/src/kernel-headers-$(KERNEL) \
       /usr/src/kernel-source-$(KERNEL) \
       /lib/modules/$(KERNEL)/build \
       /lib/modules/$(KERNEL)/source \
       /usr/src/linux

test_dir = $(shell [ -e $(dir)/include/linux -o -e $(dir)/include/generated/uapi ] && echo $(dir))
KSP := $(foreach dir, $(KSP), $(test_dir))

ifeq (,$(KSRC))
ifeq (,$(VERSION_FILE))
  VSPATH := /include/linux/version.h \
			/include/generated/utsrelease.h \
			/include/linux/utsrelease.h \
			/boot/vmlinuz.version.h \
			/include/generated/uapi/linux/version.h

  test_version_h = $(shell [ -f $(ksp)$(file) ] && echo $(ksp)$(file))
  KVER := $(foreach ksp,$(KSP),$(foreach file, $(VSPATH), $(test_version_h)))
  VER_FILE := $(firstword $(KVER))

  strip_ksrc=$(findstring $(pp),$(VER_FILE))
  KSRC := $(foreach pp,$(KSP),$(strip_ksrc))
  KSRC := $(firstword $(KSRC))
endif
endif

ifeq (,$(KSRC))
  $(error Linux kernel source not found)
else
ifeq (/lib/modules/$(KERNEL)/source, $(KSRC))
  KOBJ :=  /lib/modules/$(KERNEL)/build
else
  KOBJ :=  $(KSRC)
endif
endif

VSP :=  $(KOBJ)/include/generated/utsrelease.h \
        $(KOBJ)/include/linux/utsrelease.h \
        $(KOBJ)/include/linux/version.h \
        /boot/vmlinuz.version.h

CSP :=  $(KSRC)/include/generated/autoconf.h \
        $(KSRC)/include/linux/autoconf.h \
        /boot/vmlinuz.autoconf.h

test_file = $(shell [ -f $(file) ] && echo $(file))
VSP := $(foreach file, $(VSP), $(test_file))
CSP := $(foreach file, $(CSP), $(test_file))

ifeq (,$(VERSION_FILE))
  VERSION_FILE := $(firstword $(VSP))
endif
ifeq (,$(CONFIG_FILE))
  CONFIG_FILE := $(firstword $(CSP))
endif

ifeq (,$(wildcard $(VERSION_FILE)))
  $(error Linux kernel source not configured - missing version.h)
endif

ifeq (,$(wildcard $(CONFIG_FILE)))
  $(error Linux kernel source not configured - missing autoconf.h)
endif

ifneq (,$(findstring egcs-2.91.66, $(shell cat /proc/version)))
  CC := kgcc gcc cc
else
  CC := gcc cc
endif
test_cc = $(shell $(cc) --version > /dev/null 2>&1 && echo $(cc))
CC := $(foreach cc, $(CC), $(test_cc))
CC := $(firstword $(CC))
ifeq (,$(CC))
  $(error Compiler not found)
endif

ARCH := $(shell uname -m | sed 's/i.86/i386/')
ifeq ($(ARCH),alpha)
  EXTRA_CFLAGS += -ffixed-8 -mno-fp-regs
endif
ifeq ($(ARCH),x86_64)
  EXTRA_CFLAGS += -mcmodel=kernel -mno-red-zone
endif
ifeq ($(ARCH),ppc)
  EXTRA_CFLAGS += -m32 -mpowerpc -mmultiple -mno-bit-align -msoft-float -DLINUXPPC -DNAL_BIG_ENDIAN
endif
ifeq ($(ARCH),ppc64)
  EXTRA_CFLAGS += -m64 -mmultiple -mno-bit-align -DLINUXPPC64 -DNAL_BIG_ENDIAN
  LDFLAGS += -m64  -melf64ppc
endif

EXTRA_CFLAGS += -DNAL_LINUX -DNAL_DRIVER -DLINUX -D__KERNEL__ -DMODULE -O2 -pipe -Wall
EXTRA_CFLAGS += -I$(KSRC)/include  -I$(NALDIR)/inc -I$(NALDIR)/inc/linux
EXTRA_CFLAGS += $(shell [ -f $(KSRC)/include/linux/modversions.h ] && \
            echo "-DMODVERSIONS -DEXPORT_SYMTAB \
                  -include $(KSRC)/include/linux/modversions.h")
EXTRA_CFLAGS += $(CFLAGS_EXTRA)
ifeq ($(SVOS),TRUE)
    EXTRA_CFLAGS += -DNAL_SVOS
endif


RHC := $(KSRC)/include/linux/rhconfig.h
ifneq (,$(wildcard $(RHC)))
  # 7.3 typo in rhconfig.h
  ifneq (,$(shell $(CC) $(CFLAGS) -E -dM $(RHC) | grep __module__bigmem))
	EXTRA_CFLAGS += -D__module_bigmem
  endif
endif

KVER := $(shell $(CC) $(CFLAGS) -E -dM $(VERSION_FILE) | grep UTS_RELEASE | \
        awk '{ print $$3 }' | sed 's/\"//g')

KKVER := $(shell echo $(KVER) | \
         awk '{ if ($$0 ~ /2\.[4-9]\./) print "1"; \
                else if ($$0 ~ /3\.[0-9]+\./) print "1"; \
                else if ($$0 ~ /4\.[0-9]+\./) print "1"; \
                else print "0"}')
ifeq ($(KKVER), 0)
  $(error *** Aborting the build. \
          *** This driver is not supported on kernel versions older than 2.4.0)
endif

ifneq (,$(wildcard /lib/modules/$(KVER)/kernel))
  INSTDIR := /lib/modules/$(KVER)/kernel/drivers/char
else
  INSTDIR := /lib/modules/$(KVER)/char
endif


SMP := $(shell $(CC) $(CFLAGS) -E -dM $(CONFIG_FILE) | \
         grep '\<CONFIG_SMP\>' | awk '{ print $$3 }')
ifneq ($(SMP),1)
  SMP := 0
endif

ifneq ($(SMP),$(shell uname -a | grep SMP > /dev/null 2>&1 && echo 1 || echo 0))
  $(warning ***)
  ifeq ($(SMP),1)
    $(warning *** Warning: kernel source configuration (SMP))
    $(warning *** does not match running kernel (UP))
  else
    $(warning *** Warning: kernel source configuration (UP))
    $(warning *** does not match running kernel (SMP))
  endif
  $(warning *** Continuing with build,)
  $(warning *** resulting driver may not be what you want)
  $(warning ***)
endif

ifeq ($(SMP),1)
  EXTRA_CFLAGS += -D__SMP__
endif


K_VERSION:=$(shell echo $(KERNEL) | cut -c1-3 | sed 's/2\.[56]/2\.6/')

ifeq ($(K_VERSION), 2.4)

TARGET = iqvlinux.o

MANSECTION = 7
MANFILE = $(TARGET:.o=.$(MANSECTION))

ARCH := $(shell uname -m | sed 's/i.86/i386/')
ifeq ($(ARCH),alpha)
  EXTRA_CFLAGS += -ffixed-8 -mno-fp-regs
endif
ifeq ($(ARCH),x86_64)
  EXTRA_CFLAGS += -mcmodel=kernel
endif
ifeq ($(ARCH),ppc)
  EXTRA_CFLAGS += -m32 -mpowerpc -mmultiple -mno-bit-align -msoft-float -DLINUXPPC -DNAL_BIG_ENDIAN
  LDFLAGS += -melf32ppc
endif
ifeq ($(ARCH),ppc64)
  EXTRA_CFLAGS += -m64 -mmultiple -mno-bit-align -DLINUXPPC64 -DNAL_BIG_ENDIAN
  LDFLAGS += -m64 -melf64ppc
endif


ifneq (,$(wildcard /etc/SuSE-release))
  EXTRA_CFLAGS += -Wno-sign-compare -fno-strict-aliasing
endif

ifneq (,$(wildcard /etc/fedora-release))
  EXTRA_CFLAGS += -fno-strict-aliasing
endif

CFLAGS += $(EXTRA_CFLAGS)

.SILENT: $(TARGET)
$(TARGET): $(filter-out $(TARGET), $(CFILES:.c=.o))
	$(LD) -r $^ -o $@
	echo; echo
	echo "**************************************************"
	echo "** $(TARGET) built for $(KVER)"
	echo -n "** SMP               "
	if [ "$(SMP)" = "1" ]; \
		then echo "Enabled"; else echo "Disabled"; fi
	echo "**************************************************"
	echo

$(CFILES:.c=.o): $(HFILES)
default:
	make -f linuxdriver.mak

else # ifeq ($(K_VERSION),2.4)
TARGET = iqvlinux.ko

MANSECTION = 7
MANFILE = $(TARGET:.ko=.$(MANSECTION))

ifneq ($(KERNELRELEASE),)
EXTRA_CFLAGS += $(CFLAGS_EXTRA)
obj-m += iqvlinux.o
iqvlinux-objs := $(CFILES:.c=.o)
else
default:
ifeq ($(KOBJ),$(KSRC))
	$(MAKE) -C $(KSRC) SUBDIRS=$(shell pwd) modules
else
	$(MAKE) -C $(KSRC) O=$(KOBJ) SUBDIRS=$(shell pwd) modules
endif
endif

endif # ifeq ($(K_VERSION),2.4)

DEPVER := $(shell /sbin/depmod -V 2>/dev/null | \
          awk 'BEGIN {FS="."} NR==1 {print $$2}')



install: default
	# remove all old versions of the driver
	find $(INSTALL_MOD_PATH)/lib/modules/$(KVER) -name $(TARGET) -exec rm -f {} \; || true
	install -D -m 744 $(TARGET) $(INSTALL_MOD_PATH)$(INSTDIR)/$(TARGET)
ifeq (,$(INSTALL_MOD_PATH))
	/sbin/depmod -a || true
else
  ifeq ($(DEPVER),1 )
	/sbin/depmod -r $(INSTALL_MOD_PATH) -a || true
  else
	/sbin/depmod -b $(INSTALL_MOD_PATH) -a -n > /dev/null || true
  endif
endif

uninstall:
	if [ -e $(INSTDIR)/$(TARGET) ] ; then \
	    rm -f $(INSTDIR)/$(TARGET) ; \
	fi
	/sbin/depmod -a

.PHONY: clean install

installrpm:
	mkdir -p $(DESTDIR)/inc/linux/
	mkdir -p $(DESTDIR)/src/linux/driver/
	cp -r $(NALDIR)/inc/* $(DESTDIR)/inc
	cp $(NALDIR)/src/linux/driver/* $(DESTDIR)/src/linux/driver/
	cp $(NALDIR)/COPYING $(DESTDIR)/COPYING

clean:
	rm -rf $(TARGET) $(TARGET:.ko=.o) $(TARGET:.ko=.mod.o) $(CFILES:.c=.o) $(MANFILE).gz .*cmd
