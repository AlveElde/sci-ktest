obj-m += sci_ktest.o

SRC := ./src
sci_ktest-objs := $(SRC)/sci_ktest.o

DIS_SRC=/home/alve/SyncThingFolders/SyncThingCaseSensitive/royn/DIS/src
MOD_DIR=$(DIS_SRC)/.tmp_versions
DIS_SYMBOLS=$(DIS_SRC)/IRM_GX/drv/src/LINUX/Module.symvers
KOSIF_SYMBOLS=$(DIS_SRC)/COMMON/osif/kernel/src/LINUX/Module.symvers
SCI_SYMBOLS=$(DIS_SRC)/SCI_SOCKET/scilib/GENIF/LINUX/Module.symvers


CPPFLAGS += -D_DIS_KERNEL_ -DOS_IS_LINUX -DCPU_ADDR_IS_64_BIT \
            -DLINUX -DUNIX -DCPU_ARCH_IS_X86_64 -DADAPTER_IS_PX -DINTERFACE_IS_GENIF

EXTRA_CFLAGS += .                                               \
                -I$(DIS_SRC)/include                   			\
                -I$(DIS_SRC)/include/dis              			\
                -I$(DIS_SRC)/COMMON/osif/kernel               	\
                -I$(DIS_SRC)/COMMON/osif/kernel/include       	\
                -I$(DIS_SRC)/COMMON/osif/kernel/include/LINUX 	\
                -I$(DIS_SRC)/IRM_GX/drv/src                   	\
                -I$(DIS_SRC)/IRM_GX/drv/include               	\
                -I$(DIS_SRC)/IRM_GX/drv/src/LINUX             	\
                -I$(DIS_SRC)/IRM_GX/drv/src/LINUX/os			\
                -I$(DIS_SRC)/SCI_SOCKET/scilib					\
                -I$(DIS_SRC)/SCI_SOCKET/scilib/GENIF			\
                -I$(DIS_SRC)/SCI_SOCKET/scilib/GENIF/lib		\
                -I$(DIS_SRC)/SCI_SOCKET/scilib/GENIF/lib/LINUX	\
                -I$(DIS_SRC)/SCI_SOCKET/scilib/GENIF/LINUX		\
                -I$(DIS_SRC)/SCI_SOCKET/scilib/GENIF/LINUX/os	\
                -I$(DIS_SRC)/SISCI/src							\
                -I$(DIS_SRC)/SISCI/src/LINUX					\

EXTRA_CFLAGS += ${CPPFLAGS}


all:
	cp -f ${DIS_SYMBOLS} ./ || :
	cat ${KOSIF_SYMBOLS} >> ./Module.symvers || :
	cat ${SCI_SYMBOLS} >> ./Module.symvers || :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

install:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

ins:
	sudo dmesg -C
	sudo insmod $(DIS_SRC)/SCI_SOCKET/scilib/GENIF/LINUX/dis_msq.ko
	sudo insmod sci_ktest.ko local_adapter_no=0 remote_node_id=99 is_server=N
	dmesg

rm: 
	sudo dmesg -C
	sudo rmmod sci_ktest.ko
	sudo rmmod $(DIS_SRC)/SCI_SOCKET/scilib/GENIF/LINUX/dis_msq.ko
	dmesg

test: ins rm

retest: clean all test