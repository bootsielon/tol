CC = g++
CCFLAGS = -Wall -g2 -I/usr/local/gsl/include
LDFLAGS = -L/usr/local/gsl/lib -lgsl -lgslcblas 

p2: p2.cc llkarma.o 
	$(CC) $(CCFLAGS) -o p2 p2.cc llkarma.o $(LDFLAGS)


p: p.cc llkarma.o 
	$(CC) $(CCFLAGS) -o p p.cc llkarma.o $(LDFLAGS)

prueba: prueba.cc llkarma.o 
	$(CC) $(CCFLAGS) -o prueba prueba.cc llkarma.o $(LDFLAGS)

llkarma.o: llkarma.h llkarma.cpp
	$(CC) $(CCFLAGS) -c  llkarma.cpp

