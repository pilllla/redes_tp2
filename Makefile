
CC = gcc
CFLAGS = -Wall -g -std=c99
# LDFLAGS is for linking external libraries (e.g., -pthread for multi-threading)
# LDFLAGS = -pthread

TARGETS = client server
COMMON_OBJS = utils.o

.PHONY: all
all: $(TARGETS)

$(TARGETS): %: %.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c utils.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGETS) $(patsubst %, %.o, $(TARGETS)) $(COMMON_OBJS)