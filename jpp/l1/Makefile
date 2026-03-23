# Generate shared library for the c code
CC = gcc
CFLAGS = -fPIC -shared
SHRD_TARGET = libnums.so
STATIC_TARGET = libnums.a
LDFLAGS = -L. -lnums
SRCS = nums.c
OBJS = $(SRCS:.c=.o)

ADA_NUMS = ada_nums.adb
ADA_OBJS = $(ADA_NUMS:.adb=.o)
ADA_ALI = $(ADA_NUMS:.adb=.ali)
ADA_LIB = libada_nums.a

RUST_LIB = rust-nums/target/release/librust_nums.a
RUST_LDFLAGS = -Lrust-nums/target/release -lnums

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(SHRD_TARGET) $(STATIC_TARGET) main \
	main_ada main_ada_c main_ada_rust main_ada.o main_ada.ali \
	main_ada_rust.o main_ada_rust.ali b~* \
	ada/*.ali ada/$(ADA_OBJS) $(ADA_LIB) ada/b~* \
	nums_c_bindings.* nums_rust_bindings.* main_rust_c

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

c-rust-link: rust-nums/target/release/libnums.a
#	Compile the rust library
	cd rust-nums && cargo build --release && cd ..
#	Compile the main program
	$(CC) main.c -o main $(RUST_LDFLAGS) $(FLAGS)

c-ada-link: ada/$(ADA_NUMS)
#	Compile the ada library
	gnatmake -c ada/$(ADA_NUMS)
	mv $(ADA_ALI) ada/$(ADA_ALI)
	mv $(ADA_OBJS) ada/$(ADA_OBJS)
	gnatbind -Lnums ada/ada_nums
	gcc -c b~$(ADA_NUMS) -o ada/b~$(ADA_OBJS)
	ar rcs $(ADA_LIB) ada/$(ADA_OBJS) ada/b~$(ADA_OBJS)
# 	rm -rf ada/nums.ali ada/b~nums.ali b~*
#	Compile the main program
	$(CC) main.c -o main -L. -lada_nums -lgnat $(FLAGS)

ada-rust-link: rust-nums/target/release/libnums.a
#	Compile the rust library
	cd rust-nums && cargo build --release && cd ..
#	Compile and link the Ada program with Rust bindings
	gnatmake -Iada main_ada_rust -o main_ada_rust -largs $(RUST_LDFLAGS) -lgnat

ada-c-link: nums.c
#	Compile the C library
	$(CC) -c -o nums.o nums.c
	ar rcs $(STATIC_TARGET) nums.o
#	Compile and link the Ada program with C bindings
	gnatmake -Iada main_ada -o main_ada_c -largs -L. -lnums -lgnat

ada-link: main.adb
#	Compile and link the Ada program with C and Rust bindings
	$(CC) -c -o nums.o nums.c
	ar rcs $(STATIC_TARGET) nums.o
	cd rust-nums && cargo build --release && cd ..
#	Compile and link the Ada program with C and Rust bindings
	gnatmake -Iada main_ada -o main_ada -largs -L. -lnums $(RUST_LDFLAGS) -lgnat

rust-c-link: main.rs
#	Compile the C library
	$(CC) -c -o nums.o nums.c
	ar rcs $(STATIC_TARGET) nums.o
#	Compile the Rust program (main.rs) with C bindings
	rustc main.rs -o main_rust_c -l static=nums -L.