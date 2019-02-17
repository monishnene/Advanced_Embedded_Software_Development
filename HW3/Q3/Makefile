obj-m += kernel_timer.o

PATH1=~/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220
PWD=$(shell pwd)
PATH2=~/buildroot/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-
PATH3=/lib/modules/$(shell uname -r)/build

ifeq ($(PLATFORM),BBG)
	
	build_command=make	ARCH=arm	CROSS_COMPILE=$(PATH2)	-C	$(PATH1)	M=$(PWD)	modules
	clean_command=make    -C      $(PATH1)        M=$(PWD)        clean
else
	build_command=make	-C	$(PATH3)	M=$(PWD)	modules
	clean_command=make	-C	$(PATH3)	M=$(PWD)	clean
endif

.PHONY:	build\
	clean\
	send

build:
	$(build_command)

clean:
	$(clean_command)

send:
	scp	$1	root@$2:/
