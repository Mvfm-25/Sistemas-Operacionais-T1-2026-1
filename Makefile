SHELL   = /bin/bash
CC      = gcc
CFLAGS  = -Wall -O0 -pthread
TARGET  = exec/counter
SRC     = source/counter.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

run: $(TARGET)
	@echo "--- T1 (sem sync) ---"
	@for n in 2 4 8; do time ./$(TARGET) T1 $$n; done
	@echo "--- T2 (mutex) ---"
	@for n in 2 4 8; do time ./$(TARGET) T2 $$n; done
	@echo "--- P1 (processos sem sync) ---"
	@for n in 2 4 8; do time ./$(TARGET) P1 $$n; done
	@echo "--- P2 (processos + semaforo) ---"
	@for n in 2 4 8; do time ./$(TARGET) P2 $$n; done
