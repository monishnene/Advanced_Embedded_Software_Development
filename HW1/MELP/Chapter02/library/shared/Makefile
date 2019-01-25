#makefile for the gcc desktop linux environment
#PIC=Position independent code for x86 architectures
CFLAGS := -Wall -g -fPIC -I../inc
OBJECTS := testlib.o
 
all: libtest.so

# Build the shared library
libtest.so: $(OBJECTS)
	$(CC) -shared -o libtest.so $(OBJECTS)

testlib.o: testlib.c
	$(CC) $(CFLAGS) -c testlib.c

clean:
	rm -f $(OBJECTS)
	rm -f libtest.so
