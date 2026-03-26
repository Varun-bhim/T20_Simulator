CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = t20_simulator

SRCS = main.c globals.c threads.c match.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c simulator.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)