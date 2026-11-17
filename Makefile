# Makefile do TP2 - Ordenacao Externa (BCC203) - branch metodo2
#
# Fontes (.c) e cabecalhos (.h) ficam na raiz do projeto.
# Arquivos gerados vao para:
#   objects/       objetos (.o)          [criada pelo make]
#   dependencies/  dependencias (.d)     [criada pelo make]
#
# Alvos:
#   make                         compila o executavel 'ordena'
#   make debug                   recompila com -g -O0 (gdb/valgrind)
#   make run ARGS="2 100 3 -P"   compila e executa ./ordena 2 100 3 -P
#   make clean                   apaga objetos, dependencias, executavel e fitas
#
# Todo .c da raiz entra na compilacao automaticamente (metodo2.c, etc.).
# Subpastas como src/ e app/ sao ignoradas.

CC      := gcc
LDFLAGS :=
LDLIBS  :=

OBJ_DIR := objects
DEP_DIR := dependencies

EXEC := ordena

CFLAGS  := -I . -I ./includes -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -O2

SRCS := $(wildcard *.c) $(wildcard src/doisf.c)
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))
DEPS := $(patsubst %.c,$(DEP_DIR)/%.d,$(notdir $(SRCS)))
vpath %.c src

.PHONY: all debug run clean

all: $(EXEC)

debug: CFLAGS := $(filter-out -O2,$(CFLAGS)) -g -O0
debug: clean $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# -MMD -MP: gera objects/x.o e dependencies/x.d numa unica chamada do gcc.
# "| $(OBJ_DIR) $(DEP_DIR)" sao pre-requisitos de ordem: garantem que as
# pastas existam sem forcar recompilacao quando a data delas mudar.
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MMD -MP -MF $(DEP_DIR)/$*.d -c -o $@ $<

$(OBJ_DIR) $(DEP_DIR):
	mkdir -p $@

run: $(EXEC)
	./$(EXEC) $(ARGS)

clean:
	$(RM) -r $(OBJ_DIR) $(DEP_DIR) $(EXEC) fita*.tmp

-include $(DEPS)
