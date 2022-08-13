# Makefile for query_app.c (application) and query_ioctl.c (module)

ifneq ($(KERNELRELEASE),)
# If makefile has been invoked from the kernel build system
# ie. the path to kernel source has already been defined,
# then we use the following rule to build the module

obj-m := query_ioctl.o
# obj-m stands for kernel module object. It creates a .ko file.

else
# If makefile has been invoked from shell, using the command
# 'make' without any parameters, then KERNELRELEASE will be empty.
# Then we use the following rule to invoke kernel build system.

KDIR ?= /lib/modules/$(shell uname -r)/build
# Specifies the kernel build directory

default: module query_app
# This will compile the application, and also build the module as follows.

module:
	$(MAKE) -C $(KDIR) M=$(shell pwd) modules
# Invoking the makefile in the kernel build system, which will
# invoke the makefile in the pwd, to build the module.

clean:
	$(MAKE) -C $(KDIR) M=$(shell pwd) clean
	$(RM) query_app
# removes all generated files in the module directory.

endif
