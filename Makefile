obj-m += registers.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

CFLAGS_registers.o :=
	

default:
	$(MAKE) -C $(KDIR) M=$(PWD) V=1 modules
