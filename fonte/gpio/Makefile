KDIR := /home/MonRa/Downloads/arm/linux-2.6.32.2/
PWD  := $(shell pwd)

obj-m += gpio.o

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	@rm -Rf *.o *.ko *.mod.c modules.order Module.symvers
