	common_sources=	data_structure.c
	
	common_includes = 

	common_flags =	-g \
			-O0 \
			-lpthread\
			-pthread \
			-lrt \
			-lm \
			-lc

	CC=gcc
	flags =	$(common_flags)
	sources =	$(common_sources)
	includes =	$(common_includes)
	objects:= $(sources:.c=.o)
	deps:= $(sources:.c=.d)	

	ifeq ($(PLATFORM),ARM)
		CC =arm-cortex_a8-linux-gnueabihf-gcc
	endif

	ifeq ($(PLATFORM),QEMU)
		CC=arm-unknown-linux-gnueabi-gcc
	endif

	ifeq ($(PLATFORM),BBG)
		CC =~/buildroot/output/host/usr/bin/arm-linux-gcc
	endif

.PHONY:	compile-all \
	clean \
	build \
	run \
	local\
	strace\
	ltrace\
	perf\
	trace_all\
	send

build:	data_structure.elf	

%.elf:	$(objects)	$(deps)	
	$(CC)	$(flags)	-o	$@	$(objects)

%.i:%.c		$(includes)
	$(CC)	$(flags)	-E	$<	-o	$@

%.asm:%.c	$(includes)
	$(CC)	$(flags)	-S	$<	-o	$@

%.o:%.c		$(includes)
	$(CC)	$(flags)	-c	$<	-o	$@

%.o:%.s		$(includes)
	$(CC)	$(flags)	-c	$<	-o	$@
	
%.d:%.c		$(includes)
	$(CC)	$(flags)	-M	$<	-o	$@	

%.d:%.s		$(includes)
	$(CC)	$(flags)	-M	$<	-o	$@

compile-all:	$(objects) 

clean :
	rm	-f	*.o	*.s	*.i	*.o	*.d	*.elf *.swp

strace:
	sudo	strace	-C	-r	-tt	-T	-o	strace_fileio_log	./fileio.elf commandline_test_string 

ltrace:
	sudo	ltrace	-n4	-T	-r	-S	-o	ltrace_fileio_log	./fileio.elf commandline_test_string 

perf:
	sudo	perf	stat	-o	perf_fileio_log	./fileio.elf commandline_test_string 

trace_all:	perf	ltrace	strace

send:
	scp fileio.elf	root@10.0.0.133:/

run:
	./data_structure.elf Tiger Dog  Cat Elephant Frog Goat Sheep Lion Eagle Shark Whale Dolphin Rhino Panda Bear  Monkey Pig Giraffe Zebra Horse Rabbit Wolf Chicken Buffalo

