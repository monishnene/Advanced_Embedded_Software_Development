CFLAGS = -Wall -g -I../inc
OBJECTS = testlib.o
 

ifeq ($(PLATFORM),ARM)
CC=arm-cortex_a8-linux-gnueabihf-gcc
endif

ifeq ($(PLATFORM),QEMU)
CC=arm-unknown-linux-gnueabi-gcc
endif


all: libtest.a

# Build the static library
libtest.a: $(OBJECTS)
	ar rc libtest.a $(OBJECTS)

testlib.o: testlib.c
	$(CC) $(CFLAGS) -c testlib.c

clean:
	rm -f $(OBJECTS)
	rm -f libtest.a
