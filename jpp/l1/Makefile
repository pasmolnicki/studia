# Generate shared library for the c code
CC = gcc
CFLAGS = -fPIC -shared
SHRD_TARGET = libnums.so
STATIC_TARGET = libnums.a
LDFLAGS = -L. -lnums
SRCS = nums.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(SHRD_TARGET) $(STATIC_TARGET) main \
	ada/nums.o ada/b~nums.o ada/libnums.a ada/nums.ali ada/b~nums.ali ada/b~* b~*

c-static: $(SRCS)
# Compile the C library (as static) then the main program
	$(CC) -c -o nums.o nums.c
	ar rcs $(STATIC_TARGET) nums.o
	$(CC) main.c -o main $(LDFLAGS) $(FLAGS)


c-shared: main.c $(SRCS)
# Compile the C library as shared then the main program
	$(CC) -fPIC -c -o nums.o nums.c
	$(CC) -shared -o $(SHRD_TARGET) nums.o
	$(CC) main.c -o main $(LDFLAGS)


rust-link: rust-nums/target/release/libnums.a
#	Compile the rust library
	cd rust-nums && cargo build --release && cd ..
#	Compile the main program
	$(CC) main.c -o main -Lrust-nums/target/release -lnums $(FLAGS)

ada-link: ada/nums.adb
#	Compile the ada library
	gnatmake -c ada/nums.adb
	mv nums.ali ada/nums.ali
	mv nums.o ada/nums.o
	gnatbind -Lnums ada/nums
	gcc -c b~nums.adb -o ada/b~nums.o
	ar rcs libnums.a ada/nums.o ada/b~nums.o
# 	rm -rf ada/nums.ali ada/b~nums.ali b~*
#	Compile the main program
	$(CC) main.c -o main -L. -lnums -lgnat $(FLAGS)