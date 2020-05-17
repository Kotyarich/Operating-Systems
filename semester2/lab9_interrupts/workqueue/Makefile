ifneq ($(KERNELRELEASE),)
	obj-m := my_work.o

else
	CURRENT = $(shell uname -r)
	KDIR = /lib/modules/$(CURRENT)/build
	PWD = $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	sudo make clean

clean:
	rm -rf .tmp_versions
	rm .my_work.*
	rm *.o
	rm *.mod.c
	rm *.symvers
	rm *.order

endif
