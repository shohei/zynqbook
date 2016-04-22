INCS += -I /usr/lib/jvm/java-6-sun/include/
INCS += -I /usr/lib/jvm/java-6-sun/include/linux

OPTS =-Wno-psabi 
OPTS += -O2
CFLAGS += $(INCS) -fPIC $(OPTS)
CXXFLAGS += $(INCS) -fPIC $(OPTS)
PREFIX=arm-none-linux-gnueabi-
CC=$(PREFIX)gcc
CXX=$(PREFIX)g++

all: liblogicBRICKS.so logicBRICKS.class Test.class

liblogicBRICKS.so:logicBRICKS.o logiBMP.o
	$(CC) -shared -o liblogicBRICKS.so logicBRICKS.o logiBMP.o -lsupc++ -lgcc_s

logicBRICKS.o:logicBRICKS.h logicBRICKS.cpp
#logicBRICKS.o:logicBRICKS.h logicBRICKS-c.cpp
#	$(CXX) $(CXXFLAGS) -c -o logicBRICKS.o logicBRICKS-c.cpp

logicBRICKS.h:logicBRICKS.class
	javah -jni logicBRICKS
	touch logicBRICKS.h

logicBRICKS.class:logicBRICKS.java
	javac logicBRICKS.java

Test.class:Test.java logicBRICKS.class
	javac Test.java

