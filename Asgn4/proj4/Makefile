cpmRun: diskSimulator.o  cpmfsys.o fsysdriver.o 
	gcc -o cpmRun diskSimulator.o cpmfsys.o fsysdriver.o -I. -std=c99

diskSimulator.o: diskSimulator.c diskSimulator.h
	gcc -c diskSimulator.c

cpmfsys.o: cpmfsys.h cpmfsys.c 
	gcc -c cpmfsys.c  

fsysdriver.o: fsysdriver.c
	gcc -c fsysdriver.c 

all: 
	cpmRun

clean: 
	rm *.o 

