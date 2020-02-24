obj-m += sci_ktest.o

SRC := ./src
sci_ktest-objs := $(SRC)/sci_ktest.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

install:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

ins: 
	sudo dmesg -C
	sudo insmod sci_ktest.ko
	dmesg

rm: 
	sudo dmesg -C
	sudo rmmod sci_ktest.ko
	dmesg

test: ins rm