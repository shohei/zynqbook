INCS += -I /usr/lib/jvm/java-6-sun/include/
INCS += -I /usr/lib/jvm/java-6-sun/include/linux

CFLAGS += $(INCS) -fPIC
CC=gcc

all: libHelloWorld.so

libHelloWorld.so:HelloWorld.o
	$(CC) -shared -o libHelloWorld.so HelloWorld.o

