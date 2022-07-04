CC=gcc
CFLAGS=-I.
OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fismos_linux_app: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o fismos_linux_app