CC=gcc
CFLAGS=-I.
OBJ = main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fismos_dev_fismos_instruction_register_app: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o fismos_dev_fismos_instruction_register_app