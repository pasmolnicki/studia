# Generate shared library for the c code
CC = gcc
CFLAGS = -fPIC -shared
FLAGS = -lm
LDFLAGS = -L. -lnums
TARGET = libnums.so
SRCS = nums.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
%.o: %.c
	$(CC) -c -fPIC $< -o $@
clean:
	rm -f $(OBJS) $(TARGET)
link: main.c $(TARGET)
	$(CC) main.c -o main -L. -lnums $(FLAGS)