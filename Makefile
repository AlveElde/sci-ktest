obj-m += sci_ktest_mod.o

SRC := ./src
sci_ktest_mod-objs := $(SRC)/sci_ktest.o $(SRC)/sci_msq.o

DIS_SRC=/opt/DIS
SCI_SRC=/home/alve/scilib
KERNEL_SRC=/home/alve/kernel
OLD_DIS_SRC=/home/alve/royn-lts/DIS/src

SCI_SYMBOLS := $(SCI_SRC)/GENIF/LINUX/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(SCI_SYMBOLS)

CPPFLAGS += -D_DIS_KERNEL_ -DOS_IS_LINUX -DCPU_ADDR_IS_64_BIT 	\
            -DLINUX -DUNIX -DCPU_ARCH_IS_X86_64 -DADAPTER_IS_PX \
			-DINTERFACE_IS_GENIF -DDEBUG -DHAVE_WAIT_QUEUE_ENTRY

EXTRA_CFLAGS += .                                               	\
                -I$(DIS_SRC)/include								\
                -I$(DIS_SRC)/include/dis							\
                -I$(DIS_SRC)/include/os								\
                -I$(SCI_SRC)/.										\
                -I$(SCI_SRC)/GENIF									\
                -I$(SCI_SRC)/GENIF/LINUX							\
                -I$(SCI_SRC)/GENIF/LINUX/os							\
                -I$(SCI_SRC)/SISCI									\
				-I$(KERNEL_SRC)/.               					\
            	-I$(KERNEL_SRC)/include       						\
                -I$(KERNEL_SRC)/include/LINUX 						\
                -I$(OLD_DIS_SRC)/IRM_GX/drv/src                   	\
                -I$(OLD_DIS_SRC)/IRM_GX/drv/include               	\
                -I$(OLD_DIS_SRC)/IRM_GX/drv/src/LINUX             	\
                -I$(OLD_DIS_SRC)/IRM_GX/drv/src/LINUX/os			\
                -I$(OLD_DIS_SRC)/include                   			\
                -I$(OLD_DIS_SRC)/include/dis              			\

EXTRA_CFLAGS += ${CPPFLAGS}

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

install:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

ins-req: all
	sudo dmesg -C
	sudo insmod $(SCI_SRC)/GENIF/LINUX/dis_msq.ko
	sudo insmod sci_ktest_mod.ko local_adapter_no=0 remote_node_id=4 is_responder=N

ins-res: all
	sudo dmesg -C
	sudo insmod $(SCI_SRC)/GENIF/LINUX/dis_msq.ko
	sudo insmod sci_ktest_mod.ko local_adapter_no=0 remote_node_id=8 is_responder=Y

rm: 
	sudo rmmod sci_ktest_mod.ko
	sudo rmmod dis_msq.ko
	dmesg -t

req: ins-req rm

res: ins-res rm
