objects = shell.o

edit : $(objects)
	cc -o edit $(objects)

shell.o : header.h

.PHONY : clean
clean :
	rm edit $(objects)
        
