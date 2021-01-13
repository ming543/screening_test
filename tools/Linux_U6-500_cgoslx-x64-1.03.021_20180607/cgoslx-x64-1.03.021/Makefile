ALLDIRS = CgosDrv CgosLib CgosDump CgosMon CgosBCgpio CgosI2CgpioLoopback CgosI2CgpioSample

# Determination of the target system
target = $(shell uname)
sysdir = Lx

ifdef target
 ifeq ("$(target)","QNX")
  sysdir = Qx
 endif
endif

M = for i in $(ALLDIRS); do make -C $$i/$(sysdir)
N = || exit 1; done

ifeq ("$(target)","QNX")
install:
	cp CgosI2CgpioSample/$(sysdir)/cgosi2cgpiosample /usr/bin
	cp CgosI2CgpioLoopback/$(sysdir)/cgosi2cgpioloopback /usr/bin
	cp CgosBCgpio/$(sysdir)/cgosbcgpio /usr/bin
	cp CgosMon/$(sysdir)/cgosmon /usr/bin
	cp CgosDump/$(sysdir)/cgosdump /usr/bin
	cp CgosLib/$(sysdir)/libcgos.so /usr/lib64
	cp CgosDrv/$(sysdir)/cgosdrv /usr/lib64
else

default:
	$(M) $(N)

install:
	install -m 755 -o root -g root CgosI2CgpioSample/$(sysdir)/cgosi2cgpiosample /usr/bin
	install -m 755 -o root -g root CgosI2CgpioLoopback/$(sysdir)/cgosi2cgpioloopback /usr/bin
	install -m 755 -o root -g root CgosBCgpio/$(sysdir)/cgosbcgpio /usr/bin
	install -m 755 -o root -g root CgosMon/$(sysdir)/cgosmon /usr/bin
	install -m 755 -o root -g root CgosDump/$(sysdir)/cgosdump /usr/bin
	install -m 755 -o root -g root CgosLib/$(sysdir)/libcgos.so /usr/lib64
	install -m 644 -o root -g root CgosDrv/$(sysdir)/cgosdrv.ko /lib/modules/`uname -r`/kernel/drivers/misc
	depmod -a

clean:
	$(M) $@ $(N)

endif



