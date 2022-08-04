# Makefile for driver1.c

ifneq ($(KERNELRELEASE),)
# If makefile has been invoked from the kernel build system
# ie. the path to kernel source has already been defined,
# then we use the following rule to build the module

obj-m := driver1.o

# obj-m stands for kernel module object. It creates a .ko file.
# driver1.o is the prerequisite file, which is created from driver1.c

else
# If makefile has been invoked from shell, using the command
# 'make' without any parameters, then KERNELRELEASE will be empty.
# Then we use the following rule to invoke kernel build system.

KDIR ?= /lib/modules/$(shell uname -r)/build
# Specifies the kernel build directory

default:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules
# Invoking the makefile in the kernel build system, which will
# invoke the makefile in the pwd, to build the module.

clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean
# removes all generated files in the module directory.

endif
