# $Id: Makefile,v 1.1 2005/07/28 15:03:50 carl Exp $
# Build EV Grid IO test program

OBJ = testEVGridIO.o readEVGrid.o writeEVGrid.o ioEVGrid.o

testEVGridIO: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean: 
	rm -f testEVGridIO $(OBJ)
