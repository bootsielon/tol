CC = g++
CCFLAGS =  -Wall -O3 -funroll-all-loops
LDFLAGS = -lgsl -latlas -lcblas

prueba: prueba.cc LLKKFSS.o gsl_linalg_ext.o gsl_util_ext.o
	$(CC) $(LDFLAGS) $(CCFLAGS) -o prueba prueba.cc LLKKFSS.o  gsl_linalg_ext.o gsl_util_ext.o


LLKKFSS.o: LLKKFSS.h LLKKFSS.cc
	$(CC) $(CCFLAGS)   -c  LLKKFSS.cc


gsl_linalg_ext.o: gsl_ext.h gsl_linalg_ext.c 
	$(CC) $(CCFLAGS)   -c gsl_linalg_ext.c 

gsl_util_ext.o: gsl_ext.h gsl_util_ext.c 
	$(CC) $(CCFLAGS)   -c gsl_util_ext.c 



