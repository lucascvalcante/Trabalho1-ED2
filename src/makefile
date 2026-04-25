CC = gcc
CFLAGS = -std=c99 -fstack-protector-all -Wall -Wextra -g -DUNITY_INCLUDE_DOUBLE
LIBS = -lm


MODULES = $(filter-out unity.c main.c, $(wildcard *.c))
OBJ_MODULES = $(MODULES:.c=.o)


TEST_SRCS = $(wildcard tst/t_*.c)

TEST_BINS = $(patsubst tst/t_%.c, t_%, $(TEST_SRCS))

ted: $(OBJ_MODULES) main.o
	# $(CC) $(CFLAGS) main.o $(OBJ_MODULES) -o ted $(LIBS)
	@echo "Executável 'ted' seria gerado aqui (aguardando main.c)"
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
t_%: %.o unity.o tst/t_%.c
	$(CC) $(CFLAGS) tst/t_$*.c $*.o unity.o -o $@ $(LIBS)
	@echo "\n--- Executando Teste: $* ---"
	./$@

tstall: $(TEST_BINS)
	@echo "\n--- Todos os testes detectados foram finalizados! ---"

clean:
	rm -f *.o ted t_*