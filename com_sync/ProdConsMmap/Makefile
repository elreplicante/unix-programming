CFLAGS  = -Wall -g -pthread
FUENTE1 = productor.c
FUENTE2 = consumidor.c
PROG1   = Productor
PROG2   = Consumidor

all :
	gcc $(CFLAGS) -o $(PROG1) $(FUENTE1)
	gcc $(CFLAGS) -o $(PROG2) $(FUENTE2)

clean :
	-rm $(PROG1) $(PROG2)
